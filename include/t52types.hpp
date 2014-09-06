#ifndef T52TYPES
#define T52TYPES

#line 4 "t52types.hpp"
#include <string>
#ifndef THROWABLE

//todo use templated class and typedefs.
/*
template <class T> class ErrWrap {
public:
    bool err; T val;
    ErrWrap<T>() {
        err = false;
    }
    ErrWrap<T>(T val) {
        this->val = val;
        err = false;
    }
};
ErrWrap<int> noErr; */


class ErrWrapint {
public:
    bool err;
    int val;
    ErrWrapint() {
        err = false;
    }
    ErrWrapint(int in) {
        val = in;
        err = false;
    }
};
extern ErrWrapint noErr;

class ErrWrapbool {
public:
    bool err;
    bool val;
    ErrWrapbool() {
        err = false;
    }
    ErrWrapbool(bool in) {
        val = in;
        err = false;
    }
};

class ErrWrapstring {
public:
    bool err;
    std::string val;
    ErrWrapstring() {
        err = false;
    }
    ErrWrapstring(std::string in) {
        val = in;
        err = false;
    }
};

class ErrWrapu16string {
public:
    bool err;
    std::u16string val;
    ErrWrapu16string() {
        err = false;
    }
    ErrWrapu16string(std::u16string in) {
        val = in;
        err = false;
    }
};



#endif

#endif
