#!/usr/bin/python3
# Tilth.py
# a PHP workflow-in-a-box with simple package management.
# Copyright 2014 Nathan Ross (nrossit2@gmail.com)
#
# Licensed under the Apache 2 License
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import os
import subprocess
import glob
from os import path
import shutil
import sys
import csv
import re
import copy
import tempfile
import argparse
import subprocess

instructions = """
  To use throw52:

  for every function which might be on the stack when an exception
  is thrown (aka might throw an exception, call a function which
  throws an exception, call a function which calls a function which etc.)
  annotate it by creating a line right above it that begins with 
  the PREPEND constant below (e.g. //throw_)

  Go through the source code looking for instances where these
  exception-throwing functions are called. throw52 works when the
  function calls are:
     1. called on a line with no other statements
     2. has the return value unused
      OR 
        has the return value immediately assigned to a variable
      OR
        is called after a 'return' keyword (with no modification)
         so that the returned value is immediately returned.
     3. best practice is to enclose any code you want ignored
        in throw52 in a preprocessor block that is NOT an else block.
        (use IF, IFDEF, IFNDEF, or ELSEIF instead)
        Because you may now or later want multiple else blocks
        in your code that pertain to different conditions' negative.

   You should go over your code manually to for #2 and remove
   all violators. Throw52 will detect some, but others will 
   lead to type errors and syntax errors.
   
   Main violators to look for
      A.calling an exception-throwing function as an argument
        to pass to a function call. 
            f.g(h());
        instead assign it to a temporary variable
            int tmp = h(); 
            f.g(tmp);

        If the variable returned is large (E.g. a vector) and
        copying is expensive, you can use preprocessor blocks
        to keep the direct as-argument call for when you're 
        not processing with throw52.

        If you miss some of these, throw52 will detect some instances
        but not all. In fact if an exception-throwing function
        is used as an argument on a line other than the outer function
        call, it will break the code and cause a compile error.

          f.g("abc",
              h());              
        
      B.modifying the result before assignment.
            int num = h() + 5;
            return g() / 2;
        instead assign it in its own line and then modify the result
            int num = h();
            num += 5;
            int tmp = g();
            return tmp + 5;
        
        This is usually something that throw52 will not detect as an error
        and will come up as a type error on compile time. 

      C. using ternary assignment.
             name = (last)? getFirst() : getLast();
         instead use an if else block
             if (last) {
                 name = getFirst();
             else {
                 name = getLast();
             }
         
         This will sometimes be detected by throw52, and some other 
         times cause type errors, but most of the time cause syntax
         errors. Ensure you have replaced all ternary assignment
         by searching for question marks.
        


"""


class _Logger:
    FATAL = 0
    ERROR = 1
    WARN = 2
    DEFAULT = 2
    NOTE = 3
    OUTLINE = 4
    VALUES = 5
    DEBUG = 6
    def __init__(self, verbosity):
        self.verbosity = verbosity
        
    def log(self, level, msg):
        if self.verbosity >= level:
            prefix = ""
            suffix = ""
            if level == _Logger.FATAL:
                prefix = "FATAL ERROR: "
                suffix = ". Exiting."
            elif level == _Logger.ERROR:
                prefix = "ERROR: "
            elif level == _Logger.WARN:
                prefix = "WARNING: "
                suffix = ""                
            sys.stderr.write(prefix + msg + suffix + "\n")
            if level == _Logger.FATAL:
                sys.exit()
                        
_logger = _Logger(1)




    #instructions will include an extra column for a minify bit, 
    # which will only be valid with anything above NO_REQUEST
    # actually wait, no i need to think about this more.
    # maybe minify should be ternify with NO_REQUEST, NO_MINIFY, MINIFY?
    #	I don't think the users want to keep track of two separate overlapping
    # trees of inheritance in their head. perhaps the original method is best.
    
def _getDefaultParams():
    return {
        # capitalized parameters are parameters that for most users
        # won't be that useful to know about (vs. say, knowing how to
        # use debug mode for rapid testing that you've set up
        # includes and tilthSrc() calls right)
        
        #when changing these, be sure to edit corresponding value
        #in argparse cmd-line code (at bottom)
        "verbosity": _Logger.DEFAULT,
        "igblock_open": "#IFDEF THROWABLE",
        "igblock_close": "#ENDIF",
        "throws_indicator":"#?throw_",
        "error_class":"ErrWrap",
        "extra_tests":False,
        "use_templates":False,
        "gen_ec":False
        #templates: ErrWrap<int>
        #no templates: ErrWrapint
    }

