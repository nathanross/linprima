#ifndef FIXED_STR_HPP
#define FIXED_STR_HPP
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace fixedstr {
    //immutable minimal-footprint strings.
    //include both chars and lens, 
    //do include the standard null terminator at the end of
    //each arr so can be read by strlen as is expected user
    //behavior on return of char array. 

    //but like string, strlen, etc., do not include this nullterm
    //in length.
    
    /*class  FixedStringClass {
    public:


    private:
    }; */

    typedef void* FixedStr;

    inline long length(const void * in) {
        return *((long *) in);
    }
    inline char * data(const void * in) {
        return ((char *) in) + 8;
    }
    inline std::string toString(const void * in) {
        return std::string(data(in), length(in));
    }

    inline void* getFixedStr(FixedStr in) {
        void * out = (char*) malloc(length(in)+9);
        memcpy(out, data(in), length(in)+9);
        return out;
    }
    inline void* getFixedStr(std::string in) {        
        char * out = (char *) malloc(in.length() + 9);
        *((long *) out) = (long) in.length();
        memcpy(out+8, in.data(), in.length());
        *(out+8+in.length()) = '\0';
        return out;
    }
    //len = exclusive of last null terminator. 
    inline void* getFixedStr(const char *in, long len) {
        char * out = (char *) malloc(len + 9);
        *((long *) out) = len;
        memcpy(out+8, in, len);
        *(out+8+len) = '\0';
        return out;
    }

    //len = exclusive of last null terminator. 
    /* inline void* getFixedStr(const char *in, long len) {
        char * out = (char *) malloc(len + 9);
        *((long *) out) = len;
        memcpy(out+8, in, len);
        *(out+8+len) = '\0';
        return out;
    } */

    //ScopedFixedString.
    //you could also consider it "stack fixed string"
    //not as memory efficient or performant. 
    //Really only necessary for
    //constants and globals
    //always constructs by copy.
    class SFixedStr {
    public:
        void * f;
        SFixedStr() 
        { f = nullptr; }



        SFixedStr& operator=(SFixedStr&& other) {
            f = other.f;
            other.f = nullptr;            
            return *this;
        }

        SFixedStr(const char* in,
                     const long len) 
        { f = getFixedStr(in, len); }
        explicit SFixedStr(const char* in) 
        { f = getFixedStr(in, strlen(in)); }
        SFixedStr(const std::string &in) 
        { f = getFixedStr(in); }

        SFixedStr(SFixedStr&& other):
            f(other.f) {
            other.f = nullptr;
        }

        SFixedStr(void *&& other):
            f(other) {
            other = nullptr;
        }

        void move(FixedStr && in) {
            f = in;
        }
        void copy(const FixedStr in) {
            f = getFixedStr(in);
        }
        void copy(const SFixedStr in) {
            f = getFixedStr(in.f);
        }

        ~SFixedStr() {
            if (f != nullptr) {
                free(f);
            }
        }
        void clear() {
            if (f != nullptr) {
                free(f);
                f = nullptr;
            }
        }
    private:
        //explicit copy only. As this is intended for use
        //in memory-sensitive applications, we want to essentially
        //enforce ::move whenever possible.
        SFixedStr(const SFixedStr &in); 
        SFixedStr& operator=(const SFixedStr &in); 
    };

}

#endif
