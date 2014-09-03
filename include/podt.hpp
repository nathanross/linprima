#ifndef PODT_HPP
#define PODT_HPP
#line 2 "podt.h"
//#include <rapidjson/etc...

//using namespace rapidjson
//using namespace std;
typedef Document::AllocatorType AllocatorType;

class Node;
class NodesComments;
struct ExtraStruct; 


struct RegexHalf {
    string value;
    string literal;
#ifndef THROWABLE
    bool err;
#endif
    int start;
    int end;
    RegexHalf();
};

RegexHalf::RegexHalf() { 
#ifndef THROWABLE
        err = false;
#endif
        start = -1; 
        end = -1; 
}

struct Loc { 
    //aka SourceLocation
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    bool hasSource;
    string source;

    Loc(int lineNumber, int idx, int lineStart);
    void toJson(Value& out, AllocatorType* alloc) const;
};

struct Comment {
    const StrRef * type;
    string value;
    int range[2];
    Loc loc;
    Comment(int lineNumber, int idx, int lineStart);
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
};

//# called ExError to prevent forseeable 
//# exception-handling namespace conflict.

class ExError 
#ifdef THROWABLE
 : public exception 
#endif
{
public:
    string description;
    int index;
    int lineNumber;
    int column;
    ExError();
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
    void toJsonTolerant(const ExtraStruct *extra,
                        Value& out, AllocatorType* alloc);
};

#ifndef THROWABLE

class AssertError {
public:
    string description;
    AssertError();
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
};
#endif



//used in initial scannig
struct TokenStruct {
    bool isNull; 
    TknType type;
    string strvalue;
    double dblvalue;
    int intvalue;
    bool bvalue;
#ifndef THROWABLE
    bool err;
#endif

    int literaltype; //lin only.

    string literal; //regex literal only
    string flags; //regex literal only

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
    TokenStruct(int lineNumber, int idx, int lineStart);
};

typedef shared_ptr<TokenStruct> ptrTkn;

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
    string valuestring;
    TknType type;
    TokenRecord(Loc locArg);
    TokenRecord(int lineNumber, int idx, int lineStart);
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
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
    unordered_set<string> labelSet;
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
    string source;
    OptionsStruct();
    bool json_getbool(Value& in, 
                      const string key, 
                      const bool defaultVal);
    OptionsStruct(const char *in_o);
};

struct ExtraStruct {
    //# port-specific member to replace "if (extra.tokens)"
    bool tokenTracking;
    vector<TokenRecord> tokenRecords; //called extra.tokens in esprima
    // name changed here to distinguish specific type and different domains
    // of these types.

    bool hasSource;
    string source; 

    bool tokenize;
    unsigned int openParenToken;
    unsigned int openCurlyToken;

    bool range;
    bool loc;

    //# port-specific member to replace "if (extra.comments)"
    bool commentTracking; 
    
    vector<Comment> comments;

    //# port specific member to replace "if (extra.errors)"
    bool errorTolerant;  

    vector<ExError> errors;

    bool attachComment;
    vector<Comment> leadingComments;
    vector<Comment> trailingComments;
    vector< shared_ptr<NodesComments> > bottomRightStack; //! todo Node header text.

    ExtraStruct(OptionsStruct opt);
    void clear();
};
#endif
