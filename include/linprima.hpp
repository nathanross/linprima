#ifndef LINPRIMA_HPP
#define LINPRIMA_HPP


#line 2 "linprima.hpp"

#include "Tokenizer.hpp"
#include "ParseFuncs.hpp"
#include "stringutils.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include <rapidjson/document.h>
#include <string>

#ifndef LIMITJSON
inline
void tokenizeImpl(rapidjson::Document &outJson,
                  const std::u16string code, 
                  OptionsStruct options,
                  const bool retErrorsAsJson) { 
    options.tokens = true;
    options.tokenize = true;
    Tokenizer tknr(code, options);
    tknr.tokenize(outJson, retErrorsAsJson);
}
inline
void tokenizeImpl(rapidjson::Document& d, 
                      const std::u16string code,
                      const OptionsStruct options) { 
    tokenizeImpl(d, code, options, false);
}
inline
void tokenizeImpl(rapidjson::Document &d,
                      const std::string code,
                      const OptionsStruct options) { 
    tokenizeImpl(d, toU16string(code), options, false);
}
inline
void tokenizeImpl(rapidjson::Document &d, 
                      const std::string code) { 
    OptionsStruct o;
    tokenizeImpl(d, toU16string(code), o, false);
}
inline
void tokenizeImpl(rapidjson::Document &d, 
                      const std::u16string code) { 
    OptionsStruct o;
    tokenizeImpl(d, code, o, false);
}
#endif

std::string tokenizeRetString(const std::u16string code,
                             OptionsStruct options);

inline
std::string tokenizeRetString(const std::string code, 
                                  const OptionsStruct options) {
    return tokenizeRetString(toU16string(code), options);
}

#ifndef LIMITJSON

inline
void parseImpl(rapidjson::Document &outJson,
                   const std::u16string code, 
                   OptionsStruct options, //# nonconst 1:1
                   const bool retErrorsAsJson) { 
    ParseTools pt(code, options);
    pt.parse(outJson, retErrorsAsJson);
};

inline
void parseImpl(rapidjson::Document& d, 
                   const std::u16string code,
                   OptionsStruct options) {    
    parseImpl(d, code, options, false);
}

inline
void parseImpl(rapidjson::Document& d, 
                   const std::string code, 
                   OptionsStruct options) {    
    parseImpl(d, toU16string(code), options, false);
}

inline
void parseImpl(rapidjson::Document& d, 
                   const std::string code) { 
    OptionsStruct o;
    parseImpl(d, toU16string(code), o, false);
}

inline
void parseImpl(rapidjson::Document& d, 
                   const std::u16string code) { 
    OptionsStruct o;
    parseImpl(d, code, o, false);
}
#endif

//# return json as string.
std::string parseRetString(const std::u16string code, OptionsStruct options);
std::string parseRetString(const std::string code,
                      OptionsStruct options) { 
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