def _createTask(instream, outstream, paramdeltas={}):
    params = _getDefaultParams()
    for k in paramdeltas: 
        params[k] = paramdeltas[k]
    return Throw52Task(instream, outstream, params)

class Throw52Task:
    def __init__(self, instream, outstream, params):
        # on creation, stores a snapshot of parameters
        
        #behavior which should be able to be applied to some blocks and 
        # files but not others should be integrated into instructions
        # rather than through piecewise changing of configuration.
    
        self._params = copy.deepcopy(params)
        
        global _logger
        _logger.verbosity = self._params['verbosity']
        
        # this program doesn't aspire to deal with C++ files > 100k lines,
        # so storing the text in memory is fine
        # text is stored as an array of the lines of the program.
        self.text = instream.read().split("\n")
        self.processCalls()
        outstream.write("\n".join(self.text))

    def stripToCode(self, line):
        return (line.split("//")[0]).rstrip()

    def checkUnusableLine(self, line, num, whyUnusable):
        #making this optional and disabled by default saves us a serious amount of time,
        #otherwise for a 5k size file we're calling ~50 regex searches on every second or third line.
        if not self._params['extra_tests']:
            return
        for f in self.f_map:
            if line.find(f) and re.search(f + "\s*\(", line):
                _logger.log(_Logger.ERROR,
                            str(num) + ": exception throwing func '" + f + "' is present in line " + \
                            " but " + whyUnusable + " output not tested! must be fixed.")
          
    def processCalls(self):
        #ASSUMES:
        #1. left var and assignment occurs on the SAME line as the function call
        # (though the function call arguments may be spread over several lines
        #2. all assignment operators assign simple function call results, NOT ternary operators.
        _logger.log(_Logger.OUTLINE, "----processCalls()----")
        text = self.text
        varnum = 1 
        f_map = {}
        void_funcs = [ x for x in f_map if f_map[x] == 'void' ]
        asign_funcs = [ x for x in f_map if f_map[x] != 'void' ]
        ident = "[a-zA-Z_](?:[a-zA-Z_0-9]*|[a-zA-Z_0-9]*\.[a-zA-Z_][a-zA-Z_0-9]*)?"
        typestr = "[a-zA-Z_](?:[a-zA-Z_0-9\*]*|[a-zA-Z_0-9]*<[a-zA-Z_0-9\*\s]+>\*?)?"
        #'a', 'ab', and 'a.b' are all valid. 'a.' is not,
        #ident = "[a-zA-Z_]"  #'a', 'ab', and 'a.b' are all valid. 'a.' is not,
        re_header = re.compile("\s?//\s?" + self._params['throws_indicator'])
        re_signature = re.compile("^\s?(?P<rettype>"+typestr+")\s[a-zA-Z_][a-zA-Z0-9_:]*\(.*?")
        re_func_name = re.compile("(" + ident + ")\s*\(")
        void_call = re.compile("^\s*(" +ident +")\s*\([^;]*;?")
        assign_call = re.compile("^\s*(?P<normalvar>(?:" + ident + "[\*\s]+" + ident + "|" + ident + "))\s*=\s*(" +ident +")\s*\([^;]*;?")
        ERROR_CLASS=self._params['error_class']

        ident_sig = "[a-zA-Z_][a-zA-Z_0-9]*"
        re_sig = re.compile("^\s?(?P<ret>" + typestr + "[\s\*]*?)\s*" \
                            "(?:" + ident_sig + "\s+)?"
                                     "(?P<class>(?:" + ident_sig + \
                                     "::)?)(?P<name>"+ ident_sig + \
                                     ")\((?P<rest>.*)")

        AFTER_SEMICOLON = "it occurs after a semicolon (against throw52 convention)"
        ASSIGN_OF_VOID = " is of void return but may be assigned to a var in this line. skipping."
        DBG_ASSIGNED = " function result assigned to a var in source "
        DBG_NOT_ASSIGNED = " function result NOT assigned to a var in source "
        STATEMENT_NUM = " is called in this line but is not the only statement in the line (throw52 convention expects it to be). skipping."

        begun_call = False
        line = ""
        finish_call = ""

        igblock_open = self._params['igblock_open']
        igblock_close = self._params['igblock_close']
        in_ignored_block = False

        outer_is_void = False
        outer_ret_type = ""
        outer_throws = False
        funcname = ""

        outer_func_name = ""
        #good for generating list of error classes needed.
        gen_ec = self._params['gen_ec']
        error_class_set = set()
        returned_class_set = set()

        use_templates = self._params['use_templates']
        no_wrap = False

        for i in range(0, len(text)):
            line = self.stripToCode(text[i])
            if in_ignored_block:
                if (line.upper()).find(igblock_close) != -1:
                    in_ignored_block = False
                continue
            if (line.upper()).find(igblock_open) != -1:
                in_ignored_block = True
                continue    
            m = re.match(re_sig, text[i])
            if m:
                newret = m.group("ret")
                #this assignshouldn't ever be used anyway... 
                #no need to know ret. type if no throwing func calls w/in.
                outer_ret_type = newret 
                outer_is_void = (newret == 'void')                
                outer_throws = re.match(re_header, text[i-1])
                outer_func_name = m.group('name')
                if outer_throws:     
                    if m.group('class'):
                        raise "don't know what to do with throwing class member"
                    f_map[m.group('name')] = {'name':"", 
                                              'wrap':True, 
                                              'void':False}
                    if outer_is_void:
                        f_map[m.group('name')]['void'] = True
                        newret = 'int'

                    if newret[-1] != "*" and newret[0].upper() == newret[0]:
                        f_map[m.group('name')]['wrap'] = False
                        returned_class_set.add(newret)
                        outer_ret_type = newret
                    elif use_templates:
                        outer_ret_type = "".join([self._params['error_class'],
                                              '<', newret, '>' ])
                    else:
                        if gen_ec:
                            error_class_set.add(newret)
                        newret = newret.replace("<","")
                        newret = newret.replace(">","")
                        newret = newret.replace(" ","")
                        newret = newret.replace("*", "Ptr")

                        outer_ret_type = "".join([self._params['error_class'],
                                                  newret])
                    f_map[m.group('name')]['name'] = outer_ret_type
                    text[i] = re.sub(re_sig, 
                                     outer_ret_type + \
                                     " \g<class>\g<name>(\g<rest>", 
                                     text[i])
                continue
            if not outer_throws and not self._params['extra_tests']:
                continue

            if begun_call:
                pos = line.find(";")
                if pos >= 0:
                    #_logger.log(_Logger.VALUES, str(i) + " found statement end ")
                    text[i] = "".join([text[i][:pos+1], " ", 
                                       finish_call, 
                                       text[i][pos+1:] ])
                    line = line[pos+1:]
                    if outer_is_void: #if we're here outer throws by definition.
                        if line[pos+1:].find("return") != -1:
                            text[i] = text[i].replace(" return ", 
                                                      " return noErr")
                            text[i] = text[i].replace(" return;", 
                                                      " return noErr;")
                    self.checkUnusableLine(line[pos+1:], i, 
                                           AFTER_SEMICOLON)
                    begun_call = False
                self.checkUnusableLine(line, i, 
                                       "".join([" call to funcname ",
                                                funcname,
                                                " hasn't closed "]))
                continue

            if outer_throws and outer_is_void:
                if text[i].find("return") != -1:                        
                    text[i] = text[i].replace(" return ", " return noErr")
                    text[i] = text[i].replace(" return;", " return noErr;")
            elif outer_is_void:
                print(" no such thing ")
            if begun_call or line.find("(") == -1:
                continue

            newvar = "th52" + str(varnum)
            varnum += 1 
            m = re.search(re_func_name, line)
            if not m:
                continue
            funcname = m.group(1)
            if not (funcname in f_map):
                _logger.log(_Logger.DEBUG, 
                            "".join([str(i),
                                     " func found, but not in f_map: '",
                                     funcname, "'"]) )                    
                if not (funcname == "DEBUGRET"):
                    self.checkUnusableLine(line, i, 
                                           " is not the first function call.")
                continue
            _logger.log(_Logger.VALUES, 
                        "".join([str(i),
                                 " throwable func found: '",
                                 funcname,
                                 "'"]))                    
            callret_type = f_map[funcname]['name']
            use_wrap = f_map[funcname]['wrap']
            is_void = f_map[funcname]['void']
            if line.find("=") != -1 and is_void:
                _logger.log(_Logger.WARN, 
                            "".join([str(i), 
                                     ": error-throwing function ",
                                     funcname,
                                     ASSIGN_OF_VOID]) )
            retvar = "th52" + str(varnum)
            #we don't need to wrap the signature's ret type
            #because we already wrapped that in the 
            #signature processing pass.
            varnum += 1
            finish_call = "".join([" if (", newvar, ".err) { ", 
                                   outer_ret_type, " ", retvar, "; ", 
                                   retvar, ".err = true; ",
                                   #"DEBUGOUT(\"",
                                   #outer_func_name,
                                   #"\", false); ",
                                   " return ", 
                                   retvar, "; } "])
            m = re.match(void_call, line)
            
            if m:
                _logger.log(_Logger.DEBUG, 
                            "".join([str(i),
                                     DBG_NOT_ASSIGNED]))
                pos = line.find(funcname)
                text[i] = "".join([text[i][:pos],
                                   callret_type, " ",
                                   newvar, " = ",
                                   text[i][pos:] ])
            elif not line.find("="):
                _logger.log(_Logger.ERROR,
                            "".join([str(i),
                                     ": error-throwing function ",
                                     funcname,
                                     STATEMENT_NUM]))
                continue
            else:
                _logger.log(_Logger.DEBUG,
                            "".join([str(i),
                                     DBG_ASSIGNED]))
                m = re.match(assign_call, line)
                if not m:
                    _logger.log(_Logger.ERROR,
                                "".join([str(i),
                                         " error-throwing function ",
                                         funcname,
                                         STATEMENT_NUM]))
                    continue
                normalvar = m.group('normalvar')
                if use_wrap:
                    text[i] = text[i].replace(normalvar, 
                                              callret_type + \
                                              " " + newvar)
                    finish_call = "".join([finish_call, 
                                           normalvar, " = ", 
                                           newvar, ".val; "])
                else:                    
                    ensure_no_type = normalvar.split(" ")[-1]
                    finish_call = "".join([" if (", 
                                           ensure_no_type, 
                                           ".err) { ", 
                                   outer_ret_type, " ", retvar, "; ", 
                                   retvar, ".err = true; ",
                                   #"DEBUGOUT(\"",
                                   #outer_func_name,
                                   #"\", false); ",
                                   " return ", 
                                   retvar, "; } "])
            if line[-1] == ';':
                _logger.log(_Logger.VALUES, str(i) + \
                            " found statement end (sameline) ")
                text[i] = "".join([self.stripToCode(text[i]),
                                   " ", finish_call, 
                                   text[i][len(self.stripToCode(text[i])):] ])
                finish_call = "DEFAULT_FINISH_CALL"
            else:
                begun_call = True
        if not gen_ec:
            return
        print("// ensure there are bool .err members, initialized with false\n" +\
              "// in the following classes:")
        for rc in returned_class_set:
            print("// "+rc)
        print("")
        
        for typename in error_class_set:
            typecleaned = typename.replace("<", "")
            typecleaned = typecleaned.replace(">", "")
            typecleaned = typecleaned.replace(" ", "")
            typecleaned = typecleaned.replace("*", "Ptr")
            print("".join(["class ",
                           ERROR_CLASS, typecleaned,
                           " {\npublic:\n    ",
                           "bool err;\n    ",
                           typename, " val;\n    ",
                           ERROR_CLASS, typecleaned, 
                           "() {\n        ",
                           "err = false;\n    }\n    ",
                           ERROR_CLASS, typecleaned,
                           "(", typename, " in) {\n        ",
                           "val = in;\n        ",
                           "err = false;\n    }\n"
                           "};\n"]))
                

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Throw52 web workflow")
    
    # defaults are manually copied here to contribute to convention of reviewing
    #	argparse args for possible needed changes when changing defaults.
    #	in most cases, it wouldn't be alright to just change the output
    #	of just getDefaultParams.
    parser.add_argument('infile', nargs='?', type=argparse.FileType('r'),
                        default="/home/n/coding/esp3/src/linprima.cpp")
                        #default=sys.stdin)
    parser.add_argument('outfile', nargs='?', type=argparse.FileType('w'),
                        default="/home/n/coding/esp3/tmp/linprima.cpp")
                        #default=sys.stdout)
    parser.add_argument("-v ", type=int, choices=range(0,6), dest="verbosity",
                    default=0,
                    help="verbosity. 0 is only display errors, 1 is default (warnings), 5 is max")
    parser.add_argument("-e","--extra_tests", dest="extra_tests",
                    action="store_true",default=False,
                        help="check more thoroughly for potential sources of processing errors. You should use this " + \
                        "option at least once if you make any heavy additions or refactors")
    parser.add_argument("-t","--use_templates", dest="use_templates",
                    action="store_true",default=False,
                        help="use templates with the error class (e.g. ErrWrap<int> instead of ErrWrapint) advantages: don't need to include variant of each class in source. disadvantage: slower performance.")

    parser.add_argument("-g","--generate_errorclasses", dest="gen_ec",
                    action="store_true",default=False,
                        help="If you're not using templates, you will want to include code in your source with a wrapping errorclass for each return type of an exception-throwing function. This option will output that code to stdout. implies no templates")

    parser.add_argument("--ignored-block-opener", dest="igblock_open",
    action="store", default="#IFDEF THROWABLE",
	help=" line that if detected, throw52 will skip all lines of code " + \
          "until it reaches the ignored-block-closer")
    parser.add_argument("--ignored-block-closer", dest="igblock_close",
    action="store", default="#ENDIF",
	help=" line that closes a block of code to be ignored by throw52")

    prev="""parser.add_argument("-c","--compress-instructions", dest="comp_instr",
	action="store_true",default=False,
	help="generate in the current directory an instructions .csv with " + \
		" only the input instructions which are not NO_REQUEST " + \
		" and that refer to an existing file.")
parser.add_argument("-d","--debug-mode", dest="debug_mode",
	action="store_true", default=False,
	help="skip any requested minifying of files for faster testing")
parser.add_argument("-g","--generate-instructions", dest="exp_instr",
	action="store_true",default=False,
	help="generate in the current directory an instructions .csv with " + \
		" the input instructions + all other source files.")
parser.add_argument("-j","--joomla-mode", dest="joomla_mode",
	action="store_true",default=False,
	help="rewrite all urls relative to the lowest shared directory," + \
	"instead of relative to the current index file.")
parser.add_argument("--maincache-dir", dest="maincache_dir",
	action="store", default=".",
	help=" relative path within the prod. directory to store the main " + \
		"(site-wide) cache. Default is '.'")
parser.add_argument("-p","--production-folder", dest="production_folder",
	action="store", default="_c",
	help=" name of production folder (where all output is stored). " + \
		" default is _c")
parser.add_argument("--path-closure", dest="path_closure",
	action="store", default="minify/compiler.jar",
	help=" path to closure compiler .jar file. Assumed " + \
		"'minify/compiler.jar'. If both yui and closure are used," + \
		" closure is preferred for js. You need at least one to minify js.")
parser.add_argument("--path-yui", dest="path_yui",
	action="store", default="minify/yuicompressor*.jar",
	help=" path to yuicompressor.jar file (req'd for css minification)." + \
		" Assumed 'minify/yuicompressor*.jar'.")
parser.add_argument("-s","--source-dirs", dest="source_dirs",
	action="store", nargs="+",	default=["src/content/*","src/lib/*"],
	help=" a folder or folders to use as source libraries (instead of " + \
	" the defaults of any folder matching ./src/lib/* or" + \
	" ./src/content/*) You can provide as many arguments as you want.")
parser.add_argument("-t ", "--transparent-mode", dest="update_only",
	action="store_true",default=False,
	help=" See file changes instantaneously without re-calling tilth " + \
		" or changing include paths. Creates a production folder " + \
		" skeleton with files that are soft links to their source " + \
		" folder counterpart. Available only on filesystems that " + \
		" (by default) allow users to create symbolic links (OS X and " + \
		" linux filesystems). Overrides Debug Mode if both are used.")
parser.add_argument("path_instr", action="store", 
	default="tilth_instructions")"""
    args = parser.parse_args()
    
    paramdeltas = {
        "verbosity":args.verbosity+1,
        "igblock_open":args.igblock_open,
        "igblock_close":args.igblock_close,
        "extra_tests":args.extra_tests,
        "use_templates":args.use_templates and not args.gen_ec,
        "gen_ec":args.gen_ec}

    #			"debug_mode":args.debug_mode,
    #			"joomla_mode":args.joomla_mode,
    #			"maincache_dir":args.maincache_dir,
    #			"transparent_mode":args.transparent_mode,
    #			"path_closure_compiler":args.path_closure,
    #			"path_yui_compressor":args.path_yui,
    #			"path_instructions":args.path_instr}
    _createTask(args.infile, args.outfile, paramdeltas)
