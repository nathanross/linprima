#line 2 "linprima.cpp"
#include <vector>
#include <map>
#include <string>
#include <unordered_set>

#ifndef LIBSTDC
#include <locale>
#include <codecvt>
#endif

//algorithm is for find(vector.begin()
#include <algorithm>
#include <functional>
#include <memory>

#ifdef USE_PROFINY
#include "Profiny.h"
#endif
#ifdef THROWABLE
#include <exception>
#endif

//#define pushinline __inline__ __attribute__((always_inline))
//#define reqinline __attribute__((always_inline)) // inline or die
#define reqinline inline //save tweaking this for last. Talking maybe 10ms system / 20 ms asm in one rep of Chart.js (huge file), so pretty low return on optimization.

using namespace rapidjson;
using namespace std;

typedef Document::AllocatorType AllocatorType;


int debuglevel = 1;
vector<string> stackfuncs;
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

//for debugging 
//shows you which entry is unitialized when one of the 
//items in the json is uninitialized (leading to a segfault)
//walks json, printing each path before trying to access/print
//value at that path.
void walkJson(string path, const Value& a) {
    string nextpath;
    if (a.IsObject()) {
        for (Value::ConstMemberIterator itr = a.MemberBegin();
             itr != a.MemberEnd(); ++itr)
            {
                nextpath = path;
                nextpath.append("/");
                nextpath.append(itr->name.GetString());
                printf("Path %s\n", nextpath.data());
                walkJson(nextpath, itr->value);
            }
    } else if (a.IsArray()) {
        int i=0;
        for (Value::ConstValueIterator itr = a.Begin(); 
             itr != a.End(); 
             ++itr) 
            {
                nextpath = path;
                nextpath.append("/");
                nextpath.append(to_string(i));
                printf("Path %s\n", nextpath.data());
                walkJson(nextpath, *itr);
                i++;
            }
    } else if (a.IsInt()) {
        printf("%i\n", a.GetInt());
    } else if (a.IsNull()) {
        printf("null\n");
    } else if (a.IsString()) {
        printf("%s\n", a.GetString());
    }
}


#ifdef DO_DEBUG
#define DEBUGIN(A,B) DEBUG_IN(A,B)
#define DEBUGOUT(A,B) DEBUG_OUT(A,B)
#define DBGRET(A,B) DBG_RET(A,B)

int *debugidx;

bool DEBUG_ON= (bool) 1;

bool HIPRI_ONLY= (bool) 1;

string colorHash(string text) {
    int num=0; 
    string code = "";
    for (int i=0; i<text.length() && i < 7; i++) {
        num += (int) text[i];
    }
    code.append("\033[1;");
    code.append(to_string(31 + (num % 7)));
    code.append("m");
    return code;
}

void DEBUG_IN(string in, bool lowprio) {    
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    debuglevel++;
    string msg = "";

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }    
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(ltrim(in))); }
    msg.append(ltrim(in));
    msg.append(to_string(debuglevel));
    msg.append("|");
    msg.append(to_string(*debugidx));
    msg.append("\033[0m\n");
    printf("%s", msg.data());
    stackfuncs.push_back(ltrim(in));
}

void DEBUG_OUT(string in, bool lowprio) {
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    string msg = "";
    string realtext = "";
    if (stackfuncs.size() > 0) {
        realtext = stackfuncs.back();
    }

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(realtext)); }
    msg.append("~");
    msg.append(ltrim(in));
    if (stackfuncs.size() > 0) {
         msg.append(stackfuncs.back());
         stackfuncs.pop_back();
         msg.append(to_string(debuglevel));
         debuglevel--;
     }  
     msg.append("|");
     msg.append(to_string(*debugidx));
     msg.append("\033[0m\n");
     printf("%s", msg.data());
 }

template<typename T> T DBG_RET(string a, T b) { 
    DEBUG_OUT(a, false); 
    return b; 
}

#endif
#ifndef DO_DEBUG
#define DEBUGIN(A,B) 
#define DEBUGOUT(A,B)
#define DBGRET(A,B) B 
/*
#define DEBUGIN(A,B) TIME_BEGIN(A,B)
#define DEBUGOUT(A,B) TIME_END(A,B)
#define DBGRET(A,B) TIME_END_RET(A,B)

bool DEBUG_ON= (bool) 1;

bool HIPRI_ONLY= (bool) 1;

void TIME_BEGIN(string in, bool lowprio) {    
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    debuglevel++;
    string msg = "";

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }    
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(ltrim(in))); }
    msg.append(ltrim(in));
    msg.append(to_string(debuglevel));
    msg.append("\033[0m\n");
    printf("%s", msg.data());
    stackfuncs.push_back(ltrim(in));
}

void TIME_END(string in, bool lowprio) {
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    string msg = "";
    string realtext = "";
    if (stackfuncs.size() > 0) {
        realtext = stackfuncs.back();
    }

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(realtext)); }
    msg.append("~");
    msg.append(ltrim(in));
    if (stackfuncs.size() > 0) {
        msg.append("@@ ");
         msg.append(stackfuncs.back());
         stackfuncs.pop_back();
         msg.append(to_string(debuglevel));
         debuglevel--;
     }
     msg.append("\033[0m\n");
     printf("%s", msg.data());
 }

template<typename T> T TIME_END_RET(string a, T b) { 
    TIME_END(a, false); 
    return b; 
}
*/


#endif

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
ErrWrapint noErr;

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
    string val;
    ErrWrapstring() {
        err = false;
    }
    ErrWrapstring(string in) {
        val = in;
        err = false;
    }
};

class ErrWrapu16string {
public:
    bool err;
    u16string val;
    ErrWrapu16string() {
        err = false;
    }
    ErrWrapu16string(u16string in) {
        val = in;
        err = false;
    }
};



#endif

#ifdef LIBSTDC

//assume everything is ascii.    
string toU8(const u16string& input) { 
    char * outtmp = new char[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char) ((int) input[i]);
    } 
    string out = string(outtmp, input.length());
    free(outtmp);
    return out;
}

string toU8(const char16_t* input) {
    return toU8(u16string(input));
}

u16string toU16string(const string& input){ 
    char16_t * outtmp = new char16_t[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char16_t) ((int) input[i]);
    } 
    u16string out = u16string(outtmp, input.length());
    free(outtmp);
    return out;
}
#endif
#ifndef LIBSTDC

std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu8;

reqinline 
string toU8(const u16string input){ 
    return convu8.to_bytes(input);
}

string toU8(const char16_t* input) {
  return toU8(u16string(input));
}

wstring toWstring(const string input) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(input);
}

reqinline
u16string toU16string(const string input){ 
    //#TODO moving this outside of the func causes a test failure? look into.
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu16;
    return convu16.from_bytes(input);
}
#endif

struct ExtraStruct; 

