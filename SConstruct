#!/usr/bin/python
import glob, os, sys, inspect, shutil
from os import path
#cmd_subfolder = path.realpath(path.abspath(path.join(path.split(inspect.getfile( inspect.currentframe() ))[0],"tools")))
#if cmd_subfolder not in sys.path:
#     sys.path.insert(0, cmd_subfolder)

import throw52

#general options
log = int(ARGUMENTS.get('log', 0))

C_ASM = 1;
C_CLANG = 2;

#-----------------------
#detect compiler used
c_asmjs = int(ARGUMENTS.get('asmjs', 0))
c_clang = int(ARGUMENTS.get('clang', 0))
c_gcc = int(ARGUMENTS.get('gcc', 0))

total = c_asmjs + c_clang + c_gcc
if total > 1:
    print("too many compilers selected")
    sys.exit()
elif total == 0:
    c_clang = 1


#----------------------
#asmjs compilation.
if c_asmjs:
   optim = int(ARGUMENTS.get('optim', 5))
   gdb = 0
   t52 = 1
   profiny = 0
   lowmem = int(ARGUMENTS.get('lowmem', 1))
   limitjson = int(ARGUMENTS.get('limj', 1))
#-----------------------
#c++ binary compilation


if c_clang or c_gcc:
    optim = int(ARGUMENTS.get('optim', 3))

    #debug with valgrind     
    gdb = int(ARGUMENTS.get('gdb', 0))

    log = int(ARGUMENTS.get('log', 0))

    #use asmjs stack-based exceptions instead of throws.    
    t52 = int(ARGUMENTS.get('t52', 0))

    profiny = int(ARGUMENTS.get('profiny', 0))
    if profiny:
        graph = int(ARGUMENTS.get('graph', 0)) 

    #minimize memory usage at some performance
    #cost (usually only useful for asmjs debug)
    lowmem = int(ARGUMENTS.get('lowmem', 1))
    limitjson = int(ARGUMENTS.get('limj', 1))

#-----------------------------

def clean(loc, preserveRoot=True):
    if path.exists(loc):
        shutil.rmtree(loc)
    if preserveRoot:
        os.mkdir(loc)

#-----------------------------
#compiler exec
comp = ""
if c_clang:
    comp = "clang++"
elif c_gcc:
    comp = "g++"
elif c_asmjs:
    comp = "/usr/bin/emcc"

#source preperation.
root = "."
srcroot = "."
tmp_path = root + '/tmp'

if t52 or profiny:
    #do this first because could cause loss
    #of work to call clean on a variable that
    #may be, with only an accidental unindent,
    #the repo root dir
    for ldir in ['/src', '/include']:
        l_destdir = tmp_path+ldir
        clean(l_destdir, True)
        l_srcdir = root+ldir
        l_srcfiles = glob.glob(l_srcdir + '/*.cpp')
        l_srcfiles.extend(glob.glob(l_srcdir + '/*.h'))
        l_srcfiles.extend(glob.glob(l_srcdir + '/*.hpp'))
        srcdestpairs = []
        for l_srcfname in l_srcfiles:
            l_destfname = l_destdir+'/'+path.basename(l_srcfname)
            srcdestpairs.append({'in':l_srcfname, 'out':l_destfname})
        if t52:       
            params = throw52.getDefaultParams() 
            throw52.Throw52Task(srcdestpairs, params)
        elif profiny:
            for srcdest in srcdestpairs:
                if t52:
                    srcdest['in'] = srcdest['out']
                f_src = open(srcdest['in'], 'r')
                data = f_src.read()
                data = data.replace('DEBUGIN("', '\n PROFINY_SCOPE \n DEBUGIN("')
                f_src.close()
                f_out = open(srcdest['out'], 'w')
                f_out.write(data)
                f_out.flush()
                f_out.close()
    srcroot = tmp_path


if c_clang or c_gcc:
    if gdb:
        l_out = tmp_path +'/test_gdb'  
    elif profiny:
        l_out = tmp_path +'/test_profiny'
    else:
        l_out = root+'/build/linprima.so'
else:
   l_out = tmp_path+'/linprim.asm.raw.js'

linkflagstr = ' -Wall -std=c++11 -O'
flagstr =' -Wall -std=c++11 -O'

if optim <= 4:
    flagstr += str(optim)
    linkflagstr += str(optim)
else:
    flagstr += 'z '
    linkflagstr += 'z '

if profiny:
   flagstr += ' -D PROFINY '
   linkflagstr += ' -lboost_timer -lboost_chrono -lboost_system '
   if graph:
      flagstr += ' -D PROFINY_CALL_GRAPH_PROFILER '
   else:
      flagstr += ' -D PROFINY_FLAT_PROFILER '

if c_gcc:
    flagstr += ' -D LIBSTDC '
else:
    flagstr += ' -stdlib=libc++ '
    linkflagstr += ' -stdlib=libc++ '

if c_clang or c_gcc:
    if profiny:
        flagstr += ' -D HASMAIN '
    elif gdb:
        flagstr += ' -g -D HASMAIN '
    else:
        flagstr += ' -fPIC '
        linkflagstr += ' -shared -fPIC '
elif c_asmjs:
    flagstr += " -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" "
    linkflagstr += " -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" "

if not t52:
    flagstr += " -D THROWABLE "
    
if log:
    flagstr += " -D DO_DEBUG "

if limitjson:
    flagstr += " -D LIMITJSON -D LOWMEM "
elif lowmem:
    flagstr += " -D LOWMEM "


pathinfo = {'PATH' : os.environ['PATH']}
#SConscript('SConscript', exports=['comp', 'root', 'srcroot', 'l_out', 'flagstr', 'linkflagstr', 'pathinfo'])

env = Environment(
    ENV = pathinfo,
    CXX=comp,
    CPPPATH=[root+'/lib', srcroot+'/include'],
    CXXFLAGS=flagstr,
    LINKFLAGS=linkflagstr)
env.Program(l_out, Glob(srcroot+'/src/*.cpp'))
