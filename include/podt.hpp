#ifndef PODT_HPP
#define PODT_HPP

#line 4 "podt.hpp"
#include "strref.hpp"
#include "enums.hpp"
#include "t52types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <rapidjson/document.h>

#ifdef THROWABLE
#include <exception>
#endif
//using namespace rapidjson
//using namespace std;

class Node;
class NodesComments;
struct ExtraStruct; 


struct RegexHalf {
    std::string value;
    std::string literal;
#ifndef THROWABLE
    bool err;
#endif
    int start;
    int end;
    RegexHalf();
};

struct Loc { 
    //aka SourceLocation
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    bool hasSource;
    std::string source;

    Loc(int lineNumber, int idx, int lineStart);
    void toJson(rapidjson::Value& out, 
                rapidjson::Document::AllocatorType* alloc) const;
};

struct Comment {
    const StrRef * type;
    std::string value;
    int range[2];
    Loc loc;
    Comment(int lineNumber, int idx, int lineStart);
    void toJson(const ExtraStruct *extra,
                rapidjson::Value& out, 
                rapidjson::Document::AllocatorType* alloc);
};

//# called ExError to prevent forseeable 
//# exception-handling namespace conflict.

class ExError 
#ifdef THROWABLE
    : public std::exception 
#endif
{
public:
    std::string description;
    int index;
    int lineNumber;
    int column;
    ExError();
    void toJson(const ExtraStruct *extra,
                rapidjson::Value& out, 
                rapidjson::Document::AllocatorType* alloc);
    void toJsonTolerant(const ExtraStruct *extra,
                        rapidjson::Value& out, 
                        rapidjson::Document::AllocatorType* alloc);
};

#ifndef THROWABLE

class AssertError {
public:
    std::string description;
    AssertError();
    void toJson(const ExtraStruct *extra,
                rapidjson::Value& out, 
                rapidjson::Document::AllocatorType* alloc);
};
#endif



//used in initial scannig
struct TokenStruct {
    bool isNull; 
    TknType type;
    std::string strvalue;
    double dblvalue;
    int intvalue;
    bool bvalue;
#ifndef THROWABLE
    bool err;
#endif

    int literaltype; //lin only.

    std::string literal; //regex literal only
    std::string flags; //regex literal only

    int prec; 
    //# for staying as close to orig. as possible in parseBinaryExpression


    int lineNumber;
    int lineStart;
    int startLineNumber;
    int startLineStart;
    int start;

    int range[2];
    int end;
    bool octal;
    Loc loc;
    TokenStruct(); //null token
    TokenStruct(int lineNumber, int idx, int lineStart);
};

typedef std::shared_ptr<TokenStruct> ptrTkn;

#ifndef THROWABLE

class ErrWrapptrTkn {
public:
    bool err;
    ptrTkn val;
    ErrWrapptrTkn() {
        err = false;
    }
    ErrWrapptrTkn(ptrTkn in) {
        val = in;
        err = false;
    }
};

#endif

//stored in extra.tokens
struct TokenRecord {
    Loc loc;
    int range[2];
    std::string valuestring;
    TknType type;
    TokenRecord(Loc locArg);
    TokenRecord(int lineNumber, int idx, int lineStart);
    void toJson(const ExtraStruct *extra,
                rapidjson::Value& out, 
                rapidjson::Document::AllocatorType* alloc);
};

struct RegexLeg {
    bool isStart;
    bool isNum;
    int num;
    const StrRef * path;
    RegexLeg(const int num) {
        isStart = false;
        isNum = true;
        this->num = num;
    }
    RegexLeg(const StrRef * path) {
        isStart = false;
        isNum = false;
        this->path = path;
    }
    RegexLeg & operator= (const RegexLeg & other) {
        isStart = other.isStart;
        isNum = other.isNum;
        num = other.num;
        path = other.path;
        return *this;
    }
};

struct StateStruct {
    bool allowIn;
    int parenthesisCount;
    std::unordered_set<std::string> labelSet;
    bool inFunctionBody;
    bool inIteration;
    bool inSwitch;
    int lastCommentStart;
    StateStruct();
};

struct OptionsStruct {
    bool range;
    bool loc;
    bool comment;
    bool tolerant;
    bool attachComment;
    bool tokens;
    bool tokenize;
    bool hasSource;
    std::string source;
    OptionsStruct();
    bool json_getbool(rapidjson::Value& in, 
                      const std::string key, 
                      const bool defaultVal);
    OptionsStruct(const char *in_o);
};

struct ExtraStruct {
    //# port-specific member to replace "if (extra.tokens)"
    bool tokenTracking;
    std::vector<TokenRecord> tokenRecords; //called extra.tokens in esprima
    // name changed here to distinguish specific type and different domains
    // of these types.

    bool hasSource;
    std::string source; 

    bool tokenize;
    unsigned int openParenToken;
    unsigned int openCurlyToken;

    bool range;
    bool loc;

    //# port-specific member to replace "if (extra.comments)"
    bool commentTracking; 
    
    std::vector<Comment> comments;

    //# port specific member to replace "if (extra.errors)"
    bool errorTolerant;  

    std::vector<ExError> errors;

    bool attachComment;
    std::vector<Comment> leadingComments;
    std::vector<Comment> trailingComments;
    std::vector< std::shared_ptr<NodesComments> > bottomRightStack; //! todo Node header text.

    ExtraStruct(OptionsStruct opt);
    void clear();
};
#endif
