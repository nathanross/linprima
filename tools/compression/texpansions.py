#!/usr/bin/python

import sys

def make_second_line(parampos, name):
    second_line = ""
    for i in range(0, len(name)+parampos):
        second_line += " "
    second_line += "{}_.length());"
    return second_line.format(name.upper())

def add_ref_lines(cpp, names, firstline):
    #explanation of -1 "magic number"
    #+1 because we want the location right after the paren
    #-2 because we're subtracting for the initial variable standin brackets {}
    # (we add the actual var name's length during the loop)
    parampos = firstline.find("(") -1     
    for name in names:
        cpp.append(firstline.format(name.upper(),
                                        name.upper()))
        cpp.append(make_second_line(parampos, name))

def add_assign(cpp, allterms, keyterms, valterms, useNum, indt):

    i=0
    for name in allterms:
        cpp.append(assign.format(name.upper(), 
                                        str(i) if useNum else name))
        i+=1

def getTerms(l_terms):
    f_terms = open(l_terms, "r")
    terms = f_terms.read().split("\n")
    f_terms.close()
    return terms

def writeout(l_out, content):
    f_out = open(l_out, "w")
    f_out.write(content)
    f_out.flush()
    f_out.close()

encoder = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'

def compressed(decoder_arr, termctr, original):
    pos = termctr[0]
    subst = []
    nextnum = 0
    while (pos > 0 or len(subst) == 0):
        subst.insert(0, encoder[pos % 62])
        pos = pos / 62; 
    decoder_arr.append(original)
    termctr[0] += 1
    return "".join(subst)

def main():
    CPP_DECODE = 1
    outputdir = "cmpR_out"
    MODULE_NAME = "texpconsts"
    allterms = getTerms("cmpR_tmp/ordered.txt")
    allterms.remove('')
    suffixString = {'compressed':'_', 'full':'_FULL'}
    hpp = []
    cpp = []
    module_header_id = MODULE_NAME.upper().replace(".", "_")
    hpp.append("#ifndef {0}\n#define {0}".format(module_header_id))
    hpp.append('\n#line 4 "{}.hpp" '.format(MODULE_NAME))
    hpp.append('#include "fixedstr.hpp"');
    hpp.append("namespace text {")

    cpp.append('#line 1 "{}.cpp" '.format(MODULE_NAME))
    cpp.append('#include "{}"'.format(MODULE_NAME + ".hpp"));
    cpp.append("namespace text {")

    assign_h = 'extern const char * {0};'
    assign = 'const char * {0} = "{1}";'
    strRef_h = "extern const fixedstr::SFixedStr {0};"
    strRef = "const fixedstr::SFixedStr {0}({1},{2});"

    decoder_arr = []
    termctr = [0];

    if (CPP_DECODE):
        for term in allterms:
            capterm = "_" + term.replace("<","Z").replace(">","Z")
            hpp.append(assign_h.format(capterm+suffixString['full']))
            cpp.append(assign.format(capterm+suffixString['full'], term))
    for lowmem in [True, False]:
        hpp.append(("#ifdef" if lowmem else "#ifndef") + " LOWMEM")
        cpp.append(hpp[-1])
        for term in allterms:
            capterm = "_" + term.replace("<","Z").replace(">","Z")
            if (not CPP_DECODE) and (not lowmem):
                cpp.append(assign.format(capterm+suffixString['full'], term))
            if lowmem:
                #compressed returns an encoded representation,
                #and stores it with a reference to the fullname.
                findoriginal = capterm+suffixString['full'] if CPP_DECODE else term
                stringRep = compressed(decoder_arr, termctr, 
                                       findoriginal)
                hpp.append(assign_h.format(capterm+suffixString['compressed']));
                cpp.append(assign.format(capterm+suffixString['compressed'], #lvalue
                                         stringRep)) #rvalue
                hpp.append(strRef_h.format(capterm));
                cpp.append(strRef.format(capterm,  #lvalue
                                         capterm+suffixString['compressed'],#rvalue
                                         len(stringRep))) 
            else:
                hpp.append(strRef_h.format(capterm)); 
                cpp.append(strRef.format(capterm, 
                                         capterm+suffixString['full'],
                                         len(term)))

        if lowmem and CPP_DECODE:

            rows = len(decoder_arr)
            hpp.append("\n extern const char * decoder[{}];".format(rows))
            cpp.append("\n const char * decoder[{}] = ".format(rows)+"{")
            for row in range(0,rows-1):
                cpp.append("   " + decoder_arr[row] + ",")
            cpp.append(decoder_arr[rows-1])
            cpp.append("};\n");

        hpp.append("#endif")
        cpp.append("#endif")

    cpp.append("}\n")

    hpp.append("}\n")
    hpp.append("#endif\n")

    writeout(outputdir + "/" + MODULE_NAME + ".hpp", "\n".join(hpp))
    writeout(outputdir + "/" + MODULE_NAME + ".cpp", "\n".join(cpp))

    if CPP_DECODE:
        return

    #indt_js = "        "
    #entry_js = indt_js + "    \"{}\","
    #for context in CONTEXTS:
    #    js.append(indt_js + "var " + context + "terms = [")
    #    for term in terms[context]:
    #        js.append(entry_js.format(term))        
    #    js.append(indt_js + "];")
    #
    #writeout(outputdir + "/jsondecompress.js", "\n".join(js))


if __name__ == '__main__':
    main()