template <typename T>
void vec2jsonCallback(Document& root,
                      AllocatorType* alloc,
                      const ExtraStruct *extra,
                      string path,
                      vector<T> in,
                      function<void (T&, 
                                     const ExtraStruct*extra,
                                     Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    Value arr(kArrayType);
    Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    Value pathval(path.data(),
                  path.length(), 
                  *alloc);
    root.AddMember(pathval.Move(), 
                   arr,
                   *alloc);
    //DEBUGOUT("", false);     
}

template <typename T>
void vec2jsonCallbackVal(Value& root,
                      AllocatorType* alloc,
                      const ExtraStruct *extra,
                      string path,
                      vector<T> in,
                      function<void (T&, 
                                     const ExtraStruct*extra,
                                     Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    Value arr(kArrayType);
    Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    Value pathval(path.data(),
                  path.length(), 
                  *alloc);
    root.AddMember(pathval.Move(), 
                   arr,
                   *alloc);
    //DEBUGOUT("", false);     
}


//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
reqinline
bool hasSet(const u16string needle, const unordered_set<u16string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}
reqinline
bool hasSet(const string needle, const unordered_set<string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}

reqinline 
bool has(const u16string needle, const vector<u16string>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const string needle, const vector<string>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const int needle, const vector<int>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
//inline bool has(const int needle, const int haystack[], const int length) {
//    return find(haystack, haystack+length, needle) != haystack+length;
//}

enum class TknType { 
    None = 0,
    BooleanLiteral=1,
    EOFF=2, //#renamed due to name collision with libc(++) macro
    Identifier=3,
    Keyword=4,
    NullLiteral=5,
    NumericLiteral=6,
    Punctuator=7,
    StringLiteral=8,
    RegularExpression=9
};

template<typename T> bool hasStringKey(const string needle, const map<string,T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}


//#define slice(a, b, c) u16string( ((const char16_t *) a+b), c-b)
reqinline
u16string slice(const char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
    const char16_t * startptr = arr + start;    
    return u16string(startptr, (end-start));
    }


//#define appendChar(a, b) a.append(u16string({b}))
reqinline
void appendChar(u16string &base, char16_t tail) { 
    base.append(u16string({tail})); 
    //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.

    }

int parseInt(u16string in_u16, int radix) {  // !!!
    const int zero = 48;
    const int upperA = 65;
    const int lowerA = 97;

    string in = toU8(in_u16);
    int length,
        cur,
        result = 0;
    length = in.length();
    for (int i=0; i<length; i++) {
        cur = (int) in[i];
        if (cur <= zero+9 &&  cur >= zero) {
            cur = cur - zero;
        } else if (cur <= upperA+5 && cur >= upperA) {
            cur = 10 + cur - upperA;
        } else if (cur <= lowerA+5 && cur >= lowerA) {
            cur = 10 + cur - lowerA;
        } else {
            return -1;
        }
        result += (cur * pow(radix,length-(i+1)));
    }
    return result; 
}

double sciNoteToDouble(string in) {
    DEBUGIN("sciNoteToDouble", false);
    char current;
    string factor = "0";
    string exp = "0";
    bool esignPassed = false; //, dotPassed = false;
    for (unsigned int i=0;i<in.length();i++) {
        current = in[i];
        if (current == u'.') {
            //dotPassed = true;
            factor.append(string({current}));
        } else if (current == u'E' || current == u'e' ) {
            esignPassed = true; 
        } else if (esignPassed) {
            if (exp == "0") { exp = ""; }
            exp.append(string({current}));
        } else {
            factor.append(string({current}));               
        }
    }

    DEBUGOUT("sciNoteToDouble", false);
    return stod(factor) * pow(10,stod(exp));
}

const char16_t NULL_CHAR16 = u'X';


// -----------------------

bool strict = false; //? remove initialization?

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
/*Loc::Loc() :
    startLine(-1), startColumn(-1),
    endLine(-1), endColumn(-1), hasSource(false),
    source("") {
} */
Loc::Loc(int lineNumber, int idx, int lineStart) : 
    startLine(lineNumber), startColumn(idx-lineStart),
             endLine(-1), endColumn(-1), hasSource(false),
             source("") {
    DEBUGIN("Loc()", true);
    DEBUGOUT("loc()", true);
}

//convenience function for using l-value (stack/non-temporary) as
//json keys when using AddMember
inline void MemberAdd(Value& out,
                      const string& key, 
                      Value& val, 
                      AllocatorType& alloc) {
    out.AddMember(Value(key.data(), key.length(), alloc).Move(),
                  val,
                  alloc);
}

//adds string via copy semantics.
void jsonAddString(Value& out, AllocatorType* alloc, 
                   string key, string val) {
    MemberAdd(out, key, 
                  Value(val.data(), 
                        val.length(), 
                        *alloc).Move(),
                  *alloc);
}

void Loc::toJson(Value& out, AllocatorType* alloc) const { 
    //DEBUGIN(" locToJson(Loc l)", false);
    Value startjson(kObjectType);
    startjson.AddMember("line", startLine, *alloc);
    startjson.AddMember("column", startColumn, *alloc);
    out.AddMember("start", startjson, *alloc);
    if (this->endLine != -1) {        
        Value endjson(kObjectType);
        endjson.AddMember("line", endLine, *alloc);
        endjson.AddMember("column", endColumn, *alloc);
        out.AddMember("end", endjson, *alloc);
    }
    if (this->hasSource) {
        out.AddMember("source",
                      Value(this->source.data(),
                            this->source.length(), 
                            *alloc).Move(),
                      *alloc);
    }
    //DEBUGOUT("locToJson", false); 
}

struct Comment {
    string type;
    string value;
    int range[2];
    Loc loc;
    Comment(int lineNumber, int idx, int lineStart);
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
};
Comment::Comment(int lineNumber, int idx, int lineStart) :
    loc(lineNumber, idx, lineStart) {
    //DEBUGIN("Comment()", false);
    this->type = -1;
    this->value = "";
    this->range[0] = -1;
    this->range[1] = -1;
    //DEBUGOUT("Comment()", false);
}

//# good to have these separated from individual nodes,
//# because unless we start storing nodes in heap,
//# what happens is we create a node, goes on stack,
//# reference it from extrastruct (storing it is a
//# prohibitive expense due to some expandable members)
//# exits stack, gone, ref. invalid.

//# better to only keep in memory the data necessary.
//# these are the only data members that will be necessary
//# once the node has left the stack (barring Assignment
//# Expressions which which put a nodecopy on the heap)
class NodesComments {
public:
    vector<Comment> leadingComments;
    vector<Comment> trailingComments;
    Value * nodesJv;
    AllocatorType * nodesAlloc;
    int range[2];
    bool isNull;
    NodesComments(AllocatorType* alloc);
    NodesComments(Value& jv,AllocatorType* alloc);
    void commentsIntoJson(const ExtraStruct *extra,
                          const bool leading);
};

NodesComments::NodesComments(AllocatorType* alloc): 
    nodesAlloc(alloc) {
    nodesJv = 0x0;
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}
NodesComments::NodesComments(Value& jv, AllocatorType* alloc) : 
    nodesAlloc(alloc) {
    this->nodesJv = &jv;
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}

void NodesComments::commentsIntoJson(const ExtraStruct *extra,
                                     const bool leading) { 
    //DEBUGIN(" NodesComments::commentsIntoJson(const bool leading)", false);
    string key;
    vector<Comment> * commentVec;
    if (leading) {
        key = "leadingComments";
        commentVec = &leadingComments;
    } else {
        key = "trailingComments";
        commentVec = &trailingComments;
    }
    Value::ConstMemberIterator itr = nodesJv->FindMember(key.data());
    if (itr != nodesJv->MemberEnd())  {
        nodesJv->EraseMember(itr);
        //switch to RemoveMember if this function becomes timesink.
    }
    if (commentVec->size() > 0) {
        vec2jsonCallbackVal<Comment>(*nodesJv,
                                  nodesAlloc,
                                  extra,
                                  key.data(),
                                  *commentVec,
                                  &Comment::toJson);
    }
    //DEBUGOUT("commentsIntoJSon", false);
}

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
ExError::ExError() {
    description = "unknown error";
    index = 0;
    lineNumber = 0;
    column = 0;
}
void ExError::toJson(const ExtraStruct *extra,
                     Value& out, AllocatorType* alloc) {
    DEBUGIN("Error::toJSON", false);
    out.AddMember("isError", true, *alloc);
    jsonAddString(out, alloc, "description", description);
    out.AddMember("index", this->index, *alloc);
    out.AddMember("lineNumber", this->lineNumber, *alloc);
    out.AddMember("column", this->column, *alloc);
    DEBUGOUT("Error::toJSON", false); 
}
void ExError::toJsonTolerant(const ExtraStruct *extra,
                             Value& out, AllocatorType* alloc) {
    DEBUGIN("Error::toJSON", false);
    jsonAddString(out, alloc, "description", description);
    out.AddMember("index", this->index, *alloc);
    out.AddMember("lineNumber", this->lineNumber, *alloc);
    out.AddMember("column", this->column, *alloc);
    DEBUGOUT("Error::toJSON", false); 
}

#ifndef THROWABLE

ExError retError;
int errorType = 0;

class AssertError {
public:
    string description;
    AssertError();
    void toJson(const ExtraStruct *extra,
                Value& out, AllocatorType* alloc);
};
AssertError::AssertError() {
    description = "";
}
void AssertError::toJson(const ExtraStruct *extra,
                         Value& out, AllocatorType* alloc) {
    jsonAddString(out, alloc, 
                  "message", description);
    out.AddMember("isAssert", true, *alloc);
}

AssertError retAssertError;


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

TokenStruct::TokenStruct(int lineNumber, int idx, int lineStart) :    
    start(-1), lineNumber(-1), lineStart(-1),
    startLineNumber(-1), startLineStart(-1), end(-1),
    loc(lineNumber, idx, lineStart) {
    DEBUGIN("TokenStruct()", true);
#ifndef THROWABLE
    err = false;
#endif
    isNull = false;
    type = TknType::None;
    octal=false; 
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenStruct()", true);
}

typedef shared_ptr<TokenStruct> ptrTkn;

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
TokenRecord::TokenRecord(Loc locArg) : loc(locArg) {
    DEBUGIN("TokenRecord()", true);
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenRecord()", true);
}
TokenRecord::TokenRecord(int lineNumber, int idx, int lineStart) :
    loc(lineNumber, idx, lineStart) {
    DEBUGIN("TokenRecord()", true);
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenRecord()", true);
}

struct LinprimaTask;

enum class Synt;
class Node {
public:
    string type;

    bool hasJv;
    Value jv;
    Loc loc;
#ifndef THROWABLE
    bool err;
#endif
    bool hasLoc;
    bool hasRange;
    int range[2];
    vector< vector<string> > regexPaths; //lin only. obv.

    shared_ptr<NodesComments> thisnc;

    string name;//for identifiers
    vector< Node* > expressions; //for sequence expressions.
    Node* leftAssign; //for assignment+reinterpretAsCover...
    Node* rightAssign; //same

    AllocatorType* alloc;
    vector<Node*>* heapNodes;

    string s(const u16string in);
    Node(bool lookaheadAvail, bool storeStats, 
         vector<Node*>* heapNodes,
         AllocatorType *alloc,
         LinprimaTask *task);
    void lookavailInit();
    void clear();
    void unused();
    void jvput(const char* path, const string b);
    void jvput(const char* path, const int b); 
    void jvput(const char* path, const bool b);
    void jvput_dbl(const char* path, const double b);
    void jvput_null(const char* path); 
    void regNoadd(const vector<string> paths, 
                  Node * child);
    void reg(const string path, 
             Node * child);
    void nodeVec(const string path, 
                 const vector<Node*>& nodes);
    void addType(const Synt in);
    void regexPaths2json(Value& out);
    //void commentsIntoJson(const bool leading);
    void processComment();
    void finish();
    void finishArrayExpression(const vector< Node* >& elements);
    void finishArrowFunctionExpression(const vector< Node* >& params, 
                                       const vector< Node* >& defaults, 
                                       Node * body, 
                                       const bool expression);
    void finishAssignmentExpression(const string oper,
                                    Node *left, 
                                    Node *right);
    void finishBinaryExpression(const string oper, 
                                Node *left, 
                                Node *right);
    void finishBlockStatement(const vector< Node* >& body);
    void finishBreakStatement(Node * label);
    void finishCallExpression(Node * callee, 
                              const vector< Node* >& args);
    void finishCatchClause(Node * param, 
                           Node * body);
    void finishConditionalExpression(Node * test,
                                     Node * consequent, 
                                     Node * alternate);
    void finishContinueStatement(Node * label);
    void finishDebuggerStatement();
    void finishDoWhileStatement(Node * body,
                                Node * test);
    void finishEmptyStatement();
    void finishExpressionStatement(Node * expression);
    void finishForStatement(Node * init, 
                            Node * test, 
                            Node * update, 
                            Node * body);
    void finishForInStatement(Node * left, 
                              Node * right, 
                              Node * body);
    void finishFunctionDeclaration(Node * id, 
                                   const vector< Node* >& params, 
                                   const vector< Node* >& defaults, 
                                   Node * body);
    void finishFunctionExpression(Node * id, 
                                  const vector< Node* >& params, 
                                  const vector< Node* >& defaults, 
                                  Node * body);
    void finishIdentifier(const string name);
    void finishIfStatement(Node * test, 
                           Node * consequent, 
                           Node * alternate); 
    void finishLabeledStatement(Node * label, 
                                Node * body);
    void finishLiteral(ptrTkn token);
    void finishMemberExpression(const char16_t accessor, 
                                Node * object, 
                                Node * property);
    void finishNewExpression(Node * callee, 
                             const vector<Node*>& args);
    void finishObjectExpression(const vector<Node*>& properties);
    void finishPostfixExpression(const string oper, 
                                 Node * argument);
    void finishProgram(const vector<Node*>& body);
    void finishProperty(const string kind, 
                        Node * key, 
                        Node * value);
    void finishReturnStatement(Node * argument);
    void finishSequenceExpression(const vector<Node*>& expressions);
    void finishSwitchCase(Node * test, 
                          const vector<Node*>& consequent);
    void finishSwitchStatement(Node * discriminant, 
                               const vector<Node*>& cases);
    void finishThisExpression();
    void finishThrowStatement(Node * argument);
    void finishTryStatement(Node * block, 
                            const vector<Node*>& guardedHandlers, 
                            const vector<Node*>& handlers, 
                            Node * finalizer);
    void finishUnaryExpression(const string oper, 
                               Node * argument);
    void finishVariableDeclaration(const vector<Node*>& declarations, 
                                   const string kind);
    void finishVariableDeclarator(Node * id, 
                                  Node * init);
    void finishWhileStatement(Node * test,
                              Node * body);
    void finishWithStatement(Node * object, 
                             Node * body);
    void delNode(Node * toDel);

private:
    LinprimaTask * task;

}; 

    
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

//class ErrWrapValueRef {
//};

class ErrWrapNodePtr {
public:
    bool err;
    Node* val;
    ErrWrapNodePtr() {
        err = false;
    }
    ErrWrapNodePtr(Node* in) {
        val = in;
        err = false;
    }
};

class ErrWrapvectorNodePtr {
public:
    bool err;
    vector<Node*> val;
    ErrWrapvectorNodePtr() {
        err = false;
    }
    ErrWrapvectorNodePtr(vector<Node*> in) {
        val = in;
        err = false;
    }
};
#endif

TokenStruct NULLTOKEN(-1,-1,-1);
ptrTkn NULLPTRTKN;
Node * NULLNODE = 0x0;

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

StateStruct::StateStruct() { 
    allowIn = true;
    inFunctionBody = false;
    inIteration = false;
    inSwitch = false;
    lastCommentStart = -1;
    parenthesisCount = 0; //? parse only?
}

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

OptionsStruct::OptionsStruct() {
    DEBUGIN("OptionsStruct()", true);
    range = false;
    loc = false;
    comment = false;
    tolerant = false;
    attachComment = false;
    tokens = false;
    tokenize = false;
    hasSource = false;
    DEBUGOUT("OptionsStruct()", true);
}
bool OptionsStruct::json_getbool(Value& jsonMap, 
                                 const string key, 
                                 const bool defaultVal) {
    Value::ConstMemberIterator itr = jsonMap.FindMember(key.data());
    if (itr != jsonMap.MemberEnd()) {        
        
        if (itr->value.IsBool()) {
            bool result = itr->value.GetBool();
            return result;
        } 
    }
    return defaultVal;
}
OptionsStruct::OptionsStruct(const char *in_o) {
    DEBUGIN("OptionsStruct(char*)", true);
    Document d;
    d.Parse(in_o);
    tokenize = false;
    if (d.HasParseError()) {
        //#I don't think this will ever come up outside of manual
        //# debugging unless there's some serious encoding error.
        DEBUGIN("failed to parse options string provided", false);
        range = false;
        loc = false;
        comment = false;
        tolerant = false;
        attachComment = false;
        tokens = false;
        hasSource = false;
    } else { 
        range = json_getbool(d, "range", false);
        loc = json_getbool(d, "loc", false);
        attachComment = json_getbool(d, "attachComment", false);
        comment = json_getbool(d, "comment", false);
        tolerant = json_getbool(d, "tolerant", false);
        tokens = json_getbool(d, "tokens", false);
        Value::ConstMemberIterator itr = d.FindMember("source");
        hasSource = (itr != d.MemberEnd() 
                     && itr->value.IsString());
        if (hasSource) {
            source = itr->value.GetString();            
        }
    }
    DEBUGOUT("OptionsStruct(char*)", true);
}

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

ExtraStruct::ExtraStruct(OptionsStruct opt) {
    tokenize = opt.tokenize;
    tokenTracking = opt.tokens;
    range = opt.range;    
    loc = opt.loc;
    errorTolerant = opt.tolerant;
    commentTracking = opt.comment;
    attachComment = opt.attachComment;
    if (loc && opt.hasSource) {
        hasSource = true;
        source = opt.source;
    } else { hasSource = false; }
    if (attachComment) {
        range = true;
        commentTracking = true;
    }

    this->clear();
    //openParenToken = -1;
    //openCurlyToken = -1;
}
void ExtraStruct::clear() {
    tokenRecords.clear();
    comments.clear();
    errors.clear();
    leadingComments.clear();
    trailingComments.clear();
}


struct ParseParamsOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    ptrTkn firstRestricted;
    ptrTkn stricted;
    unordered_set<string> paramSet;
    string message;
    ParseParamsOptions();
};

ParseParamsOptions::ParseParamsOptions() {
    firstRestricted = 0x0; 
    stricted = 0x0; 
}



struct ParseParamsOut {
#ifndef THROWABLE
    bool err;
#endif
    ptrTkn firstRestricted;
    ptrTkn stricted;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
    ParseParamsOut();
};

ParseParamsOut::ParseParamsOut() {
#ifndef THROWABLE
    err = false;
#endif
    message="";
    firstRestricted = 0x0; 
    stricted = 0x0; 
}


struct ReinterpretOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    Node *firstRestricted;
    Node *stricted;
    unordered_set<string> paramSet;
    string message;
    ReinterpretOptions();
};

ReinterpretOptions::ReinterpretOptions() {
    firstRestricted = NULLNODE; //?
    stricted = NULLNODE; //? nec.? had it only in reinOut before.
    //? not sure if context will make it different.
}

struct ReinterpretOut {
    Node *firstRestricted;
    Node *stricted;
    bool isNull;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
#ifndef THROWABLE
    bool err;
#endif
    void* rest; //seems to be a dummy var?
    ReinterpretOut();
};
ReinterpretOut::ReinterpretOut() {
#ifndef THROWABLE
    err = false;
#endif
    isNull=false;
    firstRestricted = NULLNODE;
    stricted = NULLNODE;
}

//---- ----------  -----------------------------
// signatures (temporary until we set up a header file):

map<TknType, string> TokenName = {
    {TknType::BooleanLiteral, "Boolean"},
    {TknType::EOFF, "<end>"},
    {TknType::Identifier, "Identifier"},
    {TknType::Keyword, "Keyword"},
    {TknType::NullLiteral, "Null"},
    {TknType::NumericLiteral, "Numeric"},
    {TknType::Punctuator, "Punctuator"},
    {TknType::StringLiteral, "String"},
    {TknType::RegularExpression, "RegularExpression"}
};

void TokenRecord::toJson(const ExtraStruct *extra,                         
                         Value& out, AllocatorType* alloc) {
    DEBUGIN(" TokenRecord::toJson", false);    
    jsonAddString(out, alloc, "type", TokenName[this->type]);
    jsonAddString(out, alloc, "value", this->valuestring);
    if (extra->range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        out.AddMember("range", rangearr, *alloc);
    }
    if (extra->loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        out.AddMember("loc", locjson, *alloc);
    }
    DEBUGOUT("TokenRecord::toJson()", false);
}

void Comment::toJson(const ExtraStruct *extra,
                     Value& out, AllocatorType* alloc) {
    DEBUGIN("Comment::toJson", false);
    jsonAddString(out, alloc, "type", this->type);
    jsonAddString(out, alloc, "value", this->value);
    if (extra->range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        out.AddMember("range", rangearr, *alloc);
    }
    if (extra->loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        out.AddMember("loc", locjson, *alloc);
    }
    DEBUGOUT("comment::toJson", false);
}


 //#C++ specific type specifier
map<string, int> LiteralType = {
    {"String", 1},
    {"Int", 2},
    {"Double", 3},
    {"Regexp", 4},
    {"Bool", 5},
    {"Null", 6}
};

vector< string > FnExprTokens = {
    // A function following one of those tokens is an expression.
    "(", "{", "[", "in", "typeof", "instanceof", "new",
    "return", "case", "delete", "throw", "void",
    // assignment operators                                      
    "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", ">>>=",
    "&=", "|=", "^=", ",",
    // binary/unary operators            
    "+", "-", "*", "/", "%", "++", "--", "<<", ">>", ">>>", "&",
    "|", "^", "!", "~", "&&", "||", "?", ":", "===", "==", ">=",
    "<=", "<", ">", "!=", "!=="
};

enum class Synt {
    AssignmentExpression,
    ArrayExpression,
    ArrowFunctionExpression,
    BlockStatement,
    BinaryExpression,
    BreakStatement,
    CallExpression,
    CatchClause,
    ConditionalExpression,
    ContinueStatement,
    DoWhileStatement,
    DebuggerStatement,
    EmptyStatement,
    ExpressionStatement,
    ForStatement,
    ForInStatement,
    FunctionDeclaration,
    FunctionExpression,
    Identifier,
    IfStatement,
    Literal,
    LabeledStatement,
    LogicalExpression,
    MemberExpression,
    NewExpression,
    ObjectExpression,
    Program,
    Property,
    ReturnStatement,
    SequenceExpression,
    SwitchStatement,
    SwitchCase,
    ThisExpression,
    ThrowStatement,
    TryStatement,
    UnaryExpression,
    UpdateExpression,
    VariableDeclaration,
    VariableDeclarator,
    WhileStatement,
    WithStatement
};

map<Synt, string> Syntax = {
    {Synt::AssignmentExpression, "AssignmentExpression"},
    {Synt::ArrayExpression, "ArrayExpression"},
    {Synt::ArrowFunctionExpression, "ArrowFunctionExpression"},
    {Synt::BlockStatement, "BlockStatement"},
    {Synt::BinaryExpression, "BinaryExpression"},
    {Synt::BreakStatement, "BreakStatement"},
    {Synt::CallExpression, "CallExpression"},
    {Synt::CatchClause, "CatchClause"},
    {Synt::ConditionalExpression, "ConditionalExpression"},
    {Synt::ContinueStatement, "ContinueStatement"},
    {Synt::DoWhileStatement, "DoWhileStatement"},
    {Synt::DebuggerStatement, "DebuggerStatement"},
    {Synt::EmptyStatement, "EmptyStatement"},
    {Synt::ExpressionStatement, "ExpressionStatement"},
    {Synt::ForStatement, "ForStatement"},
    {Synt::ForInStatement, "ForInStatement"},
    {Synt::FunctionDeclaration, "FunctionDeclaration"},
    {Synt::FunctionExpression, "FunctionExpression"},
    {Synt::Identifier, "Identifier"},
    {Synt::IfStatement, "IfStatement"},
    {Synt::Literal, "Literal"},
    {Synt::LabeledStatement, "LabeledStatement"},
    {Synt::LogicalExpression, "LogicalExpression"},
    {Synt::MemberExpression, "MemberExpression"},
    {Synt::NewExpression, "NewExpression"},
    {Synt::ObjectExpression, "ObjectExpression"},
    {Synt::Program, "Program"},
    {Synt::Property, "Property"},
    {Synt::ReturnStatement, "ReturnStatement"},
    {Synt::SequenceExpression, "SequenceExpression"},
    {Synt::SwitchStatement, "SwitchStatement"},
    {Synt::SwitchCase, "SwitchCase"},
    {Synt::ThisExpression, "ThisExpression"},
    {Synt::ThrowStatement, "ThrowStatement"},
    {Synt::TryStatement, "TryStatement"},
    {Synt::UnaryExpression, "UnaryExpression"},
    {Synt::UpdateExpression, "UpdateExpression"},
    {Synt::VariableDeclaration, "VariableDeclaration"},
    {Synt::VariableDeclarator, "VariableDeclarator"},
    {Synt::WhileStatement, "WhileStatement"},
    {Synt::WithStatement, "WithStatement"}
};

unique_ptr<Node> make_unique_Node() {
    unique_ptr<Node> a(new Node(false, false, 0x0, 0x0, 0x0));
    return a;
}

map<string, unique_ptr<Node>> PlaceHolders { 
};
 //map<string, int> 


map<string, int> PropertyKind = {
    {"Data", 1},
    {"Get", 2},
    {"Set", 4}
};

 // Error messages should be identical to V8.
enum class Mssg {
    None,
    UnexpectedToken,
    UnexpectedNumber,
    UnexpectedString,
    UnexpectedIdentifier,
    UnexpectedReserved,
    UnexpectedEOS,
    NewlineAfterThrow,
    InvalidRegExp,
    UnterminatedRegExp,
    InvalidLHSInAssignment,
    InvalidLHSInForIn,
    MultipleDefaultsInSwitch,
    NoCatchOrFinally,
    UnknownLabel,
    Redeclaration,
    IllegalContinue,
    IllegalBreak,
    IllegalReturn,
    StrictModeWith,
    StrictCatchVariable,
    StrictVarName,
    StrictParamName,
    StrictParamDupe,
    StrictFunctionName,
    StrictOctalLiteral,
    StrictDelete,
    StrictDuplicateProperty,
    AccessorDataProperty,
    AccessorGetSet,
    StrictLHSAssignment,
    StrictLHSPostfix,
    StrictLHSPrefix,
    StrictReservedWord
};
map<Mssg, string> Messages = {
    {Mssg::UnexpectedToken, "Unexpected token %0"},
    {Mssg::UnexpectedNumber, "Unexpected number"},
    {Mssg::UnexpectedString, "Unexpected string"},
    {Mssg::UnexpectedIdentifier, "Unexpected identifier"},
    {Mssg::UnexpectedReserved, "Unexpected reserved word"},
    {Mssg::UnexpectedEOS, "Unexpected end of input"},
    {Mssg::NewlineAfterThrow, "Illegal newline after throw"},
    {Mssg::InvalidRegExp, "Invalid regular expression"},
    {Mssg::UnterminatedRegExp, "Invalid regular expression: missing /"},
    {Mssg::InvalidLHSInAssignment, "Invalid left-hand side in assignment"},
    {Mssg::InvalidLHSInForIn, "Invalid left-hand side in for-in"},
    {Mssg::MultipleDefaultsInSwitch, "More than one default clause in switch statement"},
    {Mssg::NoCatchOrFinally, "Missing catch or finally after try"},
    {Mssg::UnknownLabel, "Undefined label \'%0\'"},
    {Mssg::Redeclaration, "%0 \'%1\' has already been declared"},
    {Mssg::IllegalContinue, "Illegal continue statement"},
    {Mssg::IllegalBreak, "Illegal break statement"},
    {Mssg::IllegalReturn, "Illegal return statement"},
    {Mssg::StrictModeWith, "Strict mode code may not include a with statement"},
    {Mssg::StrictCatchVariable, "Catch variable may not be eval or arguments in strict mode"},
    {Mssg::StrictVarName, "Variable name may not be eval or arguments in strict mode"},
    {Mssg::StrictParamName, "Parameter name eval or arguments is not allowed in strict mode"},
    {Mssg::StrictParamDupe, "Strict mode function may not have duplicate parameter names"},
    {Mssg::StrictFunctionName, "Function name may not be eval or arguments in strict mode"},
    {Mssg::StrictOctalLiteral, "Octal literals are not allowed in strict mode."},
    {Mssg::StrictDelete, "Delete of an unqualified identifier in strict mode."},
    {Mssg::StrictDuplicateProperty, "Duplicate data property in object literal not allowed in strict mode"},
    {Mssg::AccessorDataProperty, "Object literal may not have data and accessor property with the same name"},
    {Mssg::AccessorGetSet, "Object literal may not have multiple get/set accessors with the same name"},
    {Mssg::StrictLHSAssignment, "Assignment to eval or arguments is not allowed in strict mode"},
    {Mssg::StrictLHSPostfix, "Postfix increment/decrement may not have eval or arguments operand in strict mode"},
    {Mssg::StrictLHSPrefix, "Prefix increment/decrement may not have eval or arguments operand in strict mode"},
    {Mssg::StrictReservedWord, "Use of future reserved word in strict mode"}
};



    vector< vector <unsigned int> > nonasciiIdentifierstart = { {170,181,186,192,216,248,710,736,748,750,880,886,887,890,895,902,904,908,910,931,1015,1162,1329,1369,1377,1488,1520,1568,1646,1647,1649,1749,1765,1766,1774,1775,1786,1791,1808,1810,1869,1969,1994,2036,2037,2042,2048,2074,2084,2088,2112,2208,2308,2365,2384,2392,2417,2437,2447,2448,2451,2474,2482,2486,2493,2510,2524,2525,2527,2544,2545,2565,2575,2576,2579,2602,2610,2611,2613,2614,2616,2617,2649,2654,2674,2693,2703,2707,2730,2738,2739,2741,2749,2768,2784,2785,2821,2831,2832,2835,2858,2866,2867,2869,2877,2908,2909,2911,2929,2947,2949,2958,2962,2969,2970,2972,2974,2975,2979,2980,2984,2990,3024,3077,3086,3090,3114,3133,3160,3161,3168,3169,3205,3214,3218,3242,3253,3261,3294,3296,3297,3313,3314,3333,3342,3346,3389,3406,3424,3425,3450,3461,3482,3507,3517,3520,3585,3634,3635,3648,3713,3714,3716,3719,3720,3722,3725,3732,3737,3745,3749,3751,3754,3755,3757,3762,3763,3773,3776,3782,3804,3840,3904,3913,3976,4096,4159,4176,4186,4193,4197,4198,4206,4213,4238,4256,4295,4301,4304,4348,4682,4688,4696,4698,4704,4746,4752,4786,4792,4800,4802,4808,4824,4882,4888,4992,5024,5121,5743,5761,5792,5870,5888,5902,5920,5952,5984,5998,6016,6103,6108,6176,6272,6314,6320,6400,6480,6512,6528,6593,6656,6688,6823,6917,6981,7043,7086,7087,7098,7168,7245,7258,7401,7406,7413,7414,7424,7680,7960,7968,8008,8016,8025,8027,8029,8031,8064,8118,8126,8130,8134,8144,8150,8160,8178,8182,8305,8319,8336,8450,8455,8458,8469,8473,8484,8486,8488,8490,8495,8508,8517,8526,8544,11264,11312,11360,11499,11506,11507,11520,11559,11565,11568,11631,11648,11680,11688,11696,11704,11712,11720,11728,11736,11823,12293,12321,12337,12344,12353,12445,12449,12540,12549,12593,12704,12784,13312,19968,40960,42192,42240,42512,42538,42539,42560,42623,42656,42775,42786,42891,42896,42928,42929,42999,43011,43015,43020,43072,43138,43250,43259,43274,43312,43360,43396,43471,43488,43494,43514,43520,43584,43588,43616,43642,43646,43697,43701,43702,43705,43712,43714,43739,43744,43762,43777,43785,43793,43808,43816,43824,43868,43876,43877,43968,44032,55216,55243,63744,64112,64256,64275,64285,64287,64298,64312,64318,64320,64321,64323,64324,64326,64467,64848,64914,65008,65136,65142,65313,65345,65382,65474,65482,65490,65498}, {170,181,186,214,246,705,721,740,748,750,884,886,887,893,895,902,906,908,929,1013,1153,1327,1366,1369,1415,1514,1522,1610,1646,1647,1747,1749,1765,1766,1774,1775,1788,1791,1808,1839,1957,1969,2026,2036,2037,2042,2069,2074,2084,2088,2136,2226,2361,2365,2384,2401,2432,2444,2447,2448,2472,2480,2482,2489,2493,2510,2524,2525,2529,2544,2545,2570,2575,2576,2600,2608,2610,2611,2613,2614,2616,2617,2652,2654,2676,2701,2705,2728,2736,2738,2739,2745,2749,2768,2784,2785,2828,2831,2832,2856,2864,2866,2867,2873,2877,2908,2909,2913,2929,2947,2954,2960,2965,2969,2970,2972,2974,2975,2979,2980,2986,3001,3024,3084,3088,3112,3129,3133,3160,3161,3168,3169,3212,3216,3240,3251,3257,3261,3294,3296,3297,3313,3314,3340,3344,3386,3389,3406,3424,3425,3455,3478,3505,3515,3517,3526,3632,3634,3635,3654,3713,3714,3716,3719,3720,3722,3725,3735,3743,3747,3749,3751,3754,3755,3760,3762,3763,3773,3780,3782,3807,3840,3911,3948,3980,4138,4159,4181,4189,4193,4197,4198,4208,4225,4238,4293,4295,4301,4346,4680,4685,4694,4696,4701,4744,4749,4784,4789,4798,4800,4805,4822,4880,4885,4954,5007,5108,5740,5759,5786,5866,5880,5900,5905,5937,5969,5996,6000,6067,6103,6108,6263,6312,6314,6389,6430,6509,6516,6571,6599,6678,6740,6823,6963,6987,7072,7086,7087,7141,7203,7247,7293,7404,7409,7413,7414,7615,7957,7965,8005,8013,8023,8025,8027,8029,8061,8116,8124,8126,8132,8140,8147,8155,8172,8180,8188,8305,8319,8348,8450,8455,8467,8469,8477,8484,8486,8488,8493,8505,8511,8521,8526,8584,11310,11358,11492,11502,11506,11507,11557,11559,11565,11623,11631,11670,11686,11694,11702,11710,11718,11726,11734,11742,11823,12295,12329,12341,12348,12438,12447,12538,12543,12589,12686,12730,12799,19893,40908,42124,42237,42508,42527,42538,42539,42606,42653,42735,42783,42888,42894,42925,42928,42929,43009,43013,43018,43042,43123,43187,43255,43259,43301,43334,43388,43442,43471,43492,43503,43518,43560,43586,43595,43638,43642,43695,43697,43701,43702,43709,43712,43714,43741,43754,43764,43782,43790,43798,43814,43822,43866,43871,43876,43877,44002,55203,55238,55291,64109,64217,64262,64279,64285,64296,64310,64316,64318,64320,64321,64323,64324,64433,64829,64911,64967,65019,65140,65276,65338,65370,65470,65479,65487,65495,65500} };
    vector< vector < unsigned int> > nonasciiIdentifierpart = { {170,181,186,192,216,248,710,736,748,750,768,886,887,890,895,902,904,908,910,931,1015,1155,1162,1329,1369,1377,1425,1471,1473,1474,1476,1477,1479,1488,1520,1552,1568,1646,1749,1759,1770,1791,1808,1869,1984,2042,2048,2112,2208,2276,2406,2417,2437,2447,2448,2451,2474,2482,2486,2492,2503,2504,2507,2519,2524,2525,2527,2534,2561,2565,2575,2576,2579,2602,2610,2611,2613,2614,2616,2617,2620,2622,2631,2632,2635,2641,2649,2654,2662,2689,2693,2703,2707,2730,2738,2739,2741,2748,2759,2763,2768,2784,2790,2817,2821,2831,2832,2835,2858,2866,2867,2869,2876,2887,2888,2891,2902,2903,2908,2909,2911,2918,2929,2946,2947,2949,2958,2962,2969,2970,2972,2974,2975,2979,2980,2984,2990,3006,3014,3018,3024,3031,3046,3072,3077,3086,3090,3114,3133,3142,3146,3157,3158,3160,3161,3168,3174,3201,3205,3214,3218,3242,3253,3260,3270,3274,3285,3286,3294,3296,3302,3313,3314,3329,3333,3342,3346,3389,3398,3402,3415,3424,3430,3450,3458,3459,3461,3482,3507,3517,3520,3530,3535,3542,3544,3558,3570,3571,3585,3648,3664,3713,3714,3716,3719,3720,3722,3725,3732,3737,3745,3749,3751,3754,3755,3757,3771,3776,3782,3784,3792,3804,3840,3864,3865,3872,3893,3895,3897,3902,3913,3953,3974,3993,4038,4096,4176,4256,4295,4301,4304,4348,4682,4688,4696,4698,4704,4746,4752,4786,4792,4800,4802,4808,4824,4882,4888,4957,4992,5024,5121,5743,5761,5792,5870,5888,5902,5920,5952,5984,5998,6002,6003,6016,6103,6108,6109,6112,6155,6160,6176,6272,6320,6400,6432,6448,6470,6512,6528,6576,6608,6656,6688,6752,6783,6800,6823,6832,6912,6992,7019,7040,7168,7232,7245,7376,7380,7416,7417,7424,7676,7960,7968,8008,8016,8025,8027,8029,8031,8064,8118,8126,8130,8134,8144,8150,8160,8178,8182,8204,8205,8255,8256,8276,8305,8319,8336,8400,8417,8421,8450,8455,8458,8469,8473,8484,8486,8488,8490,8495,8508,8517,8526,8544,11264,11312,11360,11499,11520,11559,11565,11568,11631,11647,11680,11688,11696,11704,11712,11720,11728,11736,11744,11823,12293,12321,12337,12344,12353,12441,12442,12445,12449,12540,12549,12593,12704,12784,13312,19968,40960,42192,42240,42512,42560,42612,42623,42655,42775,42786,42891,42896,42928,42929,42999,43072,43136,43216,43232,43259,43264,43312,43360,43392,43471,43488,43520,43584,43600,43616,43642,43739,43744,43762,43777,43785,43793,43808,43816,43824,43868,43876,43877,43968,44012,44013,44016,44032,55216,55243,63744,64112,64256,64275,64285,64298,64312,64318,64320,64321,64323,64324,64326,64467,64848,64914,65008,65024,65056,65075,65076,65101,65136,65142,65296,65313,65343,65345,65382,65474,65482,65490,65498}, {170,181,186,214,246,705,721,740,748,750,884,886,887,893,895,902,906,908,929,1013,1153,1159,1327,1366,1369,1415,1469,1471,1473,1474,1476,1477,1479,1514,1522,1562,1641,1747,1756,1768,1788,1791,1866,1969,2037,2042,2093,2139,2226,2403,2415,2435,2444,2447,2448,2472,2480,2482,2489,2500,2503,2504,2510,2519,2524,2525,2531,2545,2563,2570,2575,2576,2600,2608,2610,2611,2613,2614,2616,2617,2620,2626,2631,2632,2637,2641,2652,2654,2677,2691,2701,2705,2728,2736,2738,2739,2745,2757,2761,2765,2768,2787,2799,2819,2828,2831,2832,2856,2864,2866,2867,2873,2884,2887,2888,2893,2902,2903,2908,2909,2915,2927,2929,2946,2947,2954,2960,2965,2969,2970,2972,2974,2975,2979,2980,2986,3001,3010,3016,3021,3024,3031,3055,3075,3084,3088,3112,3129,3140,3144,3149,3157,3158,3160,3161,3171,3183,3203,3212,3216,3240,3251,3257,3268,3272,3277,3285,3286,3294,3299,3311,3313,3314,3331,3340,3344,3386,3396,3400,3406,3415,3427,3439,3455,3458,3459,3478,3505,3515,3517,3526,3530,3540,3542,3551,3567,3570,3571,3642,3662,3673,3713,3714,3716,3719,3720,3722,3725,3735,3743,3747,3749,3751,3754,3755,3769,3773,3780,3782,3789,3801,3807,3840,3864,3865,3881,3893,3895,3897,3911,3948,3972,3991,4028,4038,4169,4253,4293,4295,4301,4346,4680,4685,4694,4696,4701,4744,4749,4784,4789,4798,4800,4805,4822,4880,4885,4954,4959,5007,5108,5740,5759,5786,5866,5880,5900,5908,5940,5971,5996,6000,6002,6003,6099,6103,6108,6109,6121,6157,6169,6263,6314,6389,6430,6443,6459,6509,6516,6571,6601,6617,6683,6750,6780,6793,6809,6823,6845,6987,7001,7027,7155,7223,7241,7293,7378,7414,7416,7417,7669,7957,7965,8005,8013,8023,8025,8027,8029,8061,8116,8124,8126,8132,8140,8147,8155,8172,8180,8188,8204,8205,8255,8256,8276,8305,8319,8348,8412,8417,8432,8450,8455,8467,8469,8477,8484,8486,8488,8493,8505,8511,8521,8526,8584,11310,11358,11492,11507,11557,11559,11565,11623,11631,11670,11686,11694,11702,11710,11718,11726,11734,11742,11775,11823,12295,12335,12341,12348,12438,12441,12442,12447,12538,12543,12589,12686,12730,12799,19893,40908,42124,42237,42508,42539,42607,42621,42653,42737,42783,42888,42894,42925,42928,42929,43047,43123,43204,43225,43255,43259,43309,43347,43388,43456,43481,43518,43574,43597,43609,43638,43714,43741,43759,43766,43782,43790,43798,43814,43822,43866,43871,43876,43877,44010,44012,44013,44025,55203,55238,55291,64109,64217,64262,64279,64296,64310,64316,64318,64320,64321,64323,64324,64433,64829,64911,64967,65019,65039,65069,65075,65076,65103,65140,65276,65305,65338,65343,65370,65470,65479,65487,65495,65500} };

void initglobals() {
    DEBUGIN(" initglobals()", true);    
    PlaceHolders["ArrowParameterPlaceHolder"] = make_unique_Node(); 
    PlaceHolders["ArrowParameterPlaceHolder"]->type="ArrowParameterPlaceholder";
    NULLTOKEN.isNull = true;
    NULLPTRTKN = make_shared<TokenStruct>(-1,-1,-1);
    (*NULLPTRTKN).isNull = true;

    DEBUGOUT("", true);
}

  // Ensure the condition is true, otherwise throw an error.
  // This is only to have a better contract semantic, i.e. another safety net
  // to catch a logic error. The condition shall be fulfilled in normal case.
  // Do NOT use this to enforce a certain condition on any user input.



#ifndef THROWABLE
//throw_
int softAssert(const bool condition, const string message) {
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
void softAssert(const bool condition, const string message) {
    DEBUGIN(" assert(bool condition, string message)", false);

    string providedMessage = "ASSERT: ";
    providedMessage.append(message);
    if (! condition)
        { throw runtime_error(providedMessage); }
    DEBUGOUT("", false);
 }

#endif

reqinline
bool isDecimalDigit(const char16_t& ch) {
    DEBUGIN("   isDecimalDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch >= 0x30 && ch <= 0x39); //0..9
}

reqinline
bool isHexDigit(const char16_t& ch) {
    DEBUGIN("   isHexDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) 
           != std::string::npos);    
}

reqinline
bool isOctalDigit(const char16_t& ch) {
    DEBUGIN("   isOctalDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (u16string({u"01234567"}).find_first_of(ch) 
            != std::string::npos);    
}

char16_t toLowercaseHex(const char16_t& ch) { //used in scanHexEscape
    //assumes isHexDigit(ch) evals to true
    u16string hexletters = u"abcdefABCDEF";
    int pos = hexletters.find_first_of(ch);
    char16_t out = ch;
    if (pos != std::string::npos && pos >= 6) {
        pos = pos - 6;
        out = hexletters[pos];
    }
    return out;
}

//7.2 White Space
reqinline
bool isWhiteSpace(const char16_t& ch) {
    DEBUGIN("   isWhiteSpace(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || ch==0xFEFF || (ch >= 0x1680 && ch <= 0x3000 
                             && has(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators
reqinline
bool isLineTerminator(const char16_t& ch) {
    DEBUGIN("   isLineTerminator(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}

 // 7.6 Identifier Names and Identifiers
bool intervalarr_contains(unsigned int key, vector< vector< unsigned int > > * arr) {
    //DEBUGIN("   intervalarr_contains ", false);
    vector< unsigned int > *range_starts = &(arr->at(0)),
        *range_ends = &(arr->at(1));
    unsigned int pos = lower_bound(range_starts->begin(), 
                          range_starts->end(), key) - range_starts->begin();
    if (range_starts->at(pos) > key) { 
        if (pos == 0) { return false; } pos--; 
    }
    return (key <= range_ends->at(pos) 
                   || (pos+1 < range_starts->size() 
                       && key == range_starts->at(pos+1)));
}

reqinline
bool isIdentifierStart(const char16_t ch) {
     DEBUGIN("   isIdentifierStart(const char16_t ch)", false);
     DEBUGOUT("", false); 
     return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch == 0x5C) ||                      // \ (backslash)
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                               &nonasciiIdentifierstart));
 } 

reqinline
bool isIdentifierPart(const char16_t ch) {
     DEBUGIN("   isIdentifierPart(const char16_t ch)", false);
     DEBUGOUT("", false); 
     return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch >= 0x30 && ch <= 0x39) ||         // 0..9
         (ch == 0x5C) ||                      // \ (backslash)
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                               &nonasciiIdentifierpart));
 }

// 7.6.1.2 Future Reserved Words
reqinline
bool isFutureReservedWord(const string id) {
     DEBUGIN("   isFutureReservedWord(const u16string id)", false);
     DEBUGOUT("", false); 
     return has(id, { //
             "class",
             "enum",
             "export",
             "extends",
             "import",
             "super"
             });
 }

reqinline
bool isStrictModeReservedWord(const string id) {
     DEBUGIN("   isStrictModeReservedWord(const u16string id)", false);
     DEBUGOUT("", false); 
     return has(id, { 
             "implements",
             "interface",
             "package",
             "private",
             "protected",
             "public",
             "static",
             "yield",
             "let"
             });
 }

reqinline
bool isRestrictedWord(const string id) {
 DEBUGIN("   isRestrictedWord(const u16string id)", false);
 DEBUGOUT("", false);
 return (id == "eval" || id == "arguments");
}

const vector<string> KEYWORDSET = 
    {"if", "in", "do", "var", "for", "new", "try", "let",
     "this", "else", "case", "void", "with", "enum",
     "while", "break", "catch", "throw", 
     "const", "yield", "class", "super",
     "return", "typeof", "delete",
     "switch", "export", "import", "default",
     "finally", "extends", "function", "continue", "debugger",
     "instanceof"};
 
// 7.6.1.1 Keywords
reqinline
bool isKeyword(const string id) {
     DEBUGIN("   isKeyword(const u16string id)", false);
     if (strict && isStrictModeReservedWord(id)) { 
         DEBUGOUT("", false); 
         return true;
     }

     DEBUGOUT("",false);
     // 'const' is specialized as Keyword in V8.
     // 'yield' and 'let' are for compatiblity with SpiderMonkey and ES.next.
     // Some others are from future reserved words.
     return has(id, KEYWORDSET);

}

//information that both Tokenizer and ParseTools use,
//and is shared between them.

struct LinprimaTask {
    u16string sourceStr;
    const char16_t *sourceRaw;
    const int length;

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
    //#throw_end
};

LinprimaTask::LinprimaTask(const u16string sourceStrArg,
                           //const char16_t *sourceRawArg,
                           //const long lengthArg,
                           const OptionsStruct optArg):
    sourceStr(sourceStrArg),
    sourceRaw(sourceStr.data()), length(sourceStr.length()),
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
         isFutureReservedWord(token->strvalue))? Mssg::UnexpectedReserved :
        Mssg::None;
    if (errmsg != Mssg::None) {
        throwError(token, Messages[errmsg], {});
    }
    if (token->type == TknType::Keyword && strict &&
        isStrictModeReservedWord(token->strvalue)) {
        throwErrorTolerant(token, Messages[Mssg::StrictReservedWord], {});
        return;
    }    
    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages[Mssg::UnexpectedToken], {token->strvalue});
    return; //#throw52

}

 // 7.4 Comments

