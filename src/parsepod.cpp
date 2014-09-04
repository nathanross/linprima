#line 1 "parsepod.cpp"
#include "parsepod.hpp"

ParseParamsOptions::ParseParamsOptions() {
    firstRestricted = nullptr; 
    stricted = nullptr; 
}

ParseParamsOut::ParseParamsOut() {
#ifndef THROWABLE
    err = false;
#endif
    message="";
    firstRestricted = nullptr; 
    stricted = nullptr; 
}

ReinterpretOptions::ReinterpretOptions() {
    firstRestricted = nullptr; //?
    stricted = nullptr; //? nec.? had it only in reinOut before.
    //? not sure if context will make it different.
}

ReinterpretOut::ReinterpretOut() {
#ifndef THROWABLE
    err = false;
#endif
    isNull=false;
    firstRestricted = nullptr;
    stricted = nullptr;
}
