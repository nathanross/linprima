#line 1 "LinprimaTask.h"


  // Ensure the condition is true, otherwise throw an error.
  // This is only to have a better contract semantic, i.e. another safety net
  // to catch a logic error. The condition shall be fulfilled in normal case.
  // Do NOT use this to enforce a certain condition on any user input.


//information that both Tokenizer and ParseTools use,
//and is shared between them.

struct LinprimaTask {
    u16string sourceStr;
    const char16_t *sourceRaw;
    const int length;

#ifdef LIMITJSON
    StringBuffer buffer;
    Writer<StringBuffer> writer;
    vector<string> * completeObjects;
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
    LinprimaTask(const u16string sourceStr,
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
                       const string messageFormat, 
                       vector< string > args);
    //#throw_begin
    void throwError(ptrTkn token, 
                    const string messageFormat, 
                    vector< string > args);
    void throwErrorTolerant(ptrTkn token, 
                            const string messageFormat, 
                            vector<string> args);
    void throwUnexpected(ptrTkn token);


#ifndef THROWABLE
    int softAssert(const bool condition, const string message);
#endif
#ifdef THROWABLE
    void softAssert(const bool condition, const string message);
#endif
    //#throw_end
};


