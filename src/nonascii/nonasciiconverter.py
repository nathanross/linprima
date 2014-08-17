#!/usr/bin/python
import re

def process_file(l_in, l_out, varname):
    f_in = open(l_in, 'r')
    text = f_in.read()
    text = text.replace("\\x", "\u00")
    i = 0;
    range_start = []
    range_end = []
    while i < len(text):
        if text[i] in "[]'\" ":
            continue
        if text[i] not in '0123456789ABCDEFu\\':
            raise Exception("".join(["Encountered unexpected character '",
                                     text[i], 
                                     "' at char num : ", str(i), 
                                     " in file ", l_in, 
                                     ". Exiting."]))
        #print(text[i+2:i+6])
        range_start.append(int(text[i+2:i+6],16))
        if text[i+6] == '-':
            #print("-"+text[i+9:i+13])
            range_end.append(int(text[i+9:i+13],16))
            i += 13            
        else:
            range_end.append(range_start[-1])
            i += 6
    # std::vector
    newtext = "".join(["vector < vector <unsigned int> > ",
                        varname, 
                       " = { {", 
                       ",".join([str(x) for x in range_start]),
                       "}, {",
                       ",".join([str(x) for x in range_end]), 
                       "} };\n"]) 
 
    ## std::array
    #newtext = "".join(["std::array < std::array <unsigned int, ", 
    #                   str(len(range_start)),
    #                   " > 2 > ",varname, 
    #                   " = { { { {", 
    #                   ",".join([str(x) for x in range_start]),
    #                   "} }, { {",
    #                   ",".join([str(x) for x in range_end]), 
    #                   "} } } };\n"])
    f_out = open(l_out, 'w')
    f_out.write(newtext)
    f_out.flush()
    f_out.close()
    

def main():
    for i in ['start', 'part']:
        process_file(i, i+'_out', 'nonasciiIdentifier' + i)

if __name__ == '__main__':
    main()
