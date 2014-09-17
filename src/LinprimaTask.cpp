#line 1 "LinprimaTask.cpp"
#include "LinprimaTask.hpp"
#include "debug.hpp"
#include "Tokenizer.hpp"

using namespace std;
using namespace rapidjson;
LinprimaTask::LinprimaTask(const u16string sourceStrArg,
                           //const char16_t *sourceRawArg,
                           //const long lengthArg,
                           const OptionsStruct optArg):
    sourceStr(sourceStrArg),
    sourceRaw(sourceStr.data()), length(sourceStr.length()),
    strict(false),
    extra(optArg), state(),
    idx(0), lineNumber(0), lineStart(0),
    lookahead(make_shared<TokenStruct>(0,0,0)) {

    if (length > 0) { lineNumber = 1; }
    lookahead->isNull = true;

#ifdef DO_DEBUG
    debugidx = &(idx);
#endif
}


#ifndef THROWABLE
 //throw_
int LinprimaTask::throwToJS(ExError err) {
    DEBUGIN(" throwToJS(ExError err)", false);
    retError = err;
    errorType = 0;
    ErrWrapint evoid;
    evoid.err = true;
    DEBUGOUT("throwToJs", false);
    return evoid;
}
#endif
#ifdef THROWABLE
void LinprimaTask::throwToJS(const ExError err) {
    DEBUGIN(" throwToJS(ExError err)", false);
    DEBUGOUT("throwToJs", false);    
    throw err;
}
#endif


ExError LinprimaTask::genExError(ptrTkn token, 
        const string messageFormat, 
        vector< string > args) {
    DEBUGIN(" genExError", false);
    ExError error;
    int searchresult;
    string searchkey, msg = messageFormat;
    for (unsigned int i=0; i<args.size(); i++) {
        searchkey="%";
        //#all this conversion is a bit ridiculous. it may
        //#be simpler here to work with just strings.
        searchkey.append(to_string(i));
        searchresult = msg.find(searchkey);
        softAssert(searchresult != string::npos, 
                   "args to genExError exceeded substitutable values in message format");
        msg.erase(searchresult, 2);
        msg.insert(searchresult, args[i]);
    }

    if (token->lineNumber != -1) {
        error.index = token->start;
        error.lineNumber = token->lineNumber;
        error.column = token->start - lineStart + 1;
    } else {
        error.index = idx;
        error.lineNumber = lineNumber;
        error.column = idx - lineStart + 1;
    }

    error.description = msg;
    DEBUGOUT("genExErr", false);
    return error;
}

//throw_
void LinprimaTask::throwError(ptrTkn token, 
                const string messageFormat, 
                vector< string > args) {
    DEBUGIN(" throwError(ptrTkn token, u16string messageFormat, vector<u16string> args)", false);
    throwToJS(genExError(token, messageFormat, args));
    DEBUGOUT(" throwError()", false);
    return;
}

//throw_
void LinprimaTask::throwErrorTolerant(ptrTkn token, 
                        const string messageFormat, 
                        vector<string> args) {
    DEBUGIN(" throwErrorTolerant(ptrTkn token, u16string messageFormat, vector<u16string> args)", false);
    ExError result = genExError(token, messageFormat, args);
    if (extra.errorTolerant) {
        extra.errors.push_back(result);
    } else {
        throwToJS(result);
    }
    DEBUGOUT("throwErrTol", false);
    return;
}

// Throw an exception because of the token.
//throw_
void LinprimaTask::throwUnexpected(ptrTkn token) {
    DEBUGIN(" throwUnexpected(ptrTkn token)", false);
    Mssg errmsg = 
        (token->type == TknType::EOFF)? Mssg::UnexpectedEOS :
        (token->type == TknType::NumericLiteral)? Mssg::UnexpectedNumber:
        (token->type == TknType::StringLiteral)? Mssg::UnexpectedString :
        (token->type == TknType::Identifier)? Mssg::UnexpectedIdentifier :
        (token->type == TknType::Keyword &&
         Tokenizer::isFutureReservedWord(token->strvalue))? Mssg::UnexpectedReserved :
        Mssg::None;
    if (errmsg != Mssg::None) {
        throwError(token, Messages[errmsg], {});
    }
    if (token->type == TknType::Keyword && strict &&
        Tokenizer::isStrictModeReservedWord(token->strvalue)) {
        throwErrorTolerant(token, Messages[Mssg::StrictReservedWord], {});
        return;
    }    
    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages[Mssg::UnexpectedToken], {token->strvalue});
    return; //#throw52

}


#ifndef THROWABLE
//throw_
int LinprimaTask::softAssert(const bool condition, 
        const string message) {
    DEBUGIN(" assert(bool condition, string message)", false);

    string providedMessage = "ASSERT: ";
    providedMessage.append(message);

    errorType = 2;
    retAssertError.description = providedMessage;
    ErrWrapint tmp;
    tmp.err = (! condition);
    DEBUGOUT("assert", false);
    return tmp;
}

#endif
#ifdef THROWABLE
void LinprimaTask::softAssert(const bool condition, 
        const string message) {
    DEBUGIN(" assert(bool condition, string message)", false);

    string providedMessage = "ASSERT: ";
    providedMessage.append(message);
    if (! condition)
        { throw runtime_error(providedMessage); }
    DEBUGOUT("", false);
 }

#endif
