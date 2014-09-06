#ifndef LINPRIMA_TASK_HPP
#define LINPRIMA_TASK_HPP

#line 4 "LinprimaTask.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

  // Ensure the condition is true, otherwise throw an error.
  // This is only to have a better contract semantic, i.e. another safety net
  // to catch a logic error. The condition shall be fulfilled in normal case.
  // Do NOT use this to enforce a certain condition on any user input.


//information that both Tokenizer and ParseTools use,
//and is shared between them.

struct LinprimaTask {
    std::u16string sourceStr;
    const char16_t *sourceRaw;
    const int length;

#ifdef LIMITJSON
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer;
    std::vector<std::string> * completeObjects;
#endif

    int errorType;
    ExError retError;
#ifndef THROWABLE
    AssertError retAssertError;
#endif

    bool strict;
    ExtraStruct extra;
    StateStruct state;
    int idx;
    int lineNumber;
    int lineStart;
    ptrTkn lookahead;
    LinprimaTask(const std::u16string sourceStr,
                 //const char16_t *sourceRaw,
                 //const long length,
                 const OptionsStruct opt);

    char16_t source(const int pos) {
        return *(sourceRaw + pos);
    }
    
#ifndef THROWABLE
    //#throw_begin
    int throwToJS(ExError err);
    //#throw_end
#endif
#ifdef THROWABLE
    void throwToJS(const ExError err);
#endif

    ExError genExError(ptrTkn token, 
                       const std::string messageFormat, 
                       std::vector< std::string > args);
    //#throw_begin
    void throwError(ptrTkn token, 
                    const std::string messageFormat, 
                    std::vector< std::string > args);
    void throwErrorTolerant(ptrTkn token, 
                            const std::string messageFormat, 
                            std::vector<std::string> args);
    void throwUnexpected(ptrTkn token);


#ifndef THROWABLE
    int softAssert(const bool condition, const std::string message);
#endif
#ifdef THROWABLE
    void softAssert(const bool condition, const std::string message);
#endif
    //#throw_end
};


#endif
