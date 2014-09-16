#ifndef LINPRIMA_HPP
#define LINPRIMA_HPP


#line 2 "linprima.hpp"

#include "Tokenizer.hpp"
#include "ParseFuncs.hpp"
#include "stringutils.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include "fixedstr.hpp"
#include <rapidjson/document.h>
#include <string>

fixedstr::SFixedStr&& tokenizeRetString(const std::u16string code,
                                        OptionsStruct options);

inline
fixedstr::SFixedStr&& tokenizeRetString(const std::string code, 
                                        const OptionsStruct options) {
    return tokenizeRetString(toU16string(code), options);
}

//# return json as string.
fixedstr::SFixedStr&& parseRetString(const std::u16string code, 
                                     OptionsStruct options);

inline
fixedstr::SFixedStr&& parseRetString(const std::string code,
                                     const OptionsStruct options) { 
    return parseRetString(toU16string(code), options);
}

extern "C" {

    char* tokenizeExtern(const char *code, 
                         const char* options);
    char* parseExtern(const char *code,
                      const char* options);

    // #emscripten will sometimes, on receipt of certain unicode chars,
    // append options any second char arg array to code. 
    // because this only happens sometimes, you can't 
    // just subtract length of second arg
    // available workarounds pending any emcc fixes
    // are compare ending text of code and assume
    // no one would deliberately pass code which ended with
    // the value of the options array
    // or simply pass a length arg in limprima-wrap in ASM

    // node that because js stores strings in UCS-2 or UTF-16
    // with some complications, we have to convert to UTF-16
    // before using substring, because the length of the char*
    // argument here is going to be byte length, not number of
    // characters as represented in javascript strings (ucs)
    char* tokenizeASMJS(const char *code, int codelen,
                        const char* options);
    char* parseASMJS(const char *code, int codelen, 
                     const char* options);

}

    //#include "profiler.h"

//#include <chrono>
//using std::chrono::system_clock;

#ifdef HASMAIN
int main();
#endif


#endif
