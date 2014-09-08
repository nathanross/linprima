#ifndef FIXED_STRING_HPP
#define FIXED_STRING_HPP
#include <string>
#include <string.h>
#include <stdlib.h>

namespace fixedstring {
    typedef void* FixedString;
    inline void* getFixedStr(std::string in) {        
        char * out = (char *) malloc(in.length() + 5);
        *((int *) out) = (int) in.length();
        strcpy(out+4,in.data());
        return out;
    }
    //len = exclusive of last null terminator. 
    inline void* getFixedStr(const char *in, int len) {
        char * out = (char *) malloc(len + 5);
        *((int *) out) = (int) len;
        strcpy(out+4,in);
        return out;
    }
    inline int length(FixedString in) {
        return *((int *) in);
    }
    inline char * data(FixedString in) {
        return ((char *) in) + 4;
    }
}

#endif