class Tokenizer {
public:
    Tokenizer(u16string code, OptionsStruct options);
    Tokenizer(shared_ptr<LinprimaTask> task);
    static bool isIdentifierName(const TknType type);
    //#throw_begin
    void skipComment();
    ptrTkn scanRegExp();
    ptrTkn collectRegex();
    ptrTkn lex();
    void peek();
    void readTokens(vector<TokenRecord> &tokens);
    //#throw_end
    void tokenize(Document& out, const bool retErrAsJson);
    void filterTokenLocation();
    ptrTkn makeToken();
private:
    shared_ptr<LinprimaTask> task;
    const char16_t * sourceRaw;
    const int length;
    int& idx;
    int& lineNumber;
    int& lineStart;
    ExtraStruct &extra;
    StateStruct &state;
    ptrTkn &lookahead;

    char16_t source(long pos);

    void addComment(const string& type, const string& value, 
                const int start, const int end, const Loc& loc);
    int skipSingleLineComment(int idxtmp, const int offset);
    //#throw_begin
    int skipMultiLineComment(int idxtmp);
    //#throw_end
    char16_t scanHexEscape(const char16_t prefix);
    //#throw_begin
    u16string scanUnicodeCodePointEscape();
    string getEscapedIdentifier();
    string getIdentifier();
    ptrTkn scanIdentifier();
    ptrTkn scanPunctuator();
    ptrTkn scanHexLiteral(const int start);
    ptrTkn scanOctalLiteral(const int start);
    ptrTkn scanNumericLiteral();
    ptrTkn scanStringLiteral();
    RegexHalf scanRegExpBody();
    RegexHalf scanRegExpFlags();
    ptrTkn advanceSlash();
    ptrTkn advance();
    ptrTkn collectToken();
    //#throw_end
};


Tokenizer::Tokenizer(const u16string code,
                     OptionsStruct options) :
    task(make_shared<LinprimaTask>(code,
                                       //code.data(), 
                                       //code.length(), 
                                   options)),
    //    task(make_shared_linprima(code.data(), code.length(), options)),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead) {


}

Tokenizer::Tokenizer(shared_ptr<LinprimaTask> taskArg) :
         task(taskArg),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead) {
    
}

ptrTkn Tokenizer::makeToken() {
    shared_ptr<TokenStruct> tmp (new TokenStruct(
                                                 lineNumber, 
                                                 idx, 
                                                 lineStart));
    return tmp;
}
bool Tokenizer::isIdentifierName(const TknType tkntype) {
    DEBUGIN("   isIdentifierName(TokenStruct token)", false);
    DEBUGOUT("", false); 
    return tkntype == TknType::Identifier ||
        tkntype == TknType::Keyword ||
        tkntype == TknType::BooleanLiteral ||
        tkntype == TknType::NullLiteral;
}
char16_t Tokenizer::source(long pos) { return *(sourceRaw + pos); }

//# only called if extra.commentTracking
void Tokenizer::addComment(const string& type, const string& value, 
                 const int start, const int end, const Loc& loc) {
    DEBUGIN(" addComment(u16string type, u16string value, int start, int end, Loc loc)", false);
    Comment comment(lineNumber, idx, lineStart);

     //assert(typeof start === 'number', 'Comment must have valid position');

     // Because the way the actual token is scanned, often the comments
     // (if any) are skipped twice during the lexical analysis.
     // Thus, we need to skip adding a comment if the comment array already
     // handled it.
     if (state.lastCommentStart >= start) {
         DEBUGOUT("", false); 
         return;
     }
     state.lastCommentStart = start;

     comment.type = type;
     comment.value = value; 
     if (extra.range) {
         comment.range[0] = start;
         comment.range[1] = end;
     }
     if (extra.loc) {
         comment.loc = loc;
     }
     extra.comments.push_back(comment);
     if (extra.attachComment) {
         extra.leadingComments.push_back(comment);
         extra.trailingComments.push_back(comment);
     }
     DEBUGOUT("", false);
}

int Tokenizer::skipSingleLineComment(int idxtmp, const int offset) {
    DEBUGIN(" skipSingleLineComment(const int offset)", false);
    int start;

    char16_t ch;
    string comment;

    Loc loc(lineNumber, idxtmp, lineStart);

    start = idxtmp - offset;
    loc.startColumn -= offset;

    while (idxtmp < length) {
        ch = source(idxtmp);
        ++idxtmp;
        switch (ch) {
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            if (extra.commentTracking) {
                comment = toU8(slice(sourceRaw, start + offset, idxtmp-1));
                loc.endLine = lineNumber;
                loc.endColumn = idxtmp - lineStart - 1;
                addComment("Line", comment, start, idxtmp - 1, loc);
            }
            if (ch == 13 && source(idxtmp) == 10) {
                ++idxtmp;
            }
            ++lineNumber;
            lineStart = idxtmp;
            DEBUGOUT("", false); 
            return idxtmp;
        default:
            break;
        }
    } 

    if (extra.commentTracking) {
        comment = toU8(slice(sourceRaw, start + offset, idxtmp)); 
        loc.endLine = lineNumber;
        loc.endColumn = idxtmp - lineStart;
        addComment("Line", comment, start, idxtmp, loc);
    }
    DEBUGOUT("", false);
    return idxtmp;
}

inline //only called once.
//#throw_
int Tokenizer::skipMultiLineComment(int idxtmp) {
    DEBUGIN(" skipMultiLineComment()", false);
    int start=0; //satisfy g++ warning about potential unitialized use
    Loc loc(lineNumber, idxtmp, lineStart);
    char16_t ch;
    string comment;

    if (extra.commentTracking) {
        start = idxtmp - 2;
        loc.startColumn -= 2 ;
    }

    while (idxtmp < length) {
        ch = source(idxtmp);
        switch (ch) {
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            if (ch == 0x0D && source(idxtmp + 1) == 0x0A) {
                ++idxtmp;
            }
            ++lineNumber;
            ++idxtmp;
            lineStart = idxtmp;
            if (idxtmp >= length) {
                idx = idxtmp;
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            break;
        case 0x2A:
            // Block comment ends with ''.
            if (source(idxtmp + 1) == 0x2F) {
                ++idxtmp;
                ++idxtmp;
                if (extra.commentTracking) {
                    comment = toU8(slice(sourceRaw, start + 2, idxtmp - 2));
                    loc.endLine = lineNumber;
                    loc.endColumn = idxtmp - lineStart;
                    addComment("Block", comment, start, idxtmp, loc);
                }
                DEBUGOUT("", false); 
                idx = idxtmp; //#throw52 error idx.
                return idxtmp;
            }
            ++idxtmp;
            break;
        default:
            ++idxtmp;
            break;
        }
    }

    idx = idxtmp;
    task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    DEBUGOUT("", false);
    return -1; //#throw52
}

//throw_
void Tokenizer::skipComment() {
    DEBUGIN(" skipComment()", false);
    char16_t ch;
    bool start;
    start = (idx == 0);
    while (idx < length) {
        ch = source(idx);

        //alternative to switch we've explored,
        //make an array of all whitespace and other
        //cases below, if not present in array, exit,
        //and move all whitespace cases to the default:
        //clause. We're talking maybe a ms of difference though.
        //both approaches work relatively very well.
        //if (! binary_search(MAYBE_COMMENT, 
        //               MAYBE_COMMENT+29,
        //              ch)) { return; }

        switch(ch) {
        case 0x20: 
        case 0x09: 
        case 0x0B: 
        case 0x0C: 
        case 0xA0: 
        case 0xFEFF: 
        case 0x1680: 
        case 0x180E: 
        case 0x2000: 
        case 0x2001: 
        case 0x2002: 
        case 0x2003: 
        case 0x2004: 
        case 0x2005: 
        case 0x2006: 
        case 0x2007: 
        case 0x2008: 
        case 0x2009: 
        case 0x200A:
        case 0x202F: 
        case 0x205F: 
        case 0x3000: 
            ++idx;
            break;
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            ++idx;
            if (ch == 0x0D && source(idx) == 0x0A) {
                ++idx;
            }
            ++lineNumber;
            lineStart = idx;
            start = true;
            break;
        case 0x2F: // U+002F is '/'
            ch = source(idx + 1);
            if (ch == 0x2F) {
                ++idx;
                ++idx;
                idx = skipSingleLineComment(idx, 2);
                start = true;
            } else if (ch == 0x2A) {  // U+002A is '*'
                ++idx;
                ++idx;
                idx = skipMultiLineComment(idx); 
            } else {
                DEBUGOUT("", false);
                return;
            }
            break;
        case 0x2D: // U+002D is '-'
            // U+003E is '>'
            if (start 
                && (source(idx + 1) == 0x2D) 
                && (source(idx + 2) == 0x3E)) {
                // '-->' is a single-line comment
                idx += 3;
                idx = skipSingleLineComment(idx, 3);
            } else {
                DEBUGOUT("", false);
                return;
            }
            break;
        case 0x3C: // U+003C is '<'
            if (slice(sourceRaw, idx + 1, idx + 4) == u"!--") {
                ++idx; // `<`
                ++idx; // `!`
                ++idx; // `-`
                ++idx; // `-`
                idx = skipSingleLineComment(idx, 4);
            } else {
                DEBUGOUT("", false);
                return; //whitespace
            }    
            break;        
        default:
            DEBUGOUT("", false);
            return;
        }
    }
    DEBUGOUT("", false);
    return; //throw52; 
}
char16_t Tokenizer::scanHexEscape(const char16_t prefix) {
    DEBUGIN("scanHexEscape(const char16_t prefix) {", false);
    int i, len;
    char16_t ch;
    int code = 0;

    len = (prefix == u'u') ? 4 : 2;
    for (i = 0; i < len; ++i) {
        if (idx < length && isHexDigit(source(idx))) {
            ch = source(idx++);
            code = code * 16 + u16string({u"0123456789abcdef"})
                .find_first_of(toLowercaseHex(ch));
        } else {
            DEBUGOUT("scanHexEscape", false); 
            return NULL_CHAR16; 
        }
    }
    DEBUGOUT("scanHexEscape", false); 
    return code;
}

//throw_
u16string Tokenizer::scanUnicodeCodePointEscape() {
    DEBUGIN("scanUnicodeCodePointEscape", false);
    char16_t ch;
    int code;
    char16_t cu[2];

    ch = source(idx);
    code = 0;

    // At least, one hex digit is required.
    if (ch == u'}') {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    while (idx < length) {
        ch = source(idx++);
        if (!isHexDigit(ch)) {
            break;
        }
        code = code * 16 + u16string({u"0123456789abcdef"})
            .find_first_of(toLowercaseHex(ch));
    }

    if (code > 0x10FFFF || ch != u'}') {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    // UTF-16 Encoding
    if (code <= 0xFFFF) {
        DEBUGOUT("", false);
        return u16string({(char16_t) code});
    }

    cu[0] = ((code - 0x10000) >> 10) + 0xD800; 
    cu[1] = ((code - 0x10000) & 1023) + 0xDC00;
    DEBUGOUT("scanUnicodeCodePointEscape", false); 
    return u16string({cu[0], cu[1]});
}

//throw_
string Tokenizer::getEscapedIdentifier() {
    DEBUGIN("getEscapedIdentifier", false);
    char16_t ch;
    u16string id;

    ch = source(idx++);
    id = u16string({ch});

    // '\u' (U+005C, U+0075) denotes an escaped character.
    if (ch == 0x5C) {
        if (source(idx) != 0x75) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierStart(ch)) { 
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
        id = u16string({ch});
    }

    while (idx < length) {
        ch = source(idx);
        if (!isIdentifierPart(ch)) {
            break;
        }
        ++idx;
        appendChar(id, ch);

        // '\u' (U+005C, U+0075) denotes an escaped character.
        if (ch == 0x5C) {
            id = id.substr(0, id.length() - 1);
            if (source(idx) != 0x75) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierPart(ch)) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            appendChar(id, ch);
        }
    }

    DEBUGOUT("getEscapedIdentifier", false); 
    return toU8(id);
}

//throw_
string Tokenizer::getIdentifier() {
    DEBUGIN("getIdentifier()", false);
    int start;
    char16_t ch;

    start = idx++;
    while (idx < length) {
        ch = source(idx);
        if (ch == 0x5C) {
            // Blackslash (U+005C) marks Unicode escape sequence.
            idx = start;
            return DBGRET("", getEscapedIdentifier());
        }
        // this is a hotpath (e.g. about 10th-15th in # calls)
        // force-inlining isIdentifierPart here and 
        // removing the 2x jumps per loop
        // saves about 95% of its cost.
        if ((ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch >= 0x30 && ch <= 0x39) ||         // 0..9
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                    &nonasciiIdentifierpart))) {
            ++idx;
        } else {
            break;
        }
    }

    return DBGRET("getIdentifier", toU8(slice(sourceRaw, start, idx))); 
}

