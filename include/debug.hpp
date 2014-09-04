#ifndef DEBUG_HPP
#define DEBUG_HPP

#line 4 "debug.hpp"
#include <string>
#include <rapidjson/document.h>

#ifdef DO_DEBUG

int *debugidx;

void DEBUGIN(std::string in, bool lowprio);

void DEBUGOUT(std::string in, bool lowprio);

template<typename T> T DBGRET(std::string a, T b) { 
    DEBUGOUT(a, false); 
    return b; 
}

//shows you which entry is unitialized when one of the 
//items in the json is uninitialized (leading to a segfault)
//walks json, printing each path before trying to access/print
//value at that path.
void walkJson(std::string path, const rapidjson::Value& a);

#endif
#ifndef DO_DEBUG

#define DEBUGIN(A,B) 
#define DEBUGOUT(A,B)
#define DBGRET(A,B) B 
#define walkJson(A, B) 

#endif


#endif