//throw_
ptrTkn Tokenizer::scanIdentifier() {
    DEBUGIN(" scanIdentifier()", false);
    ptrTkn t = makeToken();
    int start;
    TknType type;
    string id; 

    start = idx;

    // Backslash (U+005C) starts an escaped character.
    //#ternary operator interferes with throw52
    if (source(idx) == 0x5C) {
        id = getEscapedIdentifier();
    } else {
        id = getIdentifier();
    }
    // There is no keyword or literal with only one character.
    // Thus, it must be an identifier.
    if (id.length() == 1) {
        type = TknType::Identifier;
    } else if (isKeyword(id)) {
        type = TknType::Keyword;
    } else if (id == "null") {
        type = TknType::NullLiteral;
    } else if (id == "true" || id == "false") {
        type = TknType::BooleanLiteral;
    } else {
        type = TknType::Identifier;
    }

    t->type = type;
    t->strvalue = id;
    t->literaltype = LiteralType["String"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanIdentifier", false);
    return t;
}

u16string emccu16str;


 // 7.7 Punctuators
 //throw_
ptrTkn Tokenizer::scanPunctuator() {
    DEBUGIN(" scanPunctuator()", false);

    ptrTkn t = makeToken();
    int start = idx;

    char16_t ch1 = source(idx);
    string ch2, ch3, ch4;
    char16_t code[2];

    code[0] = source(idx);

    t->type = TknType::Punctuator;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->literaltype = LiteralType["String"];

    switch (code[0]) {
        // Check for most common single-character punctuators.
    case 0x2E:  // . dot
    case 0x28:  // ( open bracket
    case 0x29:  // ) close bracket
    case 0x3B:  // ; semicolon
    case 0x2C:  // , comma
    case 0x7B:  // { open curly brace
    case 0x7D:  // } close curly brace
    case 0x5B:  // [
    case 0x5D:  // ]
    case 0x3A:  // :
    case 0x3F:  // ?
    case 0x7E:  // ~
        ++idx;
        if (extra.tokenize) {
            if (code[0] == 0x28) {
                extra.openParenToken = extra.tokenRecords.size();
            } else if (code[0] == 0x7B) {
                extra.openCurlyToken = extra.tokenRecords.size();
            }
        }
        t->strvalue = toU8(u16string({ code[0] }));

        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    default:
        code[1] = source(idx + 1);

        // '=' (U+003D) marks an assignment or comparison operator.
        if (code[1] == 0x3D) {
            switch (code[0]) {
            case 0x2B:  // +
            case 0x2D:  // -
            case 0x2F:  // /
            case 0x3C:  // <
            case 0x3E:  // >
            case 0x5E:  // ^
            case 0x7C:  // |
            case 0x25:  // %
            case 0x26:  // &
            case 0x2A:  // *
                idx += 2;
                t->strvalue = toU8(u16string({code[0], code[1]}));      
                t->end = idx;
                DEBUGOUT("", false); 
                return t;
            case 0x21: // !
            case 0x3D: // =
                idx += 2;

                // !== and ===
                if (source(idx) == 0x3D) {
                    ++idx;
                }
                t->strvalue = toU8(slice(sourceRaw, start, idx));
                t->end = idx;
                DEBUGOUT("", false); 
                return t;
            }
        }
    } 


    // 4-character punctuator: >>>=

    ch4 = toU8(slice(sourceRaw, idx, idx+4)); 

    if (ch4 == ">>>=") {
        idx += 4;
        t->strvalue = ch4;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 3-character punctuators: === !== >>> <<= >>=

    ch3 = ch4.substr(0, 3); 

    if (ch3 == ">>>" || ch3 == "<<=" || ch3 == ">>=") {
        idx += 3;
        t->strvalue = ch3;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // Other 2-character punctuators: ++ -- << >> && ||
    ch2 = ch3.substr(0, 2); 

    if (((ch1 == (char16_t) ch2[1]) 
         && (u16string({u"+-<>&|"}).find_first_of(ch1)
             != std::string::npos))
        || ch2 == "=>") {
        idx += 2;
        t->strvalue = ch2;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 1-character punctuators: < > = ! + - * % & | ^ /

    if (u16string({u"<>=!+-*%&|^/"}) //? is it necessary/correct to have this as ({u""}) and not just (u"")
        .find_first_of(ch1) != std::string::npos) {
        ++idx;
        t->strvalue = toU8(u16string({ch1}));
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});

    DEBUGOUT("", false); 
    return t;
  //# return avoids compile warnings bcos clang doesn't look into task->throwError.
}
     // 7.8.3 Numeric Literals
//throw_
ptrTkn Tokenizer::scanHexLiteral(const int start) {
    DEBUGIN(" scanHexLiteral(const int start)", false);
    u16string number;
    ptrTkn t = makeToken();

    while (idx < length) {
        if (!isHexDigit(source(idx))) {
            break;
        }
        appendChar(number, source(idx++));
    }

    if (number.length() == 0) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    if (isIdentifierStart(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::NumericLiteral;
    t->intvalue = parseInt(number, 16);
    t->literaltype = LiteralType["Int"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanHexLiteral", false);
    return t;
}

//throw_
ptrTkn Tokenizer::scanOctalLiteral(const int start) {
    DEBUGIN(" scanOctalLiteral(const int start)", false);
    u16string number = u"0";

    ptrTkn t = makeToken();
    appendChar(number, source(idx++));

    while (idx < length) {
        if (!isOctalDigit(source(idx))) {
            break;
        }
        appendChar(number, source(idx++));
    }

    if (isIdentifierStart(source(idx)) || isDecimalDigit(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::NumericLiteral;
    t->intvalue = parseInt(number, 8);
    t->literaltype = LiteralType["Int"];
    t->octal = true;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanOctalLiteral", false);
    return t;
}

//#throw_
ptrTkn Tokenizer::scanNumericLiteral() {
    DEBUGIN(" scanNumericLiteral()", false);
    int start;
    char16_t ch;
    ptrTkn t = makeToken();
    u16string number;


    ch = source(idx);
    softAssert(isDecimalDigit(ch) || (ch == u'.'),
               "Numeric literal must start with a decimal digit or a decimal point");

    start = idx;
    number = u"";
    if (ch != u'.') {
        appendChar(number, source(idx++));
        ch = source(idx);

        // Hex number starts with '0x'.
        // Octal number starts with '0'.
        if (number == u"0") {
            if (ch == u'x' || ch == u'X') {
                ++idx;
                return DBGRET("scanNumericLiteral", scanHexLiteral(start));
            }
            if (isOctalDigit(ch)) {
                return DBGRET("scanNumericLiteral", scanOctalLiteral(start));
            }
            // decimal number starts with '0' such as '09' is illegal.
            if (ch && isDecimalDigit(ch)) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"}); 
            }
        }

        while (isDecimalDigit(source(idx))) {
            appendChar(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'.') {
        //#JSON can't parse decimal numbers without
        //#a number preceding the decimal.
        if (start == idx) { appendChar(number, u'0'); }

        appendChar(number, source(idx++));
        while (isDecimalDigit(source(idx))) {
            //if (source(idx) != u'0') { hasDot = true; } //# js auto-casts dbls of negligible epsilon-to-int to int
            appendChar(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'e' || ch == u'E') {
        appendChar(number, source(idx++));
        ch = source(idx);
        if (ch == u'+' || ch == u'-') {
            appendChar(number, source(idx++));
        }
        if (isDecimalDigit(source(idx))) {
            while (isDecimalDigit(source(idx))) {
                appendChar(number, source(idx++));
            }
        } else {
            task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
    }

    if (isIdentifierStart(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }
    t->type = TknType::NumericLiteral;
    t->strvalue = toU8(number);
    //t->dblvalue = //# want to allow browsers to use full allowance of whatever their local max int size is.
    //               //#which is same as it would work in esprima.
    //(hasSciNote)?
    //sciNoteToDouble(toU8string(number)) : stod(toU8string(number));
    t->literaltype = LiteralType["Double"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;

    DEBUGOUT("scanNumericLiteral", false);
    return t; 
}


// 7.8.4 String Literals
//throw_
ptrTkn Tokenizer::scanStringLiteral() {
    DEBUGIN(" scanStringLiteral()", false);

    ptrTkn t = makeToken();
    bool octal = false;
    u16string str;
    int start, restore, code;
    char16_t quote, ch, unescaped;
    int startLineNumber = lineNumber;
    int startLineStart = lineStart;

    quote = source(idx);
    softAssert((quote == u'\'' || quote == u'"'),
               "String literal must starts with a quote");

    start = idx;
    ++idx;

    while (idx < length) {
        ch = source(idx++);
        if (ch == quote) {
            quote = NULL_CHAR16;
            break;
        } else if (ch == u'\\') {
            ch = source(idx++);
            if (!ch || !isLineTerminator(ch)) { // what does !ch mean in this context
                if (ch == u'u' || ch == u'x') {
                    if (source(idx) == u'{') {
                        ++idx;
#ifndef THROWABLE
                        u16string tmp = scanUnicodeCodePointEscape();
                        str.append(tmp);
#endif
#ifdef THROWABLE
                        str.append(scanUnicodeCodePointEscape());
#endif
                    } else {
                        restore = idx;
                        unescaped = scanHexEscape(ch);
                        if (unescaped != NULL_CHAR16) {
                            appendChar(str, unescaped);
                        } else {
                            idx = restore;
                            appendChar(str, ch);
                        }
                    }    
                } else if (ch == u'n') {
                    appendChar(str, u'\n');
                } else if (ch == u'r') {
                    appendChar(str, u'\r');
                } else if (ch == u't') {
                    appendChar(str, u'\t');
                } else if (ch == u'b') {
                    appendChar(str, u'\b');
                } else if (ch == u'f') {
                    appendChar(str, u'\f');
                } else if (ch == u'v') {
                    appendChar(str, u'\x0B');
                } else {
                    if (isOctalDigit(ch)) {
                        code = u16string({u"01234567"}).find_first_of(ch);

                        // \0 is not octal escape sequence
                        if (code != 0) {
                            octal = true;
                        }

                        if (idx < length && isOctalDigit(source(idx))) {
                            octal = true;
                            code = code * 8 + 
                                u16string({u"01234567"})
                                .find_first_of(source(idx++));

                            // 3 digits are only allowed when string starts
                            // with 0, 1, 2, 3
                            if (u16string({u"0123"})
                                .find_first_of(ch) != std::string::npos &&
                                idx < length &&
                                isOctalDigit(source(idx))) {
                                code = code * 8 + u16string({u"01234567"})
                                    .find_first_of(source(idx++));
                            }
                        }
                        appendChar(str, (char16_t) code);
                    } else {
                        str += ch;
                    }
                }
            } else {
                ++lineNumber;
                if (ch == u'\r' && source(idx) == u'\n') {
                    ++idx;
                }
                lineStart = idx;
            }
        } else if (isLineTerminator(ch)) {
            break;
        } else {
            appendChar(str, ch);
        }
    }

    if (quote != NULL_CHAR16) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::StringLiteral;
    t->strvalue = toU8(str);
    t->literaltype = LiteralType["String"];            
    t->octal = octal;
    t->startLineNumber = startLineNumber;
    t->startLineStart = startLineStart;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart,
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanStringLiteral", false);
    return t;
}

//throw_
RegexHalf Tokenizer::scanRegExpBody() {
    DEBUGIN("scanRegExpBody()", false);
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexHalf rh;

    ch = source(idx);
    softAssert(ch == u'/',
               "Regular expression literal must start with a slash");
    appendChar(str, source(idx++)); 

    classMarker = false;
    terminated = false;
    while (idx < length) {
        ch = source(idx++);
        appendChar(str, ch);
        if (ch == u'\\') {
            ch = source(idx++);
            // ECMA-262 7.8.5
            if (isLineTerminator(ch)) {
                task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
            }
            appendChar(str, ch);
        } else if (isLineTerminator(ch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
        } else if (classMarker) {
            if (ch == u']') {
                classMarker = false;
            }
        } else {
            if (ch == u'/') {
                terminated = true;
                break;
            } else if (ch == u'[') {
                classMarker = true;
            }
        }
    }

    if (!terminated) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length() - 2);
    rh.value = toU8(body);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpBody", false);
    return rh;
}

//throw_
RegexHalf Tokenizer::scanRegExpFlags() {
    DEBUGIN("scanRegExpFlags()", false);
    char16_t ch;
    u16string str, flags;
    int restore;
    RegexHalf rh;

    str = u"";
    flags = u"";
    while (idx < length) {
        ch = source(idx);
        if (!isIdentifierPart(ch)) {
            break;
        }

        ++idx;
        if (ch == u'\\' && idx < length) {
            ch = source(idx);
            if (ch == u'u') {
                ++idx;
                restore = idx;
                ch = scanHexEscape(u'u');
                if (ch) {
                    appendChar(flags, ch);
                    for (str.append(u16string({u'\\', u'u'})); 
                         restore < idx; ++restore) {                         
                        appendChar(str, source(restore));
                    }
                } else {
                    idx = restore;
                    appendChar(flags, u'u');
                    str.append(u"\\u");
                }

                task->throwErrorTolerant(NULLPTRTKN, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {"ILLEGAL"});
            } else {
                appendChar(str, u'\\');
                task->throwErrorTolerant(NULLPTRTKN, 
                                   Messages[Mssg::UnexpectedToken],
                                   {"ILLEGAL"});
            }
        } else {
            appendChar(flags, ch);
            appendChar(str, ch);
        }
    }

    rh.value = toU8(flags);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpFlags", false);
    return rh;
}

//throw_
ptrTkn Tokenizer::scanRegExp() {
    DEBUGIN(" scanRegExp()", false);
    int start;
    RegexHalf body; 
    RegexHalf flags; 
    ptrTkn t = makeToken();

    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = makeToken();
    lookahead->isNull = true;
    skipComment(); 
    start = idx;

    body = scanRegExpBody(); //eregexhalf
    flags = scanRegExpFlags(); //eregexhalf

    //value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t->type = TknType::RegularExpression;
        t->strvalue = "regexDummy"; //?
        t->lineNumber = lineNumber;
        t->lineStart = lineStart;
        t->start = start;
        t->end = idx;
        DEBUGOUT("", false); 
        return t; //not polymorphic right now. not going to work... :!
    }

    t->literal = body.literal; 
    t->literal.append(flags.literal);
    t->literaltype = LiteralType["Regexp"];
    t->strvalue = body.value;
    t->flags = flags.value;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanRegExp", false);
    return t;
}

//throw_
ptrTkn Tokenizer::collectRegex() {
    DEBUGIN(" collectRegex()", false);
    int pos;

    ptrTkn regex;
    string tokval;

    skipComment();
    Loc loc(lineNumber, idx, lineStart);

    pos = idx;

    regex = scanRegExp(); //etkns
    loc.endLine = lineNumber;
    loc.endColumn = idx - lineStart;

    if (!extra.tokenize) {
        TokenRecord tr(loc);
        // Pop the previous token, which is likely '/' or '/='
        if (extra.tokenRecords.size() > 0) {
            const TokenRecord * token = 
                &(extra.tokenRecords[extra.tokenRecords.size() - 1]);
            if (token->range[0] == pos 
                && token->type == TknType::Punctuator) {
                
                tokval = token->valuestring; 
                if (tokval == "/" || tokval == "/=") {
                    extra.tokenRecords.pop_back();
                }
            }
        }

        tr.type = TknType::RegularExpression;
        tr.valuestring = regex->literal;
        tr.range[0] = pos;
        tr.range[1] = idx;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectRegex", false);
    return regex;
}

//throw_
ptrTkn Tokenizer::advanceSlash() {
    DEBUGIN(" advanceSlash()", false);
    //# only gets called if extra.tokenize == true

    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    if (extra.tokenRecords.size() == 0) {
        // Nothing before that: it cannot be a division.
        return DBGRET("advSlash1", collectRegex()); 
    }    
    const TokenRecord *prevToken = 
        &(extra.tokenRecords[extra.tokenRecords.size() - 1]);

    if (prevToken->type == TknType::Punctuator) { 
        if (prevToken->valuestring == "]") { 
            return DBGRET("advSlash2", scanPunctuator());
        }
        const TokenRecord *checkToken;
        if (prevToken->valuestring == ")") {
            //checkToken && 
            //# instead of checking for existence, we add
            //# the openParenToken value check to the condition above.
            //# remember exta.tokens() is already size > 0 bcos 
            //# check at top of func.

            if (extra.openParenToken > 0
                && extra.tokenRecords.size() > (extra.openParenToken - 1)) { 
                checkToken = 
                    &(extra.tokenRecords[extra.openParenToken - 1]);
                if (checkToken->type == TknType::Keyword && 
                    has(checkToken->valuestring, 
                        {"if", "while", "for", "with"})) {
                    return DBGRET("advSlash3", collectRegex()); 
                }
            }
            return DBGRET("advSlash4", scanPunctuator());
        }
        if (prevToken->valuestring == "}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokenRecords.size() > (extra.openCurlyToken -3) &&
                extra.tokenRecords[extra.openCurlyToken - 3].type 
                == TknType::Keyword) { 
                // Anonymous function.

                if (extra.openCurlyToken > 3
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 4)) {
                    checkToken = 
                        &(extra.tokenRecords[extra.openCurlyToken -4]);
                } else { 
                    return DBGRET("advSlash5", scanPunctuator());
                }
            } else if (extra.openCurlyToken >= 4 
                       && extra.tokenRecords.size()
                       > (extra.openCurlyToken -4) 
                       && extra.tokenRecords[extra.openCurlyToken - 4].type
                       == TknType::Keyword) {
                // again previously had checked type against string in this cond.
                // Named function.
                if (extra.openCurlyToken > 4
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 5)) {
                    checkToken = 
                        &(extra.tokenRecords[extra.openCurlyToken -5]);
                } else { 
                    return DBGRET("advSlash6", collectRegex());
                }
            } else {
                return DBGRET("advSlash7", scanPunctuator());
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (has(checkToken->valuestring, FnExprTokens)) {
                // It is an expression.
                return DBGRET("advSlash8", scanPunctuator());
            }
            // It is a declaration.
            return DBGRET("advSlash9", collectRegex()); 
        }
        return DBGRET("advSlash10", collectRegex());
    }
    if (prevToken->type == TknType::Keyword) { 
        return DBGRET("advSlash11", collectRegex()); 
    }
    return DBGRET("advSlash12", scanPunctuator());
}

//throw_
ptrTkn Tokenizer::advance() {
    DEBUGIN(" advance()", false);
    char16_t ch;

    skipComment(); 

    if (idx >= length) {
        ptrTkn t = makeToken();
        t->type = TknType::EOFF;
        t->lineNumber = lineNumber;
        t->lineStart = lineStart;
        t->start = idx;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    ch = source(idx);
    if (isIdentifierStart(ch)) {
        return DBGRET("adv", scanIdentifier());
    }

    // Very common: ( and ) and ;
    if (ch == 0x28 || ch == 0x29 || ch == 0x3B) {
        return DBGRET("adv", scanPunctuator());
    }

    // String literal starts with single quote (U+0027) or double quote (U+0022).
    if (ch == 0x27 || ch == 0x22) {
        return DBGRET("adv", scanStringLiteral());
    }

    // Dot (.) U+002E can also start a floating-point number, hence the need
    // to check the next character.
    if (ch == 0x2E) {
        if (isDecimalDigit(source(idx + 1))) {
            return DBGRET("adv", scanNumericLiteral());
        }
        return DBGRET("adv", scanPunctuator());
    }

    if (isDecimalDigit(ch)) {
        return DBGRET("adv", scanNumericLiteral());
    }

    // Slash (/) U+002F can also start a regex.
    if (extra.tokenize && ch == 0x2F) {
        return DBGRET("adv", advanceSlash());
    }

    return DBGRET("adv", scanPunctuator());
}

//throw_
ptrTkn Tokenizer::collectToken() {
    DEBUGIN(" collectToken()", false);
    ptrTkn token;
    u16string value;

    skipComment(); //ev
    Loc loc(lineNumber, idx, lineStart);

    token = advance(); //etkns
    loc.endLine = lineNumber;
    loc.endColumn = idx - lineStart;

    if (token->type != TknType::EOFF) { 
        //this didn't check against string. is fine.
        TokenRecord tr(loc);
        tr.valuestring = toU8(slice(sourceRaw, 
                                    token->start, 
                                    token->end));
        tr.type = token->type;
        tr.range[0] = token->start;
        tr.range[1] = token->end;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectToken", false);
    return token;
}

//throw_
ptrTkn Tokenizer::lex() {
    DEBUGIN(" lex()", false);
    ptrTkn token;

    token = lookahead;
    idx = token->end;
    lineNumber = token->lineNumber;
    lineStart = token->lineStart;

    //#ternary operators not supported by throw52
    if (extra.tokenTracking) {
        lookahead = collectToken();
    } else { 
        lookahead = advance(); 
    }

    idx = token->end;
    lineNumber = token->lineNumber;
    lineStart = token->lineStart;

    DEBUGOUT("lex", false);
    return token;
}

//throw_
void Tokenizer::peek() {
    DEBUGIN(" peek()", false);
    int pos, line, start;
    pos = idx;
    line = lineNumber;
    start = lineStart;

    //#ternary operators not supported by throw52
    if (extra.tokenTracking) {
        lookahead = collectToken();
    } else { 
        lookahead = advance(); 
    }

    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("peek", false);
    return; //#throw52
}

 //# Position and SourceLocation are defined as structs near the top.



 
 //Loc 


//#this is the ONLY constructor in this code capable of 
//#modifying state, it ALWAYS and ONLY changes state if lookaheadAvail
//#is true. Important to keep in mind when making 
//#1:1 updates.
Node::Node(bool lookaheadAvail, 
           bool exists, 
           vector<Node*>* heapNodes,
           AllocatorType* allocArg,
           LinprimaTask * taskArg):
    loc(-1,-1,-1), alloc(allocArg), task(taskArg) {
 
    DEBUGIN("Node::Node(bool, bool)", true);
#ifndef THROWABLE
    err = false;
#endif
    hasJv = false;
    hasRange = false;
    hasLoc = false;
    if (lookaheadAvail) {
        //#not true for nullnode or parseProgram's node.
        lookavailInit();
    } 
    if (lookaheadAvail || exists) {
        this->heapNodes = heapNodes;
        heapNodes->push_back(this);
        if (task->extra.range) {
            hasRange = true;
            range[0] = task->idx;
            range[1] = 0;
        }

        if (task->extra.loc) {
            hasLoc = true;
            loc.startLine = task->lineNumber;
            loc.startColumn = task->idx - task->lineStart;
        } 
    }
    DEBUGOUT("", true);
}

string Node::s(const u16string in) { return toU8(in); }

void Node::lookavailInit() {
    hasJv = true;
    jv.SetObject();

     task->idx = task->lookahead->start;
     if (task->lookahead->type == TknType::StringLiteral) {
         task->lineNumber = task->lookahead->startLineNumber;
         task->lineStart = task->lookahead->startLineStart;
     } else {
         task->lineNumber = task->lookahead->lineNumber;
         task->lineStart = task->lookahead->lineStart;
     }
     if (hasRange) { //#should always be true, but keep it open while testing.
         loc.startLine = task->lineNumber;
         loc.startColumn = task->idx - task->lineStart;
         range[0] = task->idx;
     }
 }

 void Node::clear() {
     regexPaths.clear();
     expressions.clear();

     hasLoc = false;
     hasRange = false;
 }

 void Node::unused() {
     delNode(this);
 }

reqinline
void Node::jvput(const char* path, const string b)  {
    jv.AddMember(StringRef(path), 
              Value(b.data(), b.length(), *alloc).Move(), 
              *alloc); 
}

reqinline
void Node::jvput(const char* path, const int b) 
{jv.AddMember(StringRef(path), b, *alloc); }

reqinline
void Node::jvput(const char* path, const bool b) 
{jv.AddMember(StringRef(path), b, *alloc); }

reqinline
void Node::jvput_dbl(const char* path, const double b) 
{jv.AddMember(StringRef(path), b, *alloc); }

reqinline
void Node::jvput_null(const char* path)
{ Value tmp; jv.AddMember(StringRef(path), tmp, *alloc); }
 

//# different name to prevent easy bug of forgetting the string.
//# root path, should be first in vector, then path within it, etc.
void Node::regNoadd(const vector<string> paths, Node * child) { 
    string debugmsg = " Node::regNoadd(vector<string> paths, Node &child) :::";
    debugmsg.append(paths[0]);
    //DEBUGIN(debugmsg, false);
    if (child == NULLNODE) { return; }

    if (child->hasRange) {
        Value rangearr(kArrayType);
        //bool m = find(heapNodes.begin(), haystack.end(), needle) != haystack.end();
        //int n = (m)? -1:0;
        //        rangearr.PushBack(n, *alloc);
        rangearr.PushBack(child->range[0], *alloc);
        rangearr.PushBack(child->range[1], *alloc);
        child->jv.AddMember("range", rangearr, *alloc);
    } 
    if (child->hasLoc) {        
        Value locjson(kObjectType);
        child->loc.toJson(locjson, alloc);
        child->jv.AddMember("loc", locjson, *alloc);
    }
    if (child->regexPaths.size() > 0) {
        if (child->regexPaths[0][0] == ".") {
            vector<string> reverse;
            for (int i=paths.size()-1; i >= 0; i--) {
                reverse.push_back(paths[i]);
            }
            regexPaths.push_back(reverse);
        } else {
            for (unsigned int i=0; i<child->regexPaths.size(); i++) {
                regexPaths.push_back(child->regexPaths[i]);
                for (int j=paths.size()-1; j>=0; j--) {
                    regexPaths.back().push_back(paths[j]);
                }
            }
        }
    }
    //DEBUGOUT("Node::regNoAdd", false);
}

void Node::reg(const string path, Node * child) { 
    //DEBUGIN("reg(string path, Node &child)", false);
    
    //addition of sequence expression's children is done lazily
    //so that they are available for reinterpretAsCoverList
    //to reinterpret them as children of a different node
    //and then cleanly delete sequenceexpression node
    //without having to extricate children objects from json, etc.
    if (child != NULLNODE) {
        if (child->type == Syntax[Synt::SequenceExpression]) {
            child->nodeVec("expressions", child->expressions);
        } else if (child->type 
                   == Syntax[Synt::AssignmentExpression]) {
            child->reg("left", child->leftAssign);
            child->reg("right", child->rightAssign);
        }

        regNoadd({path}, child);
    
        jv.AddMember(Value(path.data(),path.length(), *alloc).Move(), 
                      child->jv.Move(), 
                     *alloc);
    
        if (child->thisnc) {
            child->thisnc->nodesJv = &(jv[path.data()]);
        }
        delNode (child); 
    } else {
        Value tmp;
        jv.AddMember(Value(path.data(),path.length(), *alloc).Move(), 
                      tmp, *alloc);
    }
    //DEBUGOUT("node::reg", false);
}

void Node::nodeVec(const string path, const vector< Node* > & nodes) { 
    //DEBUGIN("nodeVec(string path, vector< Node > & nodes)", false);
    Value arr(kArrayType);
    for (unsigned int i=0; i<nodes.size(); i++) {
        if (nodes[i] != NULLNODE) {
            if (nodes[i]->type == Syntax[Synt::SequenceExpression]) {
                nodes[i]->nodeVec("expressions", nodes[i]->expressions);
            } else if (nodes[i]->type == Syntax[Synt::AssignmentExpression]) {
                nodes[i]->reg("left", nodes[i]->leftAssign);
                nodes[i]->reg("right", nodes[i]->rightAssign);
            }
            regNoadd({path, to_string(i)}, nodes[i]);
            arr.PushBack(nodes[i]->jv.Move(), *alloc);
            if (nodes[i]->thisnc) {
                nodes[i]->thisnc->nodesJv = &(arr[i]);
            }
            delNode (nodes[i]);
        } else {
            Value tmp;
            arr.PushBack(tmp, *alloc);
        }
    } 
    jv.AddMember(Value(path.data(),path.length(), *alloc).Move(), 
                 arr, *alloc);
    //DEBUGOUT("node::nodeVec", false);
}
inline
void Node::addType(const Synt in) { 
    //DEBUGIN("addType", false);
    type = Syntax[in];
    
    jv.AddMember("type",
                 Value(type.data(), type.length(), *alloc).Move(),
                  *alloc);
}
void Node::regexPaths2json(Value& out) { 
    //DEBUGIN("Node::regexPaths2json()", false);
    out.SetArray();    
    Value path;
    for (unsigned int i=0; i<regexPaths.size(); i++) {
        path.SetArray();
        for (int j=regexPaths[i].size()-1; j>=0; j--) {
            string step;
            path.PushBack(Value(regexPaths[i][j].data(), 
                                regexPaths[i][j].length(), 
                                *alloc).Move(),
                          *alloc);            
        } 
        out.PushBack(path, *alloc);
    }
    //DEBUGOUT("", false);     
}


//#not the most efficient way to do this. would be easy to choke
//#on big comments. TODO move NodesComments to heap, free
//on removal from bottomright and end of use.
void Node::processComment() {
    DEBUGIN("processComment()", false);
    //# assumes attachComments 
    //# so that means range is already true.

    ExtraStruct &extra = task->extra;
    vector< Comment > trailingComments;

    vector< shared_ptr<NodesComments> > * bottomRight = 
        &(extra.bottomRightStack);
    shared_ptr<NodesComments> lastChild;
    shared_ptr<NodesComments> last;
    thisnc.reset(new NodesComments(jv, alloc));
    bool LEADING = true, TRAILING= false;
    if (bottomRight->size() > 0) {
        last = bottomRight->back();
    }

    if (type == Syntax[Synt::Program]) {  
        if (jv["body"].Size() > 0) {
            DEBUGOUT("", false); 
            return;
        }
    }

    thisnc->range[0] = range[0];
    thisnc->range[1] = range[1];

    if (extra.trailingComments.size() > 0) {
        if (extra.trailingComments[0].range[0] >= thisnc->range[1]) {
            trailingComments = extra.trailingComments;
            extra.trailingComments.clear();
        } else {
            extra.trailingComments.clear(); 
            //# originally first clause had =[] and this has .length = 0
            //# don't think there's an effective difference thoug
        }
    } else {
        if (last && 
            last->trailingComments.size() > 0 && 
            last->trailingComments[0].range[0] >= thisnc->range[1]) {
            trailingComments = last->trailingComments;
            last->trailingComments.clear();
            last->commentsIntoJson(&(task->extra), TRAILING);
            //delete last->trailingComments; 
            //? maybe have a boolean to say no trailing comments? length will prob. be workable.
        }
    }

    // Eating the stack.
    if (last) {
        while ((last) && last->range[0] >= thisnc->range[0]) {
            lastChild = last;
            if (bottomRight->size() > 0) { 
                last = bottomRight->back(); 
                bottomRight->pop_back();
            } else { 
                last.reset();
            }
        }
    }

    if (lastChild) {
        if (lastChild->leadingComments.size() > 0 &&
            lastChild->leadingComments.back()
            .range[1] <= thisnc->range[0]) {
            thisnc->leadingComments = lastChild->leadingComments;
            lastChild->leadingComments.clear();
            lastChild->commentsIntoJson(&(task->extra), LEADING);
            thisnc->commentsIntoJson(&(task->extra), LEADING);
        }
    } else if (extra.leadingComments.size() > 0 && 
               extra.leadingComments[extra.leadingComments.size() - 1]
               .range[1] <= thisnc->range[0]) {
        thisnc->leadingComments = extra.leadingComments;
        extra.leadingComments.clear();
        thisnc->commentsIntoJson(&(task->extra), LEADING);
    }

    if (trailingComments.size() > 0) {
        thisnc->trailingComments = trailingComments;
        thisnc->commentsIntoJson(&(task->extra), TRAILING);
    }

    bottomRight->push_back(thisnc);
    DEBUGOUT("", false);
}


void Node::finish() {
    DEBUGIN("finish()", false);
    if (task->extra.range) {
        this->range[1] = task->idx; 
    }
    if (task->extra.loc) {        
        loc.endLine = task->lineNumber;
        loc.endColumn = task->idx - task->lineStart;
        if (task->extra.hasSource) {
            loc.source = task->extra.source; 
            loc.hasSource = true;
        }
    }

    if (task->extra.attachComment) {
        this->processComment();
    }
    DEBUGOUT("node::finish", false);
}


void Node::finishArrayExpression(const vector< Node* >& elements) {
    DEBUGIN("finishArrayExpression(vector< Node >& elements)", false);
    addType(Synt::ArrayExpression);
    nodeVec("elements", elements);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishArrowFunctionExpression(const vector< Node* >& params,
                                         const vector< Node* >& defaults, 
                                         Node * body, 
                                         const bool expression) {
    DEBUGIN("finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression)", false);
    addType(Synt::ArrowFunctionExpression);

    jvput_null("id");
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", expression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishAssignmentExpression(const string oper, 
                                      Node * left, 
                                      Node * right) {
    DEBUGIN("finishAssignmentExpression(u16string oper, Node& left, Node& right)", false);

    addType(Synt::AssignmentExpression);
    jvput("operator", oper);

    leftAssign = left;
    rightAssign = right;

    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBinaryExpression(const string oper, 
                                  Node * left, 
                                  Node * right) {
    DEBUGIN("finishBinaryExpression(u16string oper, Node& left, Node& right)", false);
    addType((oper == "||" || oper == "&&") ? 
            Synt::LogicalExpression : Synt::BinaryExpression);
    jvput("operator", oper);
    reg("left", left); 
    reg("right", right);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBlockStatement(const vector< Node* >& body) {
    DEBUGIN("finishBlockStatement(vector< Node >& body)", false);
    addType(Synt::BlockStatement);
    nodeVec("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBreakStatement(Node * label) {
    DEBUGIN("finishBreakStatement(Node& label)", false);
    addType(Synt::BreakStatement);
    reg("label", label);
    this->finish();  
    DEBUGOUT("", false);
}


void Node::finishCallExpression(Node * callee, 
                                const vector< Node* >& args) {
    DEBUGIN("finishCallExpression", false);
    addType(Synt::CallExpression);
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish(); 
    DEBUGOUT("", false);
}


void Node::finishCatchClause(Node * param, 
                             Node * body) {
    DEBUGIN("finishCatchClause", false);
    addType(Synt::CatchClause);
    reg("param", param);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishConditionalExpression(Node * test, 
                                       Node * consequent,
                                       Node * alternate) {
    DEBUGIN("finishConditionalExpression", false);
    addType(Synt::ConditionalExpression);
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishContinueStatement(Node * label) {
    DEBUGIN("finishContinueStatement", false);
    addType(Synt::ContinueStatement);
    reg("label", label);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishDebuggerStatement() {
    DEBUGIN("finishDebuggerStatement", false);
    addType(Synt::DebuggerStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishDoWhileStatement(Node * body, Node * test) {
    DEBUGIN("finishDoWhileStatement", false);
    addType(Synt::DoWhileStatement);
    reg("body", body);
    reg("test", test);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishEmptyStatement() {
    DEBUGIN("finishEmptyStatement()", false);
    addType(Synt::EmptyStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishExpressionStatement(Node * expression) {
    DEBUGIN("finishExpressionStatement", false);
    addType(Synt::ExpressionStatement);
    reg("expression", expression);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishForStatement(Node * init, 
                              Node * test, 
                              Node * update, 
                              Node * body) {
    DEBUGIN("finishForStatement", false);
    addType(Synt::ForStatement);
    reg("init", init);
    reg("test", test);
    reg("update", update);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishForInStatement(Node * left, 
                                Node * right, 
                                Node * body) {
    DEBUGIN("finishForInStatement", false);
    addType(Synt::ForInStatement);
    reg("left", left);
    reg("right", right);
    reg("body", body);
    jvput("each", false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionDeclaration(Node * id, 
                                     const vector< Node* >& params, 
                                     const vector< Node* >& defaults, 
                                     Node * body) {
    DEBUGIN("Node::finishFunctionDeclaration", false);
    addType(Synt::FunctionDeclaration);
    reg("id", id);
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionExpression(Node * id, 
                                    const vector< Node* >& params, 
                                    const vector< Node* >& defaults, 
                                    Node * body) {  
    DEBUGIN("finishFunctionExpression", false);
    addType(Synt::FunctionExpression);
    reg("id", id);
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", false);
    this->finish();
    DEBUGOUT("", false);
}
void Node::finishIdentifier(const string name) {
    DEBUGIN("finishIdentifier", false);
    addType(Synt::Identifier);
    this->name = name;
    jvput("name", name);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishIfStatement(Node * test, 
                             Node * consequent, 
                             Node * alternate) {
    DEBUGIN("finishIfStatement", false);
    addType(Synt::IfStatement);
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishLabeledStatement(Node * label, 
                                  Node * body) {
    DEBUGIN("finishLabeledStatement", false);
    addType(Synt::LabeledStatement);
    reg("label", label);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishLiteral(ptrTkn token) {
    DEBUGIN("finishLiteral(ptrTkn token)", false);
    addType(Synt::Literal);
    if (token->literaltype == LiteralType["String"]) {
        jvput("value", token->strvalue);
    } else if (token->literaltype == LiteralType["Int"]) {
        jvput("value", token->intvalue);
    } else if (token->literaltype == LiteralType["Double"]) {
        jvput_dbl("value", stod(token->strvalue));
    } else if (token->literaltype == LiteralType["Bool"]) {
        jvput("value", token->bvalue);
    } else if (token->literaltype == LiteralType["Null"]) {
        jvput_null("value");
    } else if (token->literaltype == LiteralType["Regexp"]) {
        Value reg(kArrayType);
        vector<string> regvals = {token->strvalue, 
                                  token->flags,
                                  to_string(task->lineNumber),
                                  to_string(token->end), 
                                  to_string(token->end+1)};
        for (int i=0;i<regvals.size();i++) {
            reg.PushBack(
                         Value(regvals[i].data(),
                               regvals[i].length(),
                               *alloc).Move(),
                         *alloc);
        }
        jv.AddMember("value", reg, *alloc);
        regexPaths.push_back({"."});
    }
    jvput("raw", s(slice(task->sourceRaw, token->start, token->end)));
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishMemberExpression(const char16_t accessor, 
                                  Node * object, 
                                  Node * property) {
    DEBUGIN("finishMemberExpression", false);
    addType(Synt::MemberExpression);
    jvput("computed", (accessor == u'['));
    reg("object", object);
    reg("property", property);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishNewExpression(Node * callee, 
                               const vector<Node*>& args) {
    DEBUGIN("finishNewExpression", false);
    addType(Synt::NewExpression);
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishObjectExpression(const vector<Node*>& properties) {
    DEBUGIN("finishObjectExpression", false);
    addType(Synt::ObjectExpression);
    nodeVec("properties", properties);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishPostfixExpression(const string oper, 
                                   Node * argument) {
    DEBUGIN("finishPostfixExpression", false);
    addType(Synt::UpdateExpression);
    jvput("operator", oper);
    reg("argument", argument);
    jvput("prefix", false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishProgram(const vector< Node* >& body) {
    DEBUGIN("finishProgram", false);
    addType(Synt::Program);
    nodeVec("body", body);
    this->finish();
    //no parent node to call reg so add these atts. here.
    if (task->extra.range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        jv.AddMember("range", rangearr, *alloc);
    }
    if (task->extra.loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        jv.AddMember("loc", locjson, *alloc);
    }
    DEBUGOUT("", false);    
}


void Node::finishProperty(const string kind,
                          Node * key, 
                          Node * value) {
    DEBUGIN("finishProperty", false);
    addType(Synt::Property);
    reg("key", key);
    reg("value", value);
    jvput("kind", kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishReturnStatement(Node * argument) {
    DEBUGIN("finishReturnStatement", false);
    addType(Synt::ReturnStatement);
    reg("argument", argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSequenceExpression(const vector< Node* >& expressions) {
    DEBUGIN("finishSequenceExpression", false);
    addType(Synt::SequenceExpression);
    this->expressions = expressions;    
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchCase(Node * test, 
                            const vector< Node* >& consequent) {
    DEBUGIN("finishSwitchCase", false);
    addType(Synt::SwitchCase);
    reg("test", test);
    nodeVec("consequent", consequent);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchStatement(Node * discriminant, 
                                 const vector < Node* >& cases) {
    DEBUGIN("finishSwitchStatement", false);
    addType(Synt::SwitchStatement);
    reg("discriminant", discriminant);
    nodeVec("cases", cases);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThisExpression() {
    DEBUGIN("finishThisExpression()", false);
    addType(Synt::ThisExpression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThrowStatement(Node * argument) {
    DEBUGIN("finishThrowStatement", false);
    addType(Synt::ThrowStatement);
    reg("argument", argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishTryStatement(Node * block, 
                              const vector<Node*>& guardedHandlers, 
                              const vector<Node*>& handlers, 
                              Node * finalizer) {
    DEBUGIN("finishTryStatement", false);
    addType(Synt::TryStatement);
    reg("block", block);
    nodeVec("guardedHandlers", guardedHandlers);
    nodeVec("handlers", handlers);
    reg("finalizer", finalizer);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishUnaryExpression(const string oper, 
                                 Node * argument) {
    DEBUGIN("finishUnaryExpression", false);
    addType((oper == "++" || oper == "--") ? 
            Synt::UpdateExpression : Synt::UnaryExpression);
    jvput("operator", oper);
    reg("argument", argument);
    jvput("prefix", true);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclaration(const vector< Node* >& declarations, 
                                     const string kind) {
    DEBUGIN("finishVariableDeclaration",false);
    addType(Synt::VariableDeclaration);
    nodeVec("declarations", declarations);
    jvput("kind", kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclarator(Node * id, 
                                    Node * init) {
    DEBUGIN("finishVariableDeclarator", false);
    addType(Synt::VariableDeclarator);
    reg("id", id);
    reg("init", init);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWhileStatement(Node * test, 
                                Node * body) {
    DEBUGIN("finishWhileStatement", false);
    addType(Synt::WhileStatement);
    reg("test", test);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWithStatement(Node * object, 
                               Node * body) {
    DEBUGIN("finishWithStatement", false);
    addType(Synt::WithStatement);
    reg("object", object);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


class WrappingNode : public Node {
public:
    WrappingNode(ptrTkn startToken, 
                 vector<Node*>*heapNodesArg,
                 AllocatorType* alloc,
                 LinprimaTask* task) 
        : Node(false, true, heapNodesArg, 
               alloc, task) {
        DEBUGIN("WrappingNode(Token)", true);
        if (!hasJv) { 
            jv.SetObject();
            hasJv=true;
        }
        if (task->extra.range) {
            hasRange = true;
            range[0] = startToken->start;
            range[1] = 0;
        }

        if (task->extra.loc) {
            loc = this->WrappingSourceLocation(startToken);
        }

        DEBUGOUT("WrappingNode(Token)", true);
    }
    Loc WrappingSourceLocation(ptrTkn startToken);
};

Loc WrappingNode::WrappingSourceLocation(ptrTkn startToken) {
    DEBUGIN("WrappingSourceLocation (Token)", true);
    Loc result(0,0,0);
    if (startToken->type == TknType::StringLiteral) {
        result.startLine = startToken->startLineNumber;
        result.startColumn = 
            startToken->start - startToken->startLineStart;
    } else {
        result.startLine = startToken->lineNumber;
        result.startColumn = startToken->start - startToken->lineStart;
    }
    result.endLine = -1;
    result.endColumn = -1;
    //return result;
    DEBUGOUT("WrappingSrcLoc", true); 
    return result;
}

// Return true if there is a line terminator before the next token.

    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.


// Return true if the next token matches the specified punctuator.

//clang++ _refuses_ to inline match(val) properly on its own.
//TODO: switch this to typedefs for better typing integration.
//#define match(A) (lookahead->type == TknType::Punctuator && lookahead->strvalue == A)

class ParseTools {
public:
    ParseTools(u16string code,
               OptionsStruct options);
    ~ParseTools();
    //#throw_begin
    Node* parseProgram();
    //#throw_end
    void parse(Document& out, const bool retErrAsJson);
private:    
    AllocatorType *alloc;

    shared_ptr<LinprimaTask> task;
    const char16_t * sourceRaw;
    const int length;
    int& idx;
    int& lineNumber;
    int& lineStart;
    ExtraStruct &extra;
    StateStruct &state;
    ptrTkn &lookahead;
    Tokenizer scanner;
    vector<Node*> heapNodes;

    Node * makeNode(bool lookavailInit, bool exists);
    WrappingNode * makeWrappingNode(ptrTkn token);

    char16_t source(long pos);


    void clearNodeHeap();
    //#throw_begin
    bool peekLineTerminator();
    void expect(const string value);
    void expectTolerant(const string value);
    void expectKeyword(const string keyword);
    //#throw_end
    bool match(const string value);
    bool matchKeyword(const string keyword);
    bool matchAssign();
    //#throw_begin
    void consumeSemicolon();
    //#throw_end
    static bool isLeftHandSide(Node* expr);

    //#throw_begin
    Node* parseArrayInitialiser();
    Node* parsePropertyFunction(vector<Node*>& param, ptrTkn first);
    Node* parseObjectPropertyKey();
    Node* parseObjectProperty();
    Node* parseObjectInitialiser();
    Node* parseGroupExpression();
    Node* parsePrimaryExpression();
    vector< Node* > parseArguments();
    Node* parseNonComputedProperty();
    Node* parseNonComputedMember();
    Node* parseComputedMember();
    Node* parseNewExpression();
    Node* parseLeftHandSideExpressionAllowCall();
    Node* parseLeftHandSideExpression();
    Node* parsePostfixExpression();
    Node* parseUnaryExpression();
    //#throw_end
    static int binaryPrecedence(const TokenStruct *token, 
                                const bool allowIn);
    //#throw_begin
    Node* parseBinaryExpression();
    Node* parseConditionalExpression();
    Node* parseConciseBody();
    //#throw_end
    void validateParamNode(ReinterpretOptions& options,
                           Node *param, const string name);
    //#throw_begin
    ReinterpretOut reinterpretAsCoverFormalsList(vector< Node* >& expressions);
    Node* parseArrowFunctionExpression(const ReinterpretOut options, 
                                   Node *node);
    Node* parseAssignmentExpression();
    Node* parseExpression();
    vector< Node* > parseStatementList();
    Node* parseBlock();
    Node* parseVariableIdentifier();
    Node* parseVariableDeclaration(const string kind);
    vector< Node* > parseVariableDeclarationList(const string kind);
    Node* parseVariableStatement(Node* node);
    Node* parseConstLetDeclaration(const string kind);
    Node* parseEmptyStatement();
    Node* parseExpressionStatement(Node *node);
    Node* parseIfStatement(Node *node);
    Node* parseDoWhileStatement(Node *node);
    Node* parseWhileStatement(Node* node);
    Node* parseForVariableDeclaration();
    Node* parseForStatement(Node* node);
    Node* parseContinueStatement(Node* node);
    Node* parseBreakStatement(Node* node);
    Node* parseReturnStatement(Node* node);
    Node* parseWithStatement(Node* node);
    Node* parseSwitchCase();
    Node* parseSwitchStatement(Node *node);
    Node* parseThrowStatement(Node* node);
    Node* parseCatchClause();
    Node* parseTryStatement(Node* node);
    Node* parseDebuggerStatement(Node* node);
    Node* parseStatement();
    Node* parseFunctionSourceElements();
    void validateParam(ParseParamsOptions& options, 
                       ptrTkn param, const string name);
    bool parseParam(ParseParamsOptions& options);
    ParseParamsOut parseParams(ptrTkn firstRestricted);
    Node* parseFunctionDeclaration();
    Node* parseFunctionExpression();
    Node* parseSourceElement();
    vector< Node* > parseSourceElements();
    //#throw_end
};

ParseTools::ParseTools(const u16string code, OptionsStruct options) :    
    task(make_shared<LinprimaTask>(code,
                                        //code.data(), 
                                        //code.length(), 
                                       options)),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead),
         scanner(task) {
}

ParseTools::~ParseTools() {
    clearNodeHeap();
}


void Node::delNode(Node * toDel) {
    if (toDel == NULLNODE) { return; }
    auto iter = find(heapNodes->begin(), heapNodes->end(), toDel);
    if (iter != heapNodes->end()) { heapNodes->erase(iter); }
    delete (toDel);
}
void ParseTools::clearNodeHeap() {
    Node *tmp;
    while (heapNodes.size() > 0) {
        auto it = heapNodes.begin();
        tmp = *it;
        heapNodes.erase(it);
        //if (tmp->jv != nullptr)
        //    { json_object_put(tmp->jv); tmp->jv = nullptr; }

        delete (tmp);
    }
}

char16_t ParseTools::source(long pos) { return *(sourceRaw + pos); }

//throw_
bool ParseTools::peekLineTerminator() {
    DEBUGIN(" peekLineTerminator()", false);
    int pos = idx,
        line = lineNumber,
        start = lineStart;
    bool found;
    scanner.skipComment();
    found = (lineNumber != line);
    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("", false);
    return found;
}

reqinline
//throw_
void ParseTools::expect(const string value) { 
    //DEBUGIN(" expect(u16string value)", false);
    ptrTkn token = scanner.lex();

    if (token->type != TknType::Punctuator || 
          token->strvalue != value) {
        task->throwUnexpected(token); 
    }
    // DEBUGOUT("expect", false);
    return;
}



//throw_
void ParseTools::expectTolerant(const string value) {
    // DEBUGIN(" expectTolerant(u16string value)", false);
    if (extra.errorTolerant) {
        ptrTkn token = lookahead;


        if (token->type != TknType::Punctuator || 
              token->strvalue != value) {
            task->throwErrorTolerant(token, Messages[Mssg::UnexpectedToken], 
                               {token->strvalue});
        } else {
            scanner.lex();
        }
    } else {
        expect(value);
    }
    // DEBUGOUT("expectTol", false);
    return;
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
reqinline
//throw_
void ParseTools::expectKeyword(const string keyword) { 
    //DEBUGIN("expectKeyword", false);
    ptrTkn token = scanner.lex();
    if (token->type != TknType::Keyword || 
        token->strvalue != keyword) {
        task->throwUnexpected(token);
    }
    return;
}

bool ParseTools::match(const string value) { 
    //DEBUGIN("match", false);
    return lookahead->type == TknType::Punctuator 
        && lookahead->strvalue == value;
        } 

// Return true if the next token matches the specified keyword

bool ParseTools::matchKeyword(const string keyword) {
    // DEBUGIN(" matchKeyword(const u16string keyword)", false);
    //  DEBUGOUT("matchKey", false); 
    return lookahead->type == TknType::Keyword 
        && lookahead->strvalue == keyword;
}

    // Return true if the next token is an assignment operator

bool ParseTools::matchAssign() { 
    if (lookahead->type != TknType::Punctuator) {
        return false;
    }
    return has(lookahead->strvalue, {
                "=",
                "*=",
                "/=",
                "%=",
                "+=",
                "-=",
                "<<=",
                ">>=",
                ">>>=",
                "&=",
                "^=",
                "|="});
}

//#forward declarations of out-of-dependency order
//#or cross-dependent functions, in rough order of req.

//#can't dynamically initialize empty vectors
//#if func is passed by reference.
vector<Node*> EMPTY_NODE_LIST;

//throw_
void ParseTools::consumeSemicolon() {
    DEBUGIN(" consumeSemicolon()", false);
    int line;

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B || match(";")) { 
        scanner.lex();
        DEBUGOUT("", false); 
        return;
    }

    line = lineNumber;
    scanner.skipComment(); 
    if (lineNumber != line) {
        DEBUGOUT("", false); 
        return;
    }

    if (lookahead->type != TknType::EOFF && !match("}")) { 
        task->throwUnexpected(lookahead); 
    }
    DEBUGOUT("consumeSemi", false);
    return;
}

    // Return true if provided expression is LeftHandSideExpression

bool ParseTools::isLeftHandSide(Node* expr) {
    DEBUGIN("   isLeftHandSide(Node expr)", false);
    DEBUGOUT("isLeft", false);
    return expr->type == Syntax[Synt::Identifier] || expr->type == Syntax[Synt::MemberExpression];
};


Node* ParseTools::makeNode(bool lookavailInit, bool exists) {
    return new Node(lookavailInit, exists, &heapNodes, alloc, task.get());
}
WrappingNode* ParseTools::makeWrappingNode(ptrTkn token) {
    return new WrappingNode(token, &heapNodes, alloc, task.get());
}

// 11.1.4 Array Initialiser

//throw_
Node* ParseTools::parseArrayInitialiser() {
    DEBUGIN(" parseArrayInitialiser()", false);
    vector< Node* > elements;
    Node *node = makeNode(true, true);    

    expect("[");

    while (!match("]")) {

        if (match(",")) {
            scanner.lex();
            elements.push_back(NULLNODE);
        } else {
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
            elements.push_back(tmp);
#endif
#ifdef THROWABLE
            elements.push_back(parseAssignmentExpression()); 
#endif
            if (!match("]")) {
                expect(",");
            }
        }
    }

    scanner.lex();

    node->finishArrayExpression(elements);
    DEBUGOUT("parseArrInit", false);
    return node;
}

// 11.1.5 Object Initialiser

//throw_
Node* ParseTools::parsePropertyFunction(vector<Node*>& param, 
                                            ptrTkn first) {
    DEBUGIN(" parsePropertyFunction", false);
    bool previousStrict;
    Node *body,
        *node = makeNode(true, true);

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (!(first->isNull) && strict && isRestrictedWord(param[0]->name)) {
        task->throwErrorTolerant(first, Messages[Mssg::StrictParamName],{});
    }
    strict = previousStrict;
    node->finishFunctionExpression(NULLNODE, param, 
                                  EMPTY_NODE_LIST, body); 
    DEBUGOUT("parsePropFunction", false);
    return node;
}

//throw_
Node* ParseTools::parseObjectPropertyKey() {
    DEBUGIN(" parseObjectPropertyKey()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token->type == TknType::StringLiteral || 
        token->type == TknType::NumericLiteral) {
        if (strict && token->octal) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictOctalLiteral], {});
        }
        node->finishLiteral(token);
        DEBUGOUT("", false); 
        return node;
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("ParseObjPropKey", false);
    return node;
}



//@Empty_node_list is passed as a vector_of_arrays,
//@ where normally what would be passed into .finishFunctionExpression is
//@ an undefined variable. this might make the value null expectedly,
//@ while making this version render empty list.
//throw_
Node* ParseTools::parseObjectProperty() {
    DEBUGIN(" parseObjectProperty()", false);
    ptrTkn token;
    vector<Node*> param;
    Node *id, *key, *value, 
        *node = makeNode(true, true);

    token = lookahead;

    if (token->type == TknType::Identifier) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token->strvalue == "get" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            expect(")");
            ptrTkn tmpNull = scanner.makeToken();
            tmpNull->isNull = true;
            value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                          tmpNull);
            node->finishProperty("get", key, value);
            DEBUGOUT("parseObjProp", false); 
            return node;
        }
        if (token->strvalue == "set" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            token = lookahead;
            if (token->type != TknType::Identifier) {
                expect(")");
                task->throwErrorTolerant(token, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {token->strvalue});

                ptrTkn tmpNull = scanner.makeToken();
                tmpNull->isNull = true;                
                value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                              tmpNull);
            } else {
#ifndef THROWABLE
                Node *tmp = parseVariableIdentifier();
                param = vector< Node* >({ tmp });
#endif
#ifdef THROWABLE
                param = vector< Node* >({ parseVariableIdentifier() });
#endif
                expect(")");
                value = parsePropertyFunction(param, token);
            }
            node->finishProperty("set", key, value);
            DEBUGOUT("parseObjProp", false); 
            return node;
        }
        expect(":");
        value = parseAssignmentExpression();
        node->finishProperty("init", id, value);
        DEBUGOUT("parseObjProp", false); 
        return node;
    }
    if (token->type == TknType::EOFF 
        || token->type == TknType::Punctuator) {
        task->throwUnexpected(token);
        DEBUGOUT("parseObjProp", false); 
        return NULLNODE; //#just to satisfy warnings.
    } else {
        key = parseObjectPropertyKey();
        expect(":");
        value = parseAssignmentExpression();
        node->finishProperty("init", key, value);
        DEBUGOUT("parseObjProp", false); 
        return node;
    }
}

string GetStringCorrect(const Value& val) {
    return string(val.GetString(), val.GetStringLength());
}

//throw_
Node* ParseTools::parseObjectInitialiser() {
    DEBUGIN(" parseObjectInitialiser()", false);
    vector<Node*> properties;
    ptrTkn token;
    Node *property, 
        *node = makeNode(true, true);
    
    string keytype, key, name, kindname;
    int kind;
    map<string, int> kmap;

    expect("{");

    while (!match("}")) {
        property = parseObjectProperty();
        

        const Value& keyobj = property->jv["key"];

        keytype = GetStringCorrect(keyobj["type"]);

        if (keytype == Syntax[Synt::Identifier]) {
            name = GetStringCorrect(keyobj["name"]);            
        } else {
            if (keyobj["value"].IsString()) {
                name = GetStringCorrect(keyobj["value"]);
            } else {
                name = to_string(keyobj["value"].GetDouble());
            }
        }
        kindname = GetStringCorrect(property->jv["kind"]);
        kind = (kindname == "init") ? PropertyKind["Data"] : 
            (kindname == "get") ? PropertyKind["Get"] : PropertyKind["Set"];

        key = "$";
        key.append(name);
        if (hasStringKey<int>(key,kmap)) {
            if (kmap[key] == PropertyKind["Data"]) {
                if (strict && kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::StrictDuplicateProperty],{});
                } else if (kind != PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                }
            } else {
                if (kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                } else if (kmap[key] & kind) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::AccessorGetSet],{});
                }
            }
            kmap[key] |= kind;
        } else {
            kmap[key] = kind;
        }

        properties.push_back(property);

        if (!match("}")) {
            expectTolerant(",");
        }
    }

    expect("}");

    node->finishObjectExpression(properties);
    DEBUGOUT("parseObjectInit", false);
    return node;
}

 // 11.1.6 The Grouping Operator
//throw_
Node* ParseTools::parseGroupExpression() {
    DEBUGIN(" parseGroupExpression()", false);
    Node *expr;
    expect("(");
    if (match(")")) {
        scanner.lex();
        DEBUGOUT("", false);
        Node *tmpnode = makeNode(false, true);
        tmpnode->type = PlaceHolders["ArrowParameterPlaceHolder"]->type;
        return tmpnode;
    }
    ++(state.parenthesisCount);
    expr = parseExpression();
    expect(")");
    string debugmsg="parseGroupExpr()";
    DEBUGOUT(debugmsg, false);
    return expr;
}


// 11.1 Primary Expressions
//throw_
Node* ParseTools::parsePrimaryExpression() {
    DEBUGIN(" parsePrimaryExpression()", false);
    //# there's some opportunity here for nested function calls
    //# by using preprocess if defined / else / endifs 
    //# that use nested in non-asm. But they're just tokens so no
    //# depth (and high copy cost) like nodes.
    //# and because there are so many it makes this function visually
    //# hard to follow if we use nested whenever possible in this.
    TknType type; 
    ptrTkn token;
    Node *expr, *node;

    if (match("(")) {
        return DBGRET("", parseGroupExpression());
    }

    if (match("[")) {
        return DBGRET("", parseArrayInitialiser());
    }

    if (match("{")) {
        return DBGRET("", parseObjectInitialiser());
    }

    type = lookahead->type;
    node = makeNode(true, true);
    expr = node;

    if (type == TknType::Identifier) {
        ptrTkn tmp = scanner.lex();
        expr->finishIdentifier(tmp->strvalue);
    } else if (type == TknType::StringLiteral || 
               type == TknType::NumericLiteral) {

        if (strict && lookahead->octal) {
            task->throwErrorTolerant(lookahead, 
                               Messages[Mssg::StrictOctalLiteral], {});
        }
        ptrTkn tmp = scanner.lex();
        expr->finishLiteral(tmp);
    } else if (type == TknType::Keyword) {
        if (matchKeyword("function")) {
            expr->unused();
            return DBGRET("", parseFunctionExpression());
        }
        if (matchKeyword("this")) {
            scanner.lex();
            expr->finishThisExpression();
        } else {
            ptrTkn tmp = scanner.lex();
            task->throwUnexpected(tmp);
        }
    } else if (type == TknType::BooleanLiteral) {
        token = scanner.lex();
        token->bvalue = (token->strvalue == "true");
        token->literaltype = LiteralType["Bool"];
        expr->finishLiteral(token);
    } else if (type == TknType::NullLiteral) {
        token = scanner.lex();
        token->isNull = true;
        token->literaltype = LiteralType["Null"];
        expr->finishLiteral(token);
    } else if (match("/") || match("/=")) {
        ptrTkn tmp;
        if (extra.tokenTracking) {
            tmp = scanner.collectRegex();
            expr->finishLiteral(tmp);
        } else {
            tmp = scanner.scanRegExp();
            expr->finishLiteral(tmp);
        }
        scanner.peek();
    } else {
        ptrTkn tmp = scanner.lex();
        task->throwUnexpected(tmp);
    }

    DEBUGOUT("", false);
    return expr;
}

// 11.2 Left-Hand-Side Expressions
//throw_
vector< Node* > ParseTools::parseArguments() {
    DEBUGIN(" parseArguments()", false);
    vector< Node* > args; 
    expect("(");
    if (!match(")")) {
        while (idx < length) {
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
            args.push_back(tmp);
#endif
#ifdef THROWABLE
            args.push_back(parseAssignmentExpression());
#endif
            if (match(")")) {
                break;
            }
            expectTolerant(",");
        }
    }
    expect(")");
    DEBUGOUT("parseArguments", false);
    return args;
}

//throw_
Node* ParseTools::parseNonComputedProperty() {
    DEBUGIN(" parseNonComputedProperty()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    if (!Tokenizer::isIdentifierName(token->type)) {
        task->throwUnexpected(token);
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("", false);
    return node;
}

//throw_
Node* ParseTools::parseNonComputedMember() {
    DEBUGIN(" parseNonComputedMember()", false);
    expect(".");
    return DBGRET("parseNonComp", parseNonComputedProperty());
}

//throw_
Node* ParseTools::parseComputedMember() {
    DEBUGIN(" parseComputedMember()", false);
    Node *expr;
    expect("[");
    expr = parseExpression();
    expect("]");
    DEBUGOUT("parseComp", false);
    return expr;
}

//throw_
Node* ParseTools::parseNewExpression() {
    DEBUGIN(" parseNewExpression()", false);
    vector< Node* > args;
    Node *callee, 
        *node = makeNode(true, true);

    expectKeyword("new");
    callee = parseLeftHandSideExpression();
    if (match("(")) { 
        args = parseArguments(); 
    }

    node->finishNewExpression(callee, args);
    DEBUGOUT("parseNewExpr", false);
    return node;
}

//throw_
Node* ParseTools::parseLeftHandSideExpressionAllowCall() {
    DEBUGIN(" parseLeftHandSideExpressionAllowCall()", false);
    vector< Node * > args;
    Node *expr, *property, *tmpnode;
    ptrTkn startToken;
    bool previousAllowIn = state.allowIn;

    startToken = lookahead;
    state.allowIn = true;
    if (matchKeyword("new")) { 
        expr = parseNewExpression();
    } else {
        expr = parsePrimaryExpression();
    }

    for (;;) {
        if (match(".")) {
            property = parseNonComputedMember();
            tmpnode = makeWrappingNode(startToken);

            tmpnode->finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else if (match("(")) {
            args = parseArguments();
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishCallExpression(expr, args);
            expr = tmpnode;
        } else if (match("[")) {
            property = parseComputedMember();
            tmpnode = makeWrappingNode(startToken);            
            tmpnode->finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else {
            break;
        }
    }

    state.allowIn = previousAllowIn;

    DEBUGOUT("parseLeftHandSideExprAllow", false); 
    return expr; 
}

//throw_
Node* ParseTools::parseLeftHandSideExpression() {
    DEBUGIN(" parseLeftHandSideExpression()", false);
    Node *tmpnode, *expr, *property;
    ptrTkn startToken;

    softAssert(state.allowIn, 
               "callee of new expression always allow in keyword.");
    startToken = lookahead;
    if (matchKeyword("new")) { 
        expr = parseNewExpression();
    } else { 
        expr = parsePrimaryExpression();
    }

    for (;;) {
        if (match("[")) {
            property = parseComputedMember();
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else if (match(".")) {
            property = parseNonComputedMember();
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else {            
            break;
        }
    }
    DEBUGOUT("parseLeftHandSideExpr", false);
    return expr;
}

// 11.3 Postfix Expressions
//throw_
Node* ParseTools::parsePostfixExpression() {
    DEBUGIN(" parsePostfixExpression()", false);
    Node *expr, *tmpnode;
    bool pltresult;
    ptrTkn token, startToken = lookahead;

    expr = parseLeftHandSideExpressionAllowCall();
    
    if (lookahead->type == TknType::Punctuator) {
        if (match("++") || match("--")) {
            pltresult = peekLineTerminator(); //#throw52
            if (!pltresult) {
                // 11.3.1, 11.3.2
                if (strict && expr->type == Syntax[Synt::Identifier] && 
                    isRestrictedWord(expr->name)) {
                    task->throwErrorTolerant(NULLPTRTKN,
                                       Messages[Mssg::StrictLHSPostfix],{});
                }

                if (!isLeftHandSide(expr)) {
                    task->throwErrorTolerant(NULLPTRTKN,
                                       Messages[Mssg::InvalidLHSInAssignment],
                                       {});
                }

                token = scanner.lex();
                tmpnode = makeWrappingNode(startToken);
                tmpnode->finishPostfixExpression(token->strvalue, expr);
                DEBUGOUT("parsePostfix", false); 
                return tmpnode;
            }
        }
    }

    DEBUGOUT("parsePostfix", false);
    return expr;
}

// 11.4 Unary Operators
//throw_
Node* ParseTools::parseUnaryExpression() {
    DEBUGIN(" parseUnaryExpression()", false);
    ptrTkn token, startToken;
    Node *expr, *tmpnode;
    u16string exprname;

    if (lookahead->type != TknType::Punctuator 
        && lookahead->type != TknType::Keyword) {
        expr = parsePostfixExpression();
    } else if (match("++") || match("--")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        // 11.4.4, 11.4.5
        if (strict && expr->type == Syntax[Synt::Identifier] 
            && isRestrictedWord(expr->name)) {
            task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::StrictLHSPrefix],{});
        }

        if (!isLeftHandSide(expr)) {
            task->throwErrorTolerant(NULLPTRTKN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else if (match("+") || match("-") || match("~") || match("!")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else if (matchKeyword("delete") 
               || matchKeyword("void") 
               || matchKeyword("typeof")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        if (strict && token->strvalue == "delete" 
            && expr->type == Syntax[Synt::Identifier]) {
            task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::StrictDelete], {});
        }
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else {
        expr = parsePostfixExpression();
    }

    DEBUGOUT("parseUnary", false);
    return expr;
}

int ParseTools::binaryPrecedence(const TokenStruct *token, 
                     const bool allowIn) {
    DEBUGIN(" binaryPrecedence(Tokenstruct token, bool allowIn)", false);
    int prec = 0;
    string tokval;

    if (token->type != TknType::Punctuator 
        && token->type != TknType::Keyword) {
        DEBUGOUT("binaryPrec", false); 
        return 0;
    }
    tokval = token->strvalue;

    if (tokval == "||") {
        prec = 1;
    } else if (tokval == "&&") {
        prec = 2;
    } else if (tokval == "|") {
        prec = 3;
    } else if (tokval == "^") {
        prec = 4;
    } else if (tokval == "&") {
        prec = 5;
    } else if (has(tokval, {"==", "!=", "===", "!=="})) {
        prec = 6;
    } else if (has(tokval, {"<", ">", "<=", ">=", "instanceof"})) {
        prec = 7;
    } else if (tokval == "in") {
        prec = allowIn ? 7 : 0;
    } else if (has(tokval,
                   {"<<", ">>", ">>>"})) {
        prec = 8;
    } else if (tokval == "+" || tokval == "-") {
        prec = 9;
    } else if (has(tokval, {"*", "/", "%"})) {
        prec = 11;
    }

    DEBUGOUT("binaryPrec", false);
    return prec;
}

// 11.5 Multiplicative Operators
// 11.6 Additive Operators
// 11.7 Bitwise Shift Operators
// 11.8 Relational Operators
// 11.9 Equality Operators
// 11.10 Binary Bitwise Operators
// 11.11 Binary Logical Operators
//throw_
Node* ParseTools::parseBinaryExpression() {
    DEBUGIN(" parseBinaryExpression()", false);

    Node *tmpnode, *expr, *left, 
        *right;
    vector < Node* > nodestack;
    ptrTkn marker, token, nulltok = scanner.makeToken();
    nulltok->isNull = true;
    vector< ptrTkn > markers, tokstack;
    string oper;
    int i, prec;

    marker = lookahead;

    left = parseUnaryExpression();

    if (left->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) {
        //? placeholder
        DEBUGOUT("parseBinary1", false); 
        return left;
    }

    token = lookahead;    
    prec = binaryPrecedence(token.get(), state.allowIn);
    if (prec == 0) {
        DEBUGOUT("parseBinary2", false); 
        return left;
    }
    token->prec = prec;
    scanner.lex();

    markers.push_back(marker);
    markers.push_back(lookahead);
    right = parseUnaryExpression();

    //we want to mirror esprima's js as close as possible. because it uses
    //a stack of mixed tokens or nodes (and the math is done so it always 
    //knows the type of the position its using) we've simply made two different
    //type stackeds (where a position is always only occupied by a non-null value in one of the stacks)
    //and access the right stack at the right time.
    nodestack.push_back(left);
    tokstack.push_back(nulltok);
    nodestack.push_back(NULLNODE);
    tokstack.push_back(token);
    nodestack.push_back(right);
    tokstack.push_back(nulltok);

    while ((prec = binaryPrecedence(lookahead.get(), state.allowIn)) > 0) { 

        // Reduce: make a binary expression from the three topmost entries.
        while ((tokstack.size() > 2) && 
               (prec <= tokstack[tokstack.size() - 2]->prec)) { 
            //? will this work the same in c++ as in js?
            right = nodestack.back(); 
            nodestack.pop_back(); tokstack.pop_back();
            oper = tokstack.back()->strvalue;
            nodestack.pop_back(); tokstack.pop_back();
            left = nodestack.back();
            nodestack.pop_back(); tokstack.pop_back();
            markers.pop_back();

            expr = makeWrappingNode(markers[markers.size() - 1]);

            expr->finishBinaryExpression(oper, left, right);

            nodestack.push_back(expr); tokstack.push_back(nulltok);
        }

        // Shift.
        token = scanner.lex();
        token->prec = prec;
        nodestack.push_back(NULLNODE);
        tokstack.push_back(token);
        markers.push_back(lookahead);
        expr = parseUnaryExpression();
        nodestack.push_back(expr);
        tokstack.push_back(nulltok);
    }

    // Final reduce to clean-up the stack.
    i = nodestack.size() - 1;
    expr = nodestack[i];
    markers.pop_back();
    while (i > 1) {
        tmpnode = makeWrappingNode(markers.back());
        markers.pop_back();
        tmpnode->finishBinaryExpression(tokstack[i - 1]->strvalue, 
                                        nodestack[i - 2], expr);
        expr = tmpnode;
        i -= 2;
    }

    DEBUGOUT("parseBinary3", false);
    return expr;
}


// 11.12 Conditional Operator

//throw_
Node* ParseTools::parseConditionalExpression() {
    DEBUGIN(" parseConditionalExpression()", false);
    Node *expr, *tmpnode, 
        *consequent, *alternate;
    bool previousAllowIn;
    ptrTkn startToken;

    startToken = lookahead;

    expr = parseBinaryExpression();
    if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) { 
        //? ever supposed to eval. to true? cause it might in some cases
        //? even tho it seems in javascript it never ever will.

        DEBUGOUT("parseCondExpr1", false); 
        return expr;
    }
    if (match("?")) {
        scanner.lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        state.allowIn = previousAllowIn;
        expect(":");
        alternate = parseAssignmentExpression();

        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishConditionalExpression(expr, consequent, alternate);
        DEBUGOUT("parseCondExpr2", false); 
        return tmpnode;
    }

    DEBUGOUT("parseCondExpr3", false);
    return expr;
}

// [ES6] 14.2 Arrow Function
//throw_
Node* ParseTools::parseConciseBody() {
    DEBUGIN(" parseConciseBody()", false);
    if (match("{")) {
        return DBGRET("parseConciseBody", parseFunctionSourceElements());
    }
    return DBGRET("parseConciseBody", parseAssignmentExpression());
}

void ParseTools::validateParamNode(ReinterpretOptions& options,
                       Node *param, const string name) {
    DEBUGIN("validateParamNode(Reinterp, Node, u16str)", false);
    string key = "$";
    key.append(name);
    if (strict) {
        if (isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamName];
        }
        if (hasSet(key, options.paramSet)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    } else if (options.firstRestricted == NULLNODE) {
        if (isRestrictedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamName];
        } else if (isStrictModeReservedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictReservedWord];
        } else if (hasSet(key, options.paramSet)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    }
    options.paramSet.insert(key);
    DEBUGOUT("validateParamNode", false);
}

//throw_
ReinterpretOut ParseTools::reinterpretAsCoverFormalsList(
                                    vector< Node* >& expressions) { 
    DEBUGIN("reinterpretAsCover", false);
    int i, len, defaultCount;
    Node *param;
    vector< Node* > params;
    vector< Node* > defaults;
    ReinterpretOptions opts;
    ReinterpretOut reOut; 
    void* rest;

    defaultCount = 0;
    rest = nullptr;

    for (i = 0, len = expressions.size(); i < len; i += 1) {
        param = expressions[i];
        if (param->type == Syntax[Synt::Identifier]) {
            params.push_back(param);
            defaults.push_back(NULLNODE);
            //null defaults might be skipped instead of regged check this.
            validateParamNode(opts, param, param->name);
        } else if (param->type == Syntax[Synt::AssignmentExpression]) {
            Node *l = param->leftAssign; 
            Node *r = param->rightAssign; 

            params.push_back(l);
            defaults.push_back(r);
            ++defaultCount;
            validateParamNode(opts, l, l->name);
        } else {
            reOut.isNull = true;
            DEBUGOUT("", false); 
            return reOut; 
        }
    }

    if (opts.message == Messages[Mssg::StrictParamDupe]) {
        task->throwError(
                   //strict ? opts.stricted : opts.firstRestricted, //? replicate?
                   NULLPTRTKN,
                   opts.message,
                   {}
                   );
    }

    if (defaultCount == 0) {
        defaults.clear();
    }
    reOut.params = params;
    reOut.defaults = defaults;
    reOut.rest = rest;
    reOut.stricted = opts.stricted;
    reOut.firstRestricted = opts.firstRestricted;
    reOut.message = opts.message;

    DEBUGOUT("reinterpret", false);
    return reOut;
}

//throw_
Node* ParseTools::parseArrowFunctionExpression(
                                   const ReinterpretOut options, 
                                   Node *node) {
    DEBUGIN(" parseArrowFunctionExpression", false);
    bool previousStrict;
    Node *body;

    expect("=>");
    previousStrict = strict;

    body = parseConciseBody();

    if (strict && options.firstRestricted != NULLNODE) { 
        task->throwError(NULLPTRTKN, options.message, {});
        //        task->throwError(options.firstRestricted, options.message, {});
    }
    if (strict && options.stricted != NULLNODE) {
        task->throwError(NULLPTRTKN, options.message, {});
        //        task->throwErrorTolerant(options.stricted, options.message, {});
    }

    strict = previousStrict;

    node->finishArrowFunctionExpression(options.params, 
                                       options.defaults, 
                                       body, 
                                       body->type != Syntax[Synt::BlockStatement]);
    DEBUGOUT("parseArrowFuncExpr", false);
    return node;
}

    // 11.13 Assignment Operators


//throw_
Node* ParseTools::parseAssignmentExpression() {
    DEBUGIN(" parseAssignmentExpression()", false);
    int oldParenthesisCount;
    ptrTkn token, startToken;
    Node *expr, *right, *tmpnode;
    vector<Node*> reIn;
    ReinterpretOut list;


    oldParenthesisCount = state.parenthesisCount;

    startToken = lookahead;
    token = lookahead;

    expr = parseConditionalExpression();
    list.isNull = true;
    if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type //? will work?
        || match("=>")) {

        if (state.parenthesisCount == oldParenthesisCount ||
            state.parenthesisCount == (oldParenthesisCount + 1)) {      
            if (expr->type == Syntax[Synt::Identifier]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn); 
            } else if (expr->type == Syntax[Synt::AssignmentExpression]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn);
                expr->unused();
            } else if (expr->type == Syntax[Synt::SequenceExpression]) {
                list = reinterpretAsCoverFormalsList(expr->expressions);
                expr->unused();
            } else if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) {
                expr->unused();
                list = reinterpretAsCoverFormalsList(reIn); 
            }
            if (!(list.isNull)) {
                return DBGRET("parseAssignExpr1", parseArrowFunctionExpression(list, makeWrappingNode(startToken)));
            }
        }
    }

    if (matchAssign()) {
        // LeftHandSideExpression
        if (!isLeftHandSide(expr)) {
            task->throwErrorTolerant(NULLPTRTKN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        // 11.13.1
        if (strict 
            && expr->type == Syntax[Synt::Identifier] 
            && isRestrictedWord(expr->name)) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictLHSAssignment], {});
        }

        token = scanner.lex();
        right = parseAssignmentExpression();
        tmpnode = makeWrappingNode(startToken);

        tmpnode->finishAssignmentExpression(token->strvalue, expr, right); 
        DEBUGOUT("parseAssignExpr2", false); 
        return tmpnode;
    }

    DEBUGOUT("parseAssignExpr3", false); 
    return expr;
}

// 11.14 Comma Operator
//throw_
Node* ParseTools::parseExpression() { 
    DEBUGIN(" parseExpression()", false);
    Node *expr; 
    ptrTkn startToken = lookahead;
    vector< Node* > expressions;

    expr = parseAssignmentExpression();

    if (match(",")) {
        expressions.push_back(expr);
        while (idx < length) {
            if (!match(",")) {
                break;
            }
            scanner.lex();
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
            expressions.push_back(tmp);
#endif
#ifdef THROWABLE
            expressions.push_back(parseAssignmentExpression());
#endif
        }

        expr = makeWrappingNode(startToken);
        expr->finishSequenceExpression(expressions);
    }
    string debugmsg = "parseExpr()";
    debugmsg.append(expr->type); 
    DEBUGOUT(debugmsg, false); 
    return expr;
}

// 12.1 Block
//throw_
vector< Node* > ParseTools::parseStatementList() { 
    DEBUGIN(" parseStatementList()", false);
    vector< Node* > list;
    Node *statement;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement == NULLNODE) { 
            break;
        }
        list.push_back(statement);
    }

    DEBUGOUT("parseStatementList", false); 
    return list;
}

//throw_
Node* ParseTools::parseBlock() { 
    DEBUGIN(" parseBlock()", false);
    vector< Node* > block;
    Node *node = makeNode(true, true);

    expect("{");
    block = parseStatementList();
    expect("}");
    node->finishBlockStatement(block);
    DEBUGOUT("parseBlock", false); 
    return node;
}


// 12.2 Variable Statement

//throw_
Node* ParseTools::parseVariableIdentifier() { 
    DEBUGIN(" parseVariableIdentifier()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    if (token->type != TknType::Identifier) {
        task->throwUnexpected(token);
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("parseVariableIdent", false); 
    return node;
}

//throw_
Node* ParseTools::parseVariableDeclaration(const string kind) { 
    DEBUGIN(" parseVariableDeclaration(u16string kind)", false);
    Node *id, *init,
        *node = makeNode(true, true);
    init = NULLNODE;

    id = parseVariableIdentifier();

    // 12.2.1
    if (strict && isRestrictedWord(id->name)) {
        task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::StrictVarName], {});
    }

    if (kind == "const") {
        expect("=");
        init = parseAssignmentExpression();
    } else if (match("=")) {
        scanner.lex();
        init = parseAssignmentExpression();
    }

    node->finishVariableDeclarator(id, init);
    DEBUGOUT("parseVariableDecl", false); 
    return node;
}

//throw_
vector< Node* > ParseTools::parseVariableDeclarationList(const string kind) {
    DEBUGIN("parseVariableDeclarationList", false);
    vector< Node* > list; 

    do {
#ifndef THROWABLE
        Node *tmp = parseVariableDeclaration(kind);
        list.push_back(tmp);
#endif
#ifdef THROWABLE
        list.push_back(parseVariableDeclaration(kind));
#endif
        if (!match(",")) {
            break;
        }
        scanner.lex();
    } while (idx < length);

    DEBUGOUT("parseVariableDeclarationList", false); 
    return list;
}

//throw_
Node* ParseTools::parseVariableStatement(Node* node) { 
    DEBUGIN(" parseVariableStatement(Node node)", false);
    vector< Node* > declarations;

    expectKeyword("var");
    declarations = parseVariableDeclarationList(""); 
    consumeSemicolon();

    node->finishVariableDeclaration(declarations, "var");
    DEBUGOUT("parseVariableStatement", false); 
    return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//throw_
Node* ParseTools::parseConstLetDeclaration(const string kind) { 
    DEBUGIN(" parseConstLetDeclaration(u16string kind)", false);
    vector< Node* > declarations;
    Node *node = makeNode(true, true);

    expectKeyword(kind);
    declarations = parseVariableDeclarationList(kind);
    consumeSemicolon();
    node->finishVariableDeclaration(declarations, kind);
    DEBUGOUT("parseConstLetDeclaration", false); 
    return node;
}

// 12.3 Empty Statement

//throw_
Node* ParseTools::parseEmptyStatement() { 
    DEBUGIN(" parseEmptyStatement()", false);
    Node *node = makeNode(true, true);
    expect(";");
    node->finishEmptyStatement();
    DEBUGOUT("parseEmptyStatement", false); 
    return node;
}

// 12.4 Expression Statement
//throw_
Node* ParseTools::parseExpressionStatement(Node *node) { 
    DEBUGIN(" parseExpressionStatement(Node node)", false);
    Node *expr;
    expr = parseExpression();
    consumeSemicolon();
    node->finishExpressionStatement(expr);
    DEBUGOUT("", false); 
    return node;
}

// 12.5 If statement
//throw_
Node* ParseTools::parseIfStatement(Node *node) { 
    DEBUGIN(" parseIfStatement(Node node)", false);
    Node *test, 
        *consequent, *alternate;
    expectKeyword("if");
    expect("(");
    test = parseExpression();
    expect(")");
    consequent = parseStatement();
    if (matchKeyword("else")) {
        scanner.lex();
        alternate = parseStatement();
    } else {
        alternate = NULLNODE;
    }
    node->finishIfStatement(test, consequent, alternate);
    DEBUGOUT("parseIfStatement", false); 
    return node;
}

// 12.6 Iteration Statements

//throw_
Node* ParseTools::parseDoWhileStatement(Node *node) { 
    DEBUGIN(" parseDoWhileStatement(Node node)", false);
    Node *body, *test;
    bool oldInIteration;

    expectKeyword("do");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    expectKeyword("while");
    expect("(");
    test = parseExpression();
    expect(")");
    if (match(";")) {
        scanner.lex();
    }
    node->finishDoWhileStatement(body, test);
    DEBUGOUT("parseDoWhileStatement", false);
    return node;
}

//throw_
Node* ParseTools::parseWhileStatement(Node* node) { 
    DEBUGIN(" parseWhileStatement(Node node)", false);
    Node *test, *body;
    bool oldInIteration;
    expectKeyword("while");
    expect("(");
    test = parseExpression();
    expect(")");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    node->finishWhileStatement(test, body);
    DEBUGOUT("parseWhileStatement", false);
    return node;
}

//throw_
Node* ParseTools::parseForVariableDeclaration() { 
    DEBUGIN(" parseForVariableDeclaration()", false);
    ptrTkn token;
    vector< Node* > declarations;
    Node *node = makeNode(true, true);

    token = scanner.lex();
    declarations = parseVariableDeclarationList("");
    node->finishVariableDeclaration(declarations, token->strvalue);
    DEBUGOUT("parseForVariableDeclaration", false); 
    return node;
}

//throw_
Node* ParseTools::parseForStatement(Node* node) { 
    DEBUGIN(" parseForStatement(Node node)", false);

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node *body, *left, *right =0x0, 
        *update, *test, *init;
    left=NULLNODE;
    update=NULLNODE; test=NULLNODE; init=NULLNODE;

    expectKeyword("for");
    expect("(");

    if (match(";")) {
        scanner.lex();
    } else {
        if (matchKeyword("var") || matchKeyword("let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (init->jv["declarations"].Size() == 1 
                && matchKeyword("in")) { 

                scanner.lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        } else {
            state.allowIn = false;
            init = parseExpression();
            state.allowIn = previousAllowIn;

            if (matchKeyword("in")) {
                // LeftHandSideExpression
                if (!isLeftHandSide(init)) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                                       Messages[Mssg::InvalidLHSInForIn],
                                       {});
                }

                scanner.lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        }

        if (left == NULLNODE) {
            expect(";");
        }
    }

    if (left == NULLNODE) {

        if (!match(";")) {
            test = parseExpression();
        }
        expect(";");

        if (!match(")")) {
            update = parseExpression();
        }
    }

    expect(")");

    oldInIteration = state.inIteration;
    state.inIteration = true;

    body = parseStatement();

    state.inIteration = oldInIteration;

    if (left == NULLNODE) {
        node->finishForStatement(init, test, update, body);
    } else {
        node->finishForInStatement(left, right, body);
    }
    DEBUGOUT("parseForStatement", false);
    return node;
}

// 12.7 The continue statement
//throw_
Node* ParseTools::parseContinueStatement(Node* node) {
    DEBUGIN(" parseContinueStatement(Node node)", false);
    Node *label;
    label = NULLNODE;
    string key;
    bool pltresult;

    expectKeyword("continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!state.inIteration) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {

        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);
        if (!(hasSet(key, state.labelSet))) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == NULLNODE && !(state.inIteration)) {
        task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
    }

    node->finishContinueStatement(label);
    DEBUGOUT("parseContinueStatement", false);
    return node;
}

// 12.8 The break statement
//throw_
Node* ParseTools::parseBreakStatement(Node* node) {
    DEBUGIN(" parseBreakStatement(Node node)", false);
    Node *label;
    string key;
    bool pltresult;
    label = NULLNODE;

    expectKeyword("break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {
        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);

        if (!(hasSet(key, state.labelSet))) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == NULLNODE && !(state.inIteration || state.inSwitch)) {
        task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak], {});
    }

    node->finishBreakStatement(label);
    DEBUGOUT("parseBreakStatement", false);
    return node;
}

// 12.9 The return statement
//throw_
Node* ParseTools::parseReturnStatement(Node* node) {
    DEBUGIN(" parseReturnStatement(Node node)", false);
    Node *argument;
    bool pltresult;
    argument = NULLNODE;

    expectKeyword("return");

    if (!(state.inFunctionBody)) {
        task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::IllegalReturn], {});
    }

    // 'return' followed by a space and an identifier is very common.
    if (source(idx) == 0x20) {
        if (isIdentifierStart(source(idx + 1))) {
            argument = parseExpression();
            consumeSemicolon();
            node->finishReturnStatement(argument);
            DEBUGOUT("parseReturnStatement", false); 
            return node;
        }
    }

    pltresult = peekLineTerminator();
    if (pltresult) {
        node->finishReturnStatement(NULLNODE);
        DEBUGOUT("parseReturnStatement", false);
        return node;
    }

    if (!match(";")) {
        if (!match("}") && lookahead->type != TknType::EOFF) {
            argument = parseExpression();
        }
    }

    consumeSemicolon();

    node->finishReturnStatement(argument);
    DEBUGOUT("parseReturnStatement", false);
    return node;
}

// 12.10 The with statement
//throw_
Node* ParseTools::parseWithStatement(Node* node) {
    DEBUGIN(" parseWithStatement(Node node)", false);
    Node *object, *body;

    if (strict) {
        // TODO(ikarienator): Should we update the test cases instead?
        scanner.skipComment(); //ev
        task->throwErrorTolerant(NULLPTRTKN, 
                           Messages[Mssg::StrictModeWith], {});
    }

    expectKeyword("with");
    expect("(");
    object = parseExpression();
    expect(")");
    body = parseStatement();
    node->finishWithStatement(object, body);
    DEBUGOUT("parseWithStatement", false);
    return node;
}

// 12.10 The swith statement
//throw_
Node* ParseTools::parseSwitchCase() {
    DEBUGIN(" parseSwitchCase()", false);
    Node *test, *statement, 
        *node = makeNode(true, true);
    vector< Node* > consequent;

    if (matchKeyword("default")) {
        scanner.lex();
        test = NULLNODE;
    } else {
        expectKeyword("case");
        test = parseExpression();
    }
    expect(":");

    while (idx < length) {
        if (match("}") 
            || matchKeyword("default") 
            || matchKeyword("case")) {
            break;
        }
        statement = parseStatement();
        consequent.push_back(statement);
    }

    node->finishSwitchCase(test, consequent);
    DEBUGOUT("parseSwitchCase", false);
    return node;
}

//throw_
Node* ParseTools::parseSwitchStatement(Node *node) {
    DEBUGIN(" parseSwitchStatement(Node node)", false);
    Node *discriminant, *clause; 
    vector< Node* > cases; 
    bool oldInSwitch, defaultFound;

    expectKeyword("switch");
    expect("(");
    discriminant = parseExpression();
    expect(")");
    expect("{");
    if (match("}")) {
        scanner.lex();
        node->finishSwitchStatement(discriminant, cases);
        DEBUGOUT("parseSwitchStatement", false); 
        return node;
    }
    oldInSwitch = state.inSwitch;
    state.inSwitch = true;
    defaultFound = false;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        clause = parseSwitchCase();
        if (clause->jv["test"].IsNull()) {
            if (defaultFound) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::MultipleDefaultsInSwitch],{});
            }
            defaultFound = true;
        }
        cases.push_back(clause);
    }

    state.inSwitch = oldInSwitch;
    expect("}");
    node->finishSwitchStatement(discriminant, cases);
    DEBUGOUT("parseSwitchStatement", false);
    return node;
}

// 12.13 The throw statement
//throw_
Node* ParseTools::parseThrowStatement(Node* node) {
    DEBUGIN(" parseThrowStatement", false);
    Node* argument;
    bool pltresult;

    expectKeyword("throw");
    pltresult = peekLineTerminator();
    if (pltresult) {
        task->throwError(NULLPTRTKN, 
                   Messages[Mssg::NewlineAfterThrow],{});
    }
    argument = parseExpression();
    consumeSemicolon();
    node->finishThrowStatement(argument);
    DEBUGOUT("parseThrowStatement", false);
    return node;
}

// 12.14 The try statement

//throw_
Node* ParseTools::parseCatchClause() {
    DEBUGIN(" parseCatchClause()", false);
    Node *body, *param, 
        *node = makeNode(true, true);

    expectKeyword("catch");

    expect("(");
    if (match(")")) {
        task->throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (strict && isRestrictedWord(param->name)) { 
        task->throwErrorTolerant(NULLPTRTKN, 
                           Messages[Mssg::StrictCatchVariable],{});
    }

    expect(")");
    body = parseBlock();
    node->finishCatchClause(param, body);
    DEBUGOUT("parseCatchClause", false);
    return node;
}

//throw_
Node* ParseTools::parseTryStatement(Node* node) {
    DEBUGIN(" parseTryStatement(Node node)", false);
    Node *block, *finalizer; 
    vector< Node* > handlers;

    finalizer = NULLNODE;

    expectKeyword("try");

    block = parseBlock();

    if (matchKeyword("catch")) {
#ifndef THROWABLE
        Node *tmp = parseCatchClause();
        handlers.push_back(tmp);
#endif
#ifdef THROWABLE
        handlers.push_back(parseCatchClause());
#endif
    }

    if (matchKeyword("finally")) {
        scanner.lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && finalizer == NULLNODE) {
        task->throwError(NULLPTRTKN, Messages[Mssg::NoCatchOrFinally], {});
    }

    node->finishTryStatement(block, EMPTY_NODE_LIST, 
                            handlers, finalizer); 
    DEBUGOUT("parseTryStatement", false);
    return node;
}

// 12.15 The debugger statement

//throw_
Node* ParseTools::parseDebuggerStatement(Node* node) {
    DEBUGIN(" parseDebuggerStatement(Node node)", false);
    expectKeyword("debugger");
    consumeSemicolon();
    node->finishDebuggerStatement();
    DEBUGOUT("parseDebuggerStatement", false);
    return node;
}

// 12 Statements

//#partial
//throw_
Node* ParseTools::parseStatement() {
    DEBUGIN(" parseStatement()", false);
    TknType type = lookahead->type;
    string key, tokval;
    Node *expr, *node, *labeledBody;

    if (type == TknType::EOFF) {
        task->throwUnexpected(lookahead);
    }

    if (type == TknType::Punctuator && lookahead->strvalue == "{") {
        return DBGRET("parseStatement", parseBlock());
    }
    
    node = makeNode(true, true);

    if (type == TknType::Punctuator) {
        tokval = lookahead->strvalue;
        if (tokval == ";") {
            node->unused();
            return DBGRET("parseStatement", parseEmptyStatement());
        } else if (tokval == "(") {
            return DBGRET("parseStatement", parseExpressionStatement(node));
        }
    } else if (type == TknType::Keyword) {
        tokval = lookahead->strvalue;
        if (tokval == "break") {
            return DBGRET("parseStatement", parseBreakStatement(node));
        } else if (tokval == "continue") {
            return DBGRET("parseStatement", parseContinueStatement(node));
        } else if (tokval == "debugger") {
            return DBGRET("parseStatement", parseDebuggerStatement(node));
        } else if (tokval == "do") {
            return DBGRET("parseStatement", parseDoWhileStatement(node));
        } else if (tokval == "for") {
            return DBGRET("parseStatement", parseForStatement(node));
        } else if (tokval == "function") {
            //#oddly enough in js passes node here.
            //#even though has no param even in js.
            return DBGRET("parseStatement", parseFunctionDeclaration());
        } else if (tokval == "if") {
            return DBGRET("parseStatement", parseIfStatement(node));
        } else if (tokval == "return") {
            return DBGRET("parseStatement", parseReturnStatement(node));
        } else if (tokval == "switch") {
            return DBGRET("parseStatement", parseSwitchStatement(node));
        } else if (tokval == "throw") {
            return DBGRET("parseStatement", parseThrowStatement(node));
        } else if (tokval == "try") {
            return DBGRET("parseStatement", parseTryStatement(node));
        } else if (tokval == "var") {
            return DBGRET("parseStatement", parseVariableStatement(node));
        } else if (tokval == "while") {
            return DBGRET("parseStatement", parseWhileStatement(node));
        } else if (tokval == "with") {
            return DBGRET("parseStatement", parseWithStatement(node));
        }
    }

    expr = parseExpression(); 

    // 12.12 Labelled Statements
    if ((expr->type == Syntax[Synt::Identifier]) && match(":")) {
        scanner.lex();

        key = "$";
        key.append(expr->name);

        if (hasSet(key, state.labelSet)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::Redeclaration], 
                       {"Label", expr->name}); 
        }
        state.labelSet.insert(key);
        labeledBody = parseStatement();
        state.labelSet.erase(key);
        node->finishLabeledStatement(expr, labeledBody);
        DEBUGOUT("parseStatement", false); 
        return node;
    }

    consumeSemicolon();

    node->finishExpressionStatement(expr);
    DEBUGOUT("parseStatement", false);
    return node;
}

// 13 Function Definition

//throw_
Node* ParseTools::parseFunctionSourceElements() {
    DEBUGIN(" parseFunctionSourceElements()", false);
    Node *sourceElement, 
        *node = makeNode(true, true);
    vector< Node* > sourceElements;
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;
    StateStruct oldstate;

    expect("{");

    firstRestricted->isNull = true;
    while (idx < length) {
        if (lookahead->type != TknType::StringLiteral) {
            break;
        }
        token = lookahead;

        sourceElement = parseSourceElement(); 
        //# returns in turn the value of parseStatement for stringLiteral 
        //# so returns a string literal expression node wrapped in an expressionStatement node.
        sourceElements.push_back(sourceElement); 
        if (GetStringCorrect(sourceElement->jv["expression"]["type"]) !=
            Syntax[Synt::Literal]) {
            //? this one I doubt there's more an efficient way to do this
            //? then json-c accesses. Storing node hierarchies just to fix this call seems to 
            //? be likely less performant.
            // this is not directive
            break;
        }
        directive = slice(sourceRaw, token->start + 1, token->end - 1);
        if (directive == u"use strict") {
            strict = true;
            if (!(firstRestricted->isNull)) {
                task->throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral], {});
            }
        } else {
            if (firstRestricted->isNull && token->octal) {
                firstRestricted = token;
                firstRestricted->isNull = false;
            }
        }
    }

    oldstate = state;

    state.labelSet.clear();
    state.inIteration = false;
    state.inSwitch = false;
    state.inFunctionBody = true;
    state.parenthesisCount = 0;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        sourceElement = parseSourceElement();
        if (sourceElement == NULLNODE) {
            break;
        }
        sourceElements.push_back(sourceElement);
    }

    expect("}");

    state.labelSet = oldstate.labelSet;
    state.inIteration = oldstate.inIteration;
    state.inSwitch = oldstate.inSwitch;
    state.inFunctionBody = oldstate.inFunctionBody;
    state.parenthesisCount = oldstate.parenthesisCount;

    node->finishBlockStatement(sourceElements);
    DEBUGOUT("parseFunctionSourceElements", false);
    return node;
}

//throw_ 
void ParseTools::validateParam(ParseParamsOptions& options, 
                               ptrTkn param, const string name) {
     DEBUGIN("validateParam", false);
     string key = "$";
     key.append(name);
     if (strict) {
         if (isRestrictedWord(name)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamName];
         }

         if (hasSet(key, options.paramSet)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     } else if (options.firstRestricted->isNull) {
         if (isRestrictedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamName];
         } else if (isStrictModeReservedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictReservedWord];
         } else if (hasSet(key, options.paramSet)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     }
     options.paramSet.insert(key);
    DEBUGOUT("validateParam", false);
    return;
}


//throw_ 
bool ParseTools::parseParam(ParseParamsOptions& options) {
    DEBUGIN(" parseParam(ParseParamsOptions options)", false);
    ptrTkn token; 
    Node *param, *def;

    token = lookahead;
    param = parseVariableIdentifier();
    //printf("token strvalue %s \n", (toU8string(token.strvalue)).data());
    //! MAJOR CONCERN: if a number is put where an argument is here,
    //! will that cause an ungraceful crash because validateParam is expecting
    //! a string so we cast it here, but the token grabbed might be
    //! a numeric literal?? if so perhaps the thing to do is check
    //! for token.type and if it is of numeric types cast it?
    validateParam(options, token, token->strvalue);

    if (match("=")) {
        scanner.lex();
        def = parseAssignmentExpression();
        ++(options.defaultCount);
        options.defaults.push_back(def);
    }

    options.params.push_back(param);

    return DBGRET("parseParam", !match(")"));
}

//throw_ 
ParseParamsOut ParseTools::parseParams(ptrTkn firstRestricted) {
    DEBUGIN(" parseParamS(ptrTkn firstRestricted)", false);
    ParseParamsOptions options;
    ParseParamsOut out;
    options.defaultCount = 0;
    options.firstRestricted = firstRestricted;
    if (options.stricted != 0x0)
        { options.stricted->isNull = true; }

    expect("(");

    if (!match(")")) {
        while (idx < length) {
            bool tmp = parseParam(options);
            if (!tmp) {
                break;
            }
            expect(",");
        }
    }

    expect(")");
    if (options.defaultCount == 0) {
        options.defaults.clear();
    }

    out.params = options.params;
    out.defaults = options.defaults;
    out.stricted = options.stricted;
    out.firstRestricted = options.firstRestricted;
    out.message = options.message;
    DEBUGOUT("parseParamS", false);
    return out;
}

//throw_ 
Node* ParseTools::parseFunctionDeclaration() {
    DEBUGIN(" parseFunctionDeclaration()", false);
    ptrTkn token, stricted, firstRestricted = scanner.makeToken();
    string message;
    Node *body, *id, 
        *node = makeNode(true, true);    
    ParseParamsOut tmp;
    vector< Node* > params;
    vector< Node* > defaults;
    bool previousStrict;
    id = NULLNODE;

    expectKeyword("function");

    token = lookahead;
    id = parseVariableIdentifier(); 

    firstRestricted->isNull = true;
    if (strict) {
        if (isRestrictedWord(token->strvalue)) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictFunctionName], {});
        }
    } else {
        if (isRestrictedWord(token->strvalue)) {
            firstRestricted = token;
            message = Messages[Mssg::StrictFunctionName];
        } else if (isStrictModeReservedWord(token->strvalue)) {
            firstRestricted = token;
            message = Messages[Mssg::StrictReservedWord];
        }
    }

    tmp = parseParams(firstRestricted);

    params = tmp.params;
    defaults = tmp.defaults;
    stricted = tmp.stricted;
    firstRestricted = tmp.firstRestricted;

    if (tmp.message != "") { //#TODO switch to hasMessage, 
        //# hasMessage being assigned to true on message assignment.
        //# so that if the message is an empty string we don't get 
        //# a false negative.
        message = tmp.message;
    }

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (strict && !(firstRestricted->isNull)) {
        task->throwError(firstRestricted, message, {});
    }
    if (strict && !(stricted->isNull)) {
        task->throwErrorTolerant(stricted, message, {});
    }
    strict = previousStrict;

    node->finishFunctionDeclaration(id, params, defaults, body);
    DEBUGOUT("parseFunctionDecl", false);
    return node;
} 

//throw_ 
Node* ParseTools::parseFunctionExpression() {
    DEBUGIN(" parseFunctionExpression()", false);
    ptrTkn token, firstRestricted = scanner.makeToken(), stricted;
    firstRestricted->isNull = true;
    string message, tokval;
    Node *body, *id;
    Node
        *node = makeNode(true, true);    
    ParseParamsOut tmp;
    vector< Node* > params;
    vector< Node* > defaults;
    bool previousStrict;

    id = NULLNODE;
    expectKeyword("function");

    if (!match("(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 

        tokval = token->strvalue;
        if (strict) {
            if (isRestrictedWord(tokval)) {
                task->throwErrorTolerant(token, 
                                   Messages[Mssg::StrictFunctionName],{});
            }
        } else {
            if (isRestrictedWord(tokval)) {
                firstRestricted = token;
                message = Messages[Mssg::StrictFunctionName];
            } else if (isStrictModeReservedWord(tokval)) {
                firstRestricted = token;
                message = Messages[Mssg::StrictReservedWord];
            }
        }
    }

    tmp = parseParams(firstRestricted); 
    params = tmp.params;
    defaults = tmp.defaults; 
    stricted = tmp.stricted;
    firstRestricted = tmp.firstRestricted;

    if (tmp.message != "") {
        message = tmp.message;
    }

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (strict && !(firstRestricted->isNull)) {
        task->throwError(firstRestricted, message, {});
    }

    if (strict && !(stricted->isNull)) {
        task->throwErrorTolerant(stricted, message, {});
    }
    strict = previousStrict;
    node->finishFunctionExpression(id, params, defaults, body);

    DEBUGOUT("parseFuncExpr", false);
    return node;
}

    // 14 Program

//throw_ 
Node* ParseTools::parseSourceElement() {
    DEBUGIN(" parseSourceElement()", false);
    string val;
    if (lookahead->type == TknType::Keyword) {
        val = lookahead->strvalue;
        if (val == "const" || val == "let") {
            return DBGRET("", parseConstLetDeclaration(val));
        } else if (val == "function") {
            return DBGRET("", parseFunctionDeclaration()); 
        } else {

            return DBGRET("", parseStatement());
        }
    }

    if (lookahead->type != TknType::EOFF) {
        return DBGRET("", parseStatement());
    }

    DEBUGOUT("parseSourceElement", false);
    return NULLNODE;
}

//throw_ 
vector< Node* > ParseTools::parseSourceElements() {
    DEBUGIN(" parseSourceElementS() ", false);
    Node *sourceElement;
    vector< Node* > sourceElements;
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;

    firstRestricted->isNull = true;
    while (idx < length) {
        token = lookahead;
        if (token->type != TknType::StringLiteral) {
            break;
        }

        sourceElement = parseSourceElement();
        sourceElements.push_back(sourceElement);
        //#todo make a function that accepts vector of nested finds
        //#so we can make tests like this more legible.

        if (GetStringCorrect(sourceElement->jv["expression"]["type"]) != 
            (Syntax[Synt::Literal])) {
            // this is not directive
            break;
        }
        directive = slice(sourceRaw, token->start + 1, token->end - 1);
        if (directive == u"use strict") {
            strict = true;

            if (!(firstRestricted->isNull)) { 

                task->throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral],{});
            }
        } else {
            if (firstRestricted->isNull && token->octal) {
                firstRestricted = token;
                firstRestricted->isNull = false; //#probably not neces.
            }
        }
    }

    while (idx < length) {
        sourceElement = parseSourceElement();

        if (sourceElement == NULLNODE) {
            break;
        }
        sourceElements.push_back(sourceElement);
    }

    DEBUGOUT("parseSourceElementS", false);
    return sourceElements;
}

//throw_ 
Node* ParseTools::parseProgram() {
    DEBUGIN(" parseProgram()", false);
    Node *node;
    vector< Node* > body;

    scanner.skipComment(); //ev
    scanner.peek();
    node = makeNode(true, true);
    strict = false;
    body = parseSourceElements();
    node->finishProgram(body);

    DEBUGOUT("parseProgram", false);
    return node;
}

void Tokenizer::filterTokenLocation() {
    return;
    /*    DEBUGIN(" filterTokenLocation()", false);
    TokenRecord token(0,0,0),entry;
    vector<TokenRecord> tokens;

    for (unsigned i = 0; i < extra.tokenRecords.size(); ++i) {
        entry = extra.tokenRecords[i];
        token.type = entry.type;
        token.valuestring = entry.valuestring;
        if (extra.range) { 
            token.range[0] = entry.range[0];
            token.range[1] = entry.range[1];
        }
        if (extra.loc) { 
            token.loc = entry.loc;
        }
        tokens.push_back(token);
    }
    extra.tokenRecords = tokens;

    DEBUGOUT("filterToken", false);    
    return; */
}


//# tokenize
//# returns a map containing under
//# (optional) 'comments' - list of comments as per esprima.
//# (optional) 'errors' - list of errors as per esprima
//# 'tokenlist' - list of tokens as per esprima

//# differences between this and esprima tokenize results:
//# -1. json hierarchy in esprima the tokenlist is the noderoot
//# -2. no js regex validation unless passed through a js environment 
//#    afterwards for validation with a tool like linprima-wrapfuncs.js


void Tokenizer::tokenize(Document &outJson, 
                    const bool retErrorsAsJson) {
    outJson.SetObject();
    AllocatorType& alloc = outJson.GetAllocator();
    extra.tokenize = true;    
    vector<TokenRecord> tokens;

    //range, loc, comment->CommentTracking and tolerant->errorTolerant
    //are the relevant/used options.

#ifndef THROWABLE
    ErrWrapint tmp = peek();
    if (tmp.err) {
        if (!extra.errorTolerant) {
            //json_object_put(outJson);
            if (errorType == 0) {
                retError.toJson(&extra, outJson, &alloc);
                return;
            }
            retAssertError.toJson(&extra, outJson, &alloc);
            return;
        }
    }
#endif
#ifdef THROWABLE
    peek();
#endif

    if (lookahead->type == TknType::EOFF) {
        vec2jsonCallback<TokenRecord>(outJson, &alloc, &extra,
                                      "tokenlist",
                                      extra.tokenRecords, 
                                      &TokenRecord::toJson);
         return;
    }

    lex();
    while (lookahead->type != TknType::EOFF) {
#ifndef THROWABLE
        ErrWrapptrTkn out = lex();
        if (out.err) { 
            if (extra.errorTolerant) {
                extra.errors.push_back(retError); 
                break;
            } else {
                if (errorType == 0) {
                    retError.toJson(&extra, outJson, &alloc);
                    return;
                }
                retAssertError.toJson(&extra, outJson, &alloc);
                return;
            }
        }
#endif
#ifdef THROWABLE
        try {
            lex();
        } catch (ExError& e) { 
            if (extra.errorTolerant) {
                extra.errors.push_back(e); 
                break;
            } else {
                if (retErrorsAsJson) {
                    //json_object_put(outJson);
                    e.toJson(&extra, outJson, &alloc);
                    return; 
                }
                throw e;
            }
        }
#endif
    }

    filterTokenLocation();
    vec2jsonCallback<TokenRecord>(outJson, &alloc, &extra,
                                  "tokenlist",
                                  extra.tokenRecords, 
                                  &TokenRecord::toJson); 
    if (extra.commentTracking) {
        vec2jsonCallback<Comment>(outJson, &alloc, &extra,
                                  "comments",
                                  extra.comments,
                                  &Comment::toJson);
    }
    if (extra.errorTolerant) {
        vec2jsonCallback<ExError>(outJson, &alloc, &extra,
                                  "errors",
                                  extra.errors,
                                  &ExError::toJsonTolerant);
    }
    extra.clear();
    return;
}

void tokenizeImpl(Document &outJson,
                  const u16string code, 
                  OptionsStruct options,
                  const bool retErrorsAsJson) { 
    options.tokens = true;
    options.tokenize = true;
    initglobals();
    Tokenizer tknr(code, options);
    tknr.tokenize(outJson, retErrorsAsJson);
    return;
}



    //? parenthesisCount for state not provided here normally as in parse. 
    //? That going to be a problem for us later?

    // Of course we collect tokens here.

    // The following two fields are necessary to compute the Regex tokens.


void tokenizeImpl(Document& d, 
                  const u16string code, const OptionsStruct options) { 
    tokenizeImpl(d, code, options, false);
}
void tokenizeImpl(Document &d,
                  const string code, const OptionsStruct options) { 
    tokenizeImpl(d, toU16string(code), options, false);
}
void tokenizeImpl(Document &d, const string code) { 
    OptionsStruct o;
    tokenizeImpl(d, toU16string(code), o, false);
}
void tokenizeImpl(Document &d, const u16string code) { 
    OptionsStruct o;
    tokenizeImpl(d, code, o, false);
}

string tokenizeRetString(const u16string code, const OptionsStruct options){
    
    Document out;
    tokenizeImpl(out, code, options, true);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    out.Accept(writer);    
    string result = buffer.GetString();
    return result;  
}
string tokenizeRetString(const string code, const OptionsStruct options) {
    return tokenizeRetString(toU16string(code), options);
}

//# Returns a map containing
//# (optional) 'comments' - list of comments as per esprima.
//# (optional) 'tokens' - list of tokens as per esprima
//# (optional) 'errors' - list of errors as per esprima
//# 'regex' - list of paths to any regex literals within the AST
//# 'program'
//# a Json-C representation of the Mozilla Parser API AST, with the following
//# differences:

//# Regex literals are represented as a pair of strings, the first the body, the second the flags.
//# Regex literals are not verified to be valid until passed through linprima-wrapfuncs.js' 
//#    json-to-js converter, unlike esprima where regexes are validated as part of the parsing api.
//# Before being passed through JSON.parse within a javascript environment, 
//#    numeric literals are represented as strings, serialized to json string using a special
//#    serializer that does not print quotes.


void ParseTools::parse(Document& outJson, 
                       const bool retErrorsAsJson) {
    outJson.SetObject();
    AllocatorType& alloclocal = outJson.GetAllocator();    
    alloc = &alloclocal;
    //relevant options are range, loc, attachComment
    //source, (hasSource also of course), and tracking and tolerance.
    //also attachComment implies range = true and commentTracking = true.
 
    Node *programNode;

#ifndef THROWABLE
    ErrWrapNodePtr tmp = parseProgram();
    if (tmp.err) {
        //pt.clearHeap();
        //json_object_put(programJson);
        if (errorType == 0) {
            retError.toJson(&extra, outJson, alloc);
            return;
        }
        retAssertError.toJson(&extra, outJson, alloc); 
        return;
    }
    programNode = tmp.val;
#endif
#ifdef THROWABLE
    try {
        programNode = parseProgram();
    } catch(ExError& e) {        
        //pt.clearHeap();
        if (retErrorsAsJson) {
            //json_object_put(programJson);
            e.toJson(&extra, outJson, alloc);
            return;
        }
        throw e;
    }
#endif
    outJson.AddMember("program", programNode->jv.Move(), *alloc);
    Value regexList(kArrayType);
    programNode->regexPaths2json(regexList);
    outJson.AddMember("regexp", 
                      regexList, 
                      *alloc);

   if (extra.commentTracking) {
       vec2jsonCallback<Comment>(outJson, alloc, &extra,
                                 "comments", extra.comments,
                                 &Comment::toJson); 
   }

   if (extra.tokenTracking) {
       scanner.filterTokenLocation();
       vec2jsonCallback<TokenRecord>(outJson, alloc, &extra,
                                     "tokens",
                                     extra.tokenRecords,
                                     &TokenRecord::toJson);
   }
   //   string s = "debug";
   //Value dbgval((int) extra.tokenRecords.size());
   //outJson.AddMember(Value(s.data(), s.length(), alloc).Move(),
   //                  dbgval, alloc);

   if (extra.errorTolerant) {
       vec2jsonCallback<ExError>(outJson, alloc, &extra,
                                 "errors",
                                 extra.errors,
                                 &ExError::toJsonTolerant);
   }

   extra.clear();
//programNode->delNode(programNode);
   return;
}
void parseImpl(Document &outJson,
               const u16string code, 
               OptionsStruct options, //# nonconst 1:1
               const bool retErrorsAsJson) { 
    initglobals();
    ParseTools pt(code, options);
    pt.parse(outJson, retErrorsAsJson);
    return;
};


void parseImpl(Document& d, const u16string code,
                OptionsStruct options) {    
    parseImpl(d, code, options, false);
}
void parseImpl(Document& d, const string code, 
                OptionsStruct options) {    
    parseImpl(d, toU16string(code), options, false);
}

void parseImpl(Document& d, const string code) { 
    OptionsStruct o;
    parseImpl(d, toU16string(code), o, false);
}

void parseImpl(Document& d, const u16string code) { 
    OptionsStruct o;
    parseImpl(d, code, o, false);
}


//# return json as string.
string parseRetString(const u16string code, OptionsStruct options) {    
    Document out;
    out.SetObject();
    parseImpl(out, code, options, true);
    //walkJson("root", out);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    out.Accept(writer); 
    string result = buffer.GetString();
    return result;  
}
string parseRetString(const string code,
                      OptionsStruct options) { 
    return parseRetString(toU16string(code), options);
}


char * outchars = new char[1];

//# not the prettiest solution, but it works for now.
//# maybe use smart pointer to free mem once returned?
//# it'd have to be compatible with c usage as regular pointer though.
//# fine if you have to increment it in a subcall.
char* strToChar(string in) {
    delete[] outchars;
    outchars = new char[in.size()+1];
    strcpy(outchars, in.c_str());
    return outchars;
}


extern "C" {
    char* tokenizeExtern(const char *code, const char* options) {
      return strToChar(tokenizeRetString(string(code), 
                                          OptionsStruct(options)));

    }
    char* parseExtern(const char *code,
                      const char* options) {
        return strToChar(parseRetString(string(code),
                                       OptionsStruct(
                                         options)));

    }

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
                        const char* options) {
        //printf("received string: =%s=\n", code);

            return strToChar(tokenizeRetString(
              toU16string(string(code)).substr(0,codelen), 
                                    OptionsStruct(options)));

    }
    char* parseASMJS(const char *code, int codelen, 
                      const char* options) {
        return strToChar(parseRetString(
                    toU16string(string(code)).substr(0, codelen), 
                                       OptionsStruct(options)));

    }

}

    //#include "profiler.h"

//#include <chrono>
//using std::chrono::system_clock;

#ifdef HASMAIN
#include <fstream>

int main() {
    string result, allopt;
    unsigned int resultlength = 0;
    
    string finput;
    ifstream ifs("/home/n/coding/esp3/bench/cases/active/Chart.js");
    //ifstream ifs("/home/n/coding/esp7/test/codetotest.js");

    finput.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );    

    vector<string> codeSamples = { finput };
    
    //vector<string> codeSamples = { 
    //   "var x = { null: 42 }" 
            //};
   
    //allopt = "{ 'tolerant':true }";
    allopt = "{ \"loc\":true, \"range\":true, \"tokens\":true }";
    //    ProfilerStart("/tmp/profile2");

    //system_clock::time_point begin = system_clock::now();
    int reps = 10;
    for (int j = 0; j<reps; j++) {
        for (unsigned int i=0; i<codeSamples.size(); i++){ 
           //result = string(tokenizeRetString(
           result = string(parseRetString(
                                           toU16string(codeSamples[i]),
                                           OptionsStruct(allopt.data())));
            resultlength += result.length() % 6;
        }
    }
    codeSamples.clear();
    finput = "";
    
    //system_clock::time_point end = system_clock::now();
    //auto timediff = end - begin;
    //int millis = std::chrono::duration_cast<std::chrono::milliseconds>(timediff).count();
    //printf("milliseconds: %i\n", (int) ((double) millis / (double) reps));
    //    ProfilerStop();
    printf("total length %u\n", resultlength);

    //printf("last result %s\n", result.data());
}
#endif
