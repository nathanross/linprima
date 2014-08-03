#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <memory>
#ifdef THROWABLE
#include <exception>
#endif

//#include "json-c/json.h"
//#include "json-c/json_object.h"
//#include "json-c/printbuf.h"
//algorithm is for find(vector.begin()
using namespace std;

int debuglevel = 1;
vector<string> stackfuncs;

static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

#if defined DO_DEBUG
bool DEBUG_ON= (bool) 1;
#else
bool DEBUG_ON= (bool) 0;
#endif
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

void DEBUGIN(string in, bool lowprio) {    
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
void DEBUGIN(string in) {
    DEBUGIN(in, false);
}

void DEBUGOUT(string in, bool lowprio) {
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
 void DEBUGOUT(string in) {
     DEBUGOUT(in, false);
 }

 void DEBUGOUT() {
     DEBUGOUT("", false);
 }

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

string toU8string(const u16string input){ 
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;
  return myconv.to_bytes(input);
}
string toU8(const u16string input){ 
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;
  return myconv.to_bytes(input);
}

string toU8(const char16_t* input) {
  return toU8(u16string(input));
}
string toU8string(const char16_t* input) {
  return toU8string(u16string(input));
}
wstring toWstring(const string input) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(input);
}

u16string toU16string(const string input){ 
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;

 return myconv.from_bytes(input);
}


//json_put_dbl: pass in the string you want in the json, and it'll get put in there without the quotes.
// assumes you're putting in a safe number that won't mess up json validity.

extern int doubleSerializer(struct json_object *jso, 
                            struct printbuf *pb, int level, int flags) {

    sprintbuf(pb, json_object_get_string(jso));
   return 0;
}

inline json_object* json_newint(int in) 
{ return json_object_new_double(in); };

inline json_object* json_newstr(string in) 
{ return  json_object_new_string(in.data()); };

inline json_object* json_newbool(bool in) 
{ return json_object_new_boolean(in?1:0); };

inline json_object* json_newmap()
{ return json_object_new_object(); };

inline json_object* json_newarr() 
{ return json_object_new_array(); };

json_object*  json_newdbl(const char * in) { 
    json_object * tmp = json_object_new_string(in);
    json_object_set_serializer(tmp, doubleSerializer, 
                               0x0, json_object_free_userdata);
   return tmp;
}

json_object* json_put_dbl(json_object *a, const char *b, string c) { 
    json_object *tmp = json_object_new_string(c.data());
    json_object_object_add(a, b, tmp);
    json_object_set_serializer(tmp, doubleSerializer, 
                               0x0, json_object_free_userdata);
   return tmp;
}
inline json_object* json_put_dbl(json_object *a, const char *b, u16string c) { return json_put_dbl(a,b,toU8string(c)); }

json_object* json_push_dbl(json_object *a, string c) { 
    json_object *tmp = json_object_new_string(c.data());
    json_object_array_add(a, tmp);
    json_object_set_serializer(tmp, doubleSerializer, 
                               0x0, json_object_free_userdata);
   return tmp;
}
inline json_object* json_push_dbl(json_object *a, const char *b, u16string c) { return json_push_dbl(a,toU8string(c)); }

json_object* json_put_newmap(json_object *a, const char *b) { 
    json_object *tmp = json_object_new_object();
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_push_newmap(json_object *a) { 
    json_object *tmp = json_object_new_object();
    json_object_array_add(a, tmp);
   return tmp;
}
json_object* json_put_newarr(json_object *a, const char *b) { 
    json_object *tmp = json_object_new_array();
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_push_newarr(json_object *a) { 
    json_object *tmp = json_object_new_array();
    json_object_array_add(a, tmp);
   return tmp;
}

void json_put_null(json_object *a, const char *b) { 
    json_object_object_add(a, b, NULL);    
}
void json_push_null(json_object *a) { 
    json_object_array_add(a, NULL);

}

 //we keep int separate in case we want to implement a separate call method for double
 //it allows to keep easy to track which calls we'll actually have to change.
json_object* json_put(json_object * a, const char* b, string c) { 
    json_object *tmp = json_object_new_string(c.data());
    json_object_object_add(a, b, tmp);
   return tmp;
}
inline json_object* json_put(json_object *a, const char *b, const u16string c) 
{ return json_put(a,b,toU8string(c)); }

json_object* json_put(json_object *a, const char *b, const int c) { 
    json_object *tmp = json_object_new_double(c);
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_put(json_object *a, const char *b, const bool c) { 
    json_object *tmp = json_object_new_boolean(c?1:0);
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_put(json_object *a, const char *b, json_object* c) { 
    json_object_object_add(a, b, c);
   return c;
}

json_object* json_push(json_object *a, const string c) { 
    json_object *tmp = json_object_new_string(c.data());
    json_object_array_add(a, tmp);
   return tmp;
}
inline json_object* json_push(json_object *a, const u16string c) { return json_push(a,toU8string(c)); }
json_object* json_push(json_object *a, const int c) { 
    json_object *tmp = json_object_new_double(c);
    json_object_array_add(a, tmp);
   return tmp;
}
json_object* json_push(json_object *a, const bool c) { 
    json_object *tmp = json_object_new_boolean(c?1:0);
    json_object_array_add(a, tmp);
   return tmp;
}
json_object* json_push(json_object *a, json_object *c) { 
    json_object_array_add(a, c);
   return c;
}

//# incrRef increases the reference count of the json_object
//# returned. This means you're essentially saying 'keep this
//# object around, even if the parent json object containing it gets
//# freed' via json_object_put called on it or some parent of it,
//# or parent of that, etc.

//# the consequence of that is that later you need to free 
//# this retrieved object individually via json_object_put
//# to decrease the refcount to 0 and have libjson-c free it.
//# otherwise you'll have a memory leak. best policy
//# for primitives is just find json objects and get their values 
//# then return the values not the object, and when possible
//# for objects and arrays, don't increment, make all necessary
//# reads and writes in scope find is called.

//# again, if you return the json out of the function find is 
//# called in, the object will still exist until put is called
//# on a parent, but it makes for spaghetti freeing.

bool json_find(json_object *root, const char * eqkey, 
               json_object*& out, const bool incrRef, 
               const json_type eqType) { 
    json_object_object_foreach(root, jkey, jval) {
        if (strcmp(jkey, eqkey) == 0) {
            if (json_object_is_type(jval, eqType)) {
                if (incrRef) { out = json_object_get(jval); }
                else { out = jval; }
                return true;
            }
            return false;
        }
    }    
    return false;
    /*    bool exists;
    json_object ** value =0x0;

    exists = json_object_object_get_ex(root, eqkey, value);

    if (! exists || 
        !(json_object_is_type(*value, eqType))) {
        return false;
    }    

    out = *value; 
    if (incrRef) { json_object_get(out); }
    return true; */
}

//# note: there is no json_any_type type
bool json_find(json_object *root, const char* eqkey,
                       json_object*& out, const bool incrRef) {

    json_object_object_foreach(root, jkey, jval) {
        if (strcmp(jkey, eqkey) == 0) {
            if (incrRef) { out = json_object_get(jval); }
            else { out = jval; }
            return true;
        }
    }    
    return false; 

    /*bool exists;
    json_object ** value = 0x0;
    exists = json_object_object_get_ex(root, eqkey, value);
    if ( !exists ) {
        return false;
    }

    out = *value; 
    if (incrRef) { json_object_get(out); }
    return true; */
}

json_object* json_require(json_object *a, const char* eqkey,
                          const bool incrRef) { 
    json_object * out;
    bool exists = json_find(a, eqkey, out, incrRef);
    if (!exists) {
        string errormsg = "json_find failed to find key : ";
        errormsg.append(string(eqkey));
        throw runtime_error(errormsg);
    }
   return out;
}

json_object* json_arrfind(json_object* a, bool incrRef, const int idx) { 
    if (idx >= json_object_array_length(a)) {
        throw runtime_error("json_arrIdx was asked to find an index outside of the array bounds");
    }
    json_object * result = json_object_array_get_idx(a, idx);
    if (incrRef) { return json_object_get(result); }
    return result;
}

void json_arrput(json_object* a, const int idx, json_object *c) {        
    if (idx > json_object_array_length(a)) {
        throw runtime_error("json_arrIdx was asked to set an index higher than array length");
    }
    json_object_array_put_idx(a, idx, c); 
}

void json_del(json_object *a, const char* key) { 
    if (json_object_object_get_ex(a, key, NULL)) {
        json_object_object_del(a, key);
    }

}

// we could make this generic, but practically it's only valid for overrides of json_push
// that 
template <typename T>
json_object* vec2json(vector<T> in) { //only practically valid for vectors of ints and strings.
    //DEBUGIN("vec2json");
    json_object * arr = json_object_new_array();
    for (int i=0; i<in.size(); i++) {
        json_push(arr, in[i]);
    }
    //DEBUGOUT("", false); 
    return arr;
}

template <typename T>
json_object* vec2jsonCallback(vector<T> in,
                              function<json_object*(T&)> const& f) {
    //DEBUGIN("vec2JsonCallback");
    json_object * arr = json_object_new_array();
    for (int i=0; i<in.size(); i++) {
        json_push(arr, f(in[i]));
    }
    //DEBUGOUT("", false); 
    return arr;
}

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
bool has(const u16string needle, const unordered_set<u16string> haystack) {
    return (haystack.find(needle) != haystack.end());
}
bool has(const string needle, const unordered_set<string> haystack) {
    return (haystack.find(needle) != haystack.end());
}
bool has(const int needle, const unordered_set<int> haystack) {
    return (haystack.find(needle) != haystack.end());
}

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


u16string slice(const char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
    const char16_t * startptr = arr + start;    
    return u16string(startptr, (end-start));
}

void append(u16string &base, char16_t tail) { 
    base.append(u16string({tail})); 
    //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.

}

int parseInt(u16string in_u16, int radix) {  // !!!
    const int zero = 48;
    const int upperA = 65;
    const int lowerA = 97;

    string in = toU8string(in_u16);
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
    DEBUGIN("sciNoteToDouble");
    char current;
    string factor = "0";
    string exp = "0";
    bool dotPassed = false, esignPassed = false;
    for (int i=0;i<in.length();i++) {
        current = in[i];
        if (current == u'.') {
            dotPassed = true;
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

    DEBUGOUT("sciNoteToDouble");
    return stod(factor) * pow(10,stod(exp));
}

const char16_t NULL_CHAR16 = u'X';


// -----------------------
int idx;
int lineNumber;
int lineStart;
int length;
bool strict = false; //? remove initialization?



struct RegexHalf {
    string value;
    string literal;
#ifndef THROWABLE
    bool err;
#endif
    int start;
    int end;
    RegexHalf() { 
#ifndef THROWABLE
        err = false;
#endif
        start = -1; 
        end = -1; 
    }
};

struct Position {
    int line;
    int column;

    Position() {
        DEBUGIN("Position()", true);
        line = lineNumber;
        column = idx - lineStart;
        DEBUGOUT("Position()", true);
    }
};

struct Loc { 
    //aka SourceLocation
    Position start;
    Position end;
    bool hasSource;
    string source;

    Loc() {
        DEBUGIN("Loc()", true);
        this->end.line = -1;
        this->end.column = -1;
        this->hasSource = false;
        this->source = "";
        DEBUGOUT("loc()", true);
    }
};

 
json_object*  posToJson(Position p) {
    //DEBUGIN(" posToJson(Position p)");
    json_object * root = json_newmap();
    json_put(root, "line", p.line);
    json_put(root, "column", p.column);
    //DEBUGOUT("posToJSon(Position)"); 
    return root;
}

json_object*  locToJson(Loc l) { 
    //DEBUGIN(" locToJson(Loc l)");
    json_object * root = json_newmap();
    json_put(root, "start", posToJson(l.start));
    if (l.end.line != -1) {
        json_put(root, "end", posToJson(l.end));
    }
    if (l.hasSource) {
        json_put(root, "source", l.source);
    }
    //DEBUGOUT("locToJson"); 
    return root;
}

struct Comment {
    string type;
    string value;
    int range[2];
    Loc loc;
    Comment() {
        //DEBUGIN("Comment()");
        this->type = -1;
        this->value = "";
        this->range[0] = -1;
        this->range[1] = -1;
        //DEBUGOUT("Comment()");
    }
    json_object * toJson();
};

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
    json_object * nodesJv;
    int range[2];
    bool isNull;
    NodesComments() {
        nodesJv = 0x0;
        isNull = false;
        range[0] = -1;
        range[1] = -1;
        leadingComments.clear();
        trailingComments.clear();
    }
    NodesComments(json_object * jv) {
        this->nodesJv = jv;
        isNull = false;
        range[0] = -1;
        range[1] = -1;
        leadingComments.clear();
        trailingComments.clear();
    }
    void commentsIntoJson(const bool leading) { 
        //DEBUGIN(" NodesComments::commentsIntoJson(const bool leading)");
        string key;
        vector<Comment> * commentVec;
        if (leading) {
            key = "leadingComments";
            commentVec = &leadingComments;
        } else {
            key = "trailingComments";
            commentVec = &trailingComments;
        }
        if (commentVec->size() > 0) {
            json_put(nodesJv, key.data(), 
                     vec2jsonCallback<Comment>(*commentVec,
                                               &Comment::toJson));
        } else {

            json_del(nodesJv, key.data());
        }
        //DEBUGOUT("commentsIntoJSon");
    }
};

//# called ExError to prevent forseeable 
//# exception-handling namespace conflict.
#ifndef THROWABLE
class ExError {
#endif
#ifdef THROWABLE
class ExError : public exception {
#endif
public:
    string description;
    int index;
    int lineNumber;
    int column;
    ExError() {
        description = "unknown error";
        index = 0;
        lineNumber = 0;
        column = 0;
    }
    json_object * toJson() {
        DEBUGIN("Error::toJSON");
        json_object * root = json_newmap();
        json_put(root, "isError", true);
        json_put(root, "description", description);
        json_put(root, "index", this->index);
        json_put(root, "lineNumber", this->lineNumber);
        json_put(root, "column", this->column);
        DEBUGOUT("Error::toJSON"); 
        return root;
    }
    json_object * toJsonTolerant() {
        DEBUGIN("Error::toJSON");
        json_object * root = json_newmap();
        json_put(root, "description", description);
        json_put(root, "index", this->index);
        json_put(root, "lineNumber", this->lineNumber);
        json_put(root, "column", this->column);
        DEBUGOUT("Error::toJSON"); 
        return root;
    }
};

#ifndef THROWABLE

ExError retError;
int errorType = 0;

class AssertError {
public:
    string description;
    AssertError() {
        description = "";
    }
    json_object * toJson() {
        json_object * root = json_newmap();
        json_put(root, "message", description);
        json_put(root, "isAssert", true);
        return root;
    }
};

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
    TokenStruct() {
        DEBUGIN("TokenStruct()", true);
#ifndef THROWABLE
        err = false;
#endif
        isNull = false;
        type = TknType::None;
        lineNumber = -1;
        lineStart = -1;
        startLineNumber = -1;
        startLineStart = -1;
        start = -1;
        end = -1;
        octal=false; 
        range[0] = -1;
        range[1] = -1;
        DEBUGOUT("TokenStruct()", true);
    }
    ~TokenStruct() {
    }
};

//stored in extra.tokens
struct TokenRecord {
    Loc loc;
    int range[2];
    string valuestring;
    string typestring;
    TokenRecord() {
        DEBUGIN("TokenRecord()", true);
        range[0] = -1;
        range[1] = -1;
        DEBUGOUT("TokenRecord()", true);
    }
    json_object * toJson();
};

enum class Synt;
class Node {
public:
    bool isNull;
    string type;

    bool hasJv;
    json_object * jv;
    Loc loc;
#ifndef THROWABLE
    bool err;
#endif
    bool hasLoc;
    bool hasRange;
    int range[2];
    vector< vector<string> > regexPaths; //lin only. obv.



    string name;//for identifiers
    vector< Node > expressions; //for sequence expressions.
    shared_ptr<Node> left; //for assignment+reinterpretAsCover...
    shared_ptr<Node> right; //same

    string s(const u16string in);

    Node();
    Node(bool lookaheadAvail, bool storeStats);
    json_object* toJson();
    void lookavailInit();
    void clear();
    void unused();
    void jvput(const string path, const string b);
    void jvput(const string path, const int b); 
    void jvput(const string path, const bool b);
    void jvput_dbl(const string path, const string b);
    void jvput_null(const string path); 
    void regNoadd(const vector<string> paths, 
                  const Node&child);
    void reg(const string path, 
             const Node& child);
    void nodeVec(const string path, 
                 const vector<Node>& nodes);
    void addType(const Synt in);
    json_object* regexPaths2json();
    void commentsIntoJson(const bool leading);
    void processComment();
    void finish();
    void finishArrayExpression(const vector< Node >& elements);
    void finishArrowFunctionExpression(const vector< Node >& params, 
                                       const vector< Node >& defaults, 
                                       const Node& body, 
                                       const bool expression);
    void finishAssignmentExpression(const string oper,
                                    const Node& left, 
                                    const Node& right);
    void finishBinaryExpression(const string oper, 
                                const Node& left, 
                                const Node& right);
    void finishBlockStatement(const vector< Node >& body);
    void finishBreakStatement(const Node& label);
    void finishCallExpression(const Node& callee, 
                              const vector< Node >& args);
    void finishCatchClause(const Node& param, 
                           const Node& body);
    void finishConditionalExpression(const Node& test,
                                     const Node& consequent, 
                                     const Node& alternate);
    void finishContinueStatement(const Node& label);
    void finishDebuggerStatement();
    void finishDoWhileStatement(const Node& body,
                                const Node& test);
    void finishEmptyStatement();
    void finishExpressionStatement(const Node expression);
    void finishForStatement(const Node& init, 
                            const Node& test, 
                            const Node& update, 
                            const Node& body);
    void finishForInStatement(const Node& left, 
                              const Node& right, 
                              const Node& body);
    void finishFunctionDeclaration(const Node& id, 
                                   const vector< Node >& params, 
                                   const vector< Node >& defaults, 
                                   const Node& body);
    void finishFunctionExpression(const Node& id, 
                                  const vector< Node >& params, 
                                  const vector< Node >& defaults, 
                                  const Node& body);
    void finishIdentifier(const string name);
    void finishIfStatement(const Node& test, 
                           const Node& consequent, 
                           const Node& alternate); 
    void finishLabeledStatement(const Node& label, 
                                const Node& body);
    void finishLiteral(TokenStruct& token);
    void finishMemberExpression(const char16_t accessor, 
                                const Node& object, 
                                const Node& property);
    void finishNewExpression(const Node& callee, 
                             const vector<Node>& args);
    void finishObjectExpression(const vector<Node>& properties);
    void finishPostfixExpression(const string oper, 
                                 const Node& argument);
    void finishProgram(const vector<Node>& body);
    void finishProperty(const string kind, 
                        const Node& key, 
                        const Node& value);
    void finishReturnStatement(const Node& argument);
    void finishSequenceExpression(const vector<Node>& expressions);
    void finishSwitchCase(const Node& test, 
                          const vector<Node>& consequent);
    void finishSwitchStatement(const Node& discriminant, 
                               const vector<Node>& cases);
    void finishThisExpression();
    void finishThrowStatement(const Node& argument);
    void finishTryStatement(const Node& block, 
                            const vector<Node>& guardedHandlers, 
                            const vector<Node>& handlers, 
                            const Node& finalizer);
    void finishUnaryExpression(const string oper, 
                               const Node& argument);
    void finishVariableDeclaration(const vector<Node>& declarations, 
                                   const string kind);
    void finishVariableDeclarator(const Node& id, 
                                  const Node& init);
    void finishWhileStatement(const Node& test,
                              const Node& body);
    void finishWithStatement(const Node& object, 
                             const Node& body);

}; 

#ifndef THROWABLE
class ErrWrapvectorNode {
public:
    bool err;
    vector<Node> val;
    ErrWrapvectorNode() {
        err = false;
    }
    ErrWrapvectorNode(vector<Node> in) {
        val = in;
        err = false;
    }
};
#endif

TokenStruct NULLTOKEN;
Node NULLNODE(false,false);
struct ExtraStruct {
    //# port-specific member to replace "if (extra.tokens)"
    bool tokenTracking;
    vector<TokenRecord> tokenRecords; //called extra.tokens in esprima
    // name changed here to distinguish specific type and different domains
    // of these types.

    bool hasSource;
    string source; 

    bool tokenize;
    int openParenToken;
    int openCurlyToken;

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
    vector< NodesComments > bottomRightStack; //! todo Node header text.

    ExtraStruct() {
        tokenize = false;
        errorTolerant = false;
        attachComment = false;
        tokenRecords.clear();
        comments.clear();
        errors.clear();
        leadingComments.clear();
        trailingComments.clear();

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
};

struct OptionsStruct {
    bool range;
    bool loc;
    bool comment;
    bool tolerant;
    bool attachComment;
    bool tokens;
    bool hasSource;
    string source;
    OptionsStruct() {
        DEBUGIN("OptionsStruct()", true);
        range = false;
        loc = false;
        comment = false;
        tolerant = false;
        attachComment = false;
        tokens = false;
        hasSource = false;
        DEBUGOUT("OptionsStruct()", true);
    }
    bool json_getbool(json_object* in, 
                      const string key, 
                      const bool defaultVal) {
        json_object* tmp;
        bool exists = json_find(in, key.data(), tmp,
                                false, json_type_boolean);
        if (!exists) { return defaultVal; }
        bool result = (bool) json_object_get_boolean(tmp);
        return result;
    }
    OptionsStruct(const char *in_o) {
        DEBUGIN("OptionsStruct(char*)", true);
        json_tokener_error tkErr;
        json_object *in = json_tokener_parse_verbose(in_o, &tkErr);
        if (tkErr != json_tokener_success) {
            //#I don't think this will ever come up outside of manual
            //# debugging unless there's some serious encoding error.
            DEBUGIN("failed to parse options string provided");
            range = false;
            loc = false;
            comment = false;
            tolerant = false;
            attachComment = false;
            tokens = false;
            hasSource = false;
        } else { 
            json_object* tmp;
            range = json_getbool(in, "range", false);
            loc = json_getbool(in, "loc", false);
            attachComment = json_getbool(in, "attachComment", false);
            comment = json_getbool(in, "comment", false);
            tolerant = json_getbool(in, "tolerant", false);
            tokens = json_getbool(in, "tokens", false);
            hasSource = json_find(in, "source", tmp,
                                  false, json_type_string);
            if (hasSource) { 
                source = json_object_get_string(tmp); 
            }
        }
        json_object_put(in);
        DEBUGOUT("OptionsStruct(char*)", true);
    }
};


struct ParseParamsOptions {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    TokenStruct firstRestricted;
    TokenStruct stricted;
    unordered_set<string> paramSet;
    string message;
    ParseParamsOptions() {
        firstRestricted.isNull = true;
        stricted.isNull = true;
    }
};



struct ParseParamsOut {
#ifndef THROWABLE
    bool err;
#endif
    TokenStruct firstRestricted;
    TokenStruct stricted;
    string message;
    vector< Node > params;
    vector< Node > defaults;
    ParseParamsOut() {
#ifndef THROWABLE
        err = false;
#endif
        message="";
        stricted.isNull = true;
        firstRestricted.isNull = true;
    }
};


struct ReinterpretOptions {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    Node firstRestricted;
    Node stricted;
    unordered_set<string> paramSet;
    string message;
    ReinterpretOptions() {
        firstRestricted = NULLNODE; //?
        stricted = NULLNODE; //? nec.? had it only in reinOut before.
        //? not sure if context will make it different.
    }
};

struct ReinterpretOut {
    Node firstRestricted;
    Node stricted;
    bool isNull;
    string message;
    vector< Node > params;
    vector< Node > defaults;
#ifndef THROWABLE
    bool err;
#endif
    void* rest; //seems to be a dummy var?
    ReinterpretOut() {
#ifndef THROWABLE
        err = false;
#endif
        isNull=false;
        firstRestricted = NULLNODE;
        stricted = NULLNODE;
    }
};

//---- ----------  -----------------------------
// signatures (temporary until we set up a header file):

//throw_
void throwError(TokenStruct& token, const string messageFormat, 
                vector< string > args);
//throw_
void throwErrorTolerant(TokenStruct& token, const string messageFormat, 
                        vector< string > args);
//throw_
void throwUnexpected(TokenStruct& token);

template<typename T> T DEBUGRET(string a, T b) { DEBUGOUT(a); return b; }

OptionsStruct options;
ExtraStruct extra;
StateStruct state;
TokenStruct lookahead;

const char16_t * sourceraw;
char16_t source(int idx) { return *(sourceraw + idx); }


json_object*  TokenRecord::toJson() {
    DEBUGIN(" TokenRecord::toJson()");
    json_object *root = json_newmap(), *rangearr;
    json_put(root, "type", this->typestring);
    json_put(root, "value", this->valuestring);
    if (extra.range) {
        rangearr = json_newarr();
        json_push(rangearr, this->range[0]);
        json_push(rangearr, this->range[1]);
        json_put(root, "range", rangearr);
    }
    if (extra.loc) {
        json_put(root, "loc", locToJson(this->loc));
    }

    DEBUGOUT("TokenRecord::toJson()");
    return root;
}

json_object * Comment::toJson() {
    DEBUGIN("Comment::toJson");
    json_object *root = json_newmap(), *rangearr;
    json_put(root, "type", this->type);
    json_put(root, "value", this->value);
    if (extra.range) {
        rangearr = json_newarr();
        json_push(rangearr, this->range[0]);
        json_push(rangearr, this->range[1]);
        json_put(root, "range", rangearr);
    }
    if (extra.loc) {
        json_put(root, "loc", locToJson(this->loc));
    }
    DEBUGOUT("comment::toJson");
    return root;
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

    /* map<string, int> Token = {
    {"BooleanLiteral", 1},
    {"EOF", 2},
    {"Identifier", 3},
    {"Keyword", 4},
    {"NullLiteral", 5},
    {"NumericLiteral", 6},
    {"Punctuator", 7},
    {"StringLiteral", 8},
    {"RegularExpression", 9}
}; */

//#todo: investigate turning this into an enum'd class
//#to avoid adding unsightly static casts everywhere used,
//#will have to change member .type in TokenStruct, and likely
//#skim over all instances and ways that type is used,
//#example check if it's ever passed to a lh variable that is
//#not the .type member of TokenStruct

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


unordered_set< string > FnExprTokens = {
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

 map<string, Node> PlaceHolders { 
     {"ArrowParameterPlaceHolder", Node()} 
 };
 //map<string, int> 


 map<string, int> PropertyKind = {
     {"Data", 1},
     {"Get", 2},
     {"Set", 4}
 };

 // Error messages should be identical to V8.
 enum class Mssg {
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
    PlaceHolders["ArrowParameterPlaceHolder"].type="ArrowParameterPlaceholder";
    NULLTOKEN.isNull = true;
    NULLNODE.isNull = true;
    DEBUGOUT("", true);
}

  // Ensure the condition is true, otherwise throw an error.
  // This is only to have a better contract semantic, i.e. another safety net
  // to catch a logic error. The condition shall be fulfilled in normal case.
  // Do NOT use this to enforce a certain condition on any user input.



#ifndef THROWABLE
//throw_
int softAssert(const bool condition, const string message) {
    DEBUGIN(" assert(bool condition, string message)");

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
    DEBUGIN(" assert(bool condition, string message)");

    string providedMessage = "ASSERT: ";
    providedMessage.append(message);
    if (! condition)
        { throw runtime_error(providedMessage); }
    DEBUGOUT("", false);
 }

#endif

bool isDecimalDigit(const char16_t ch) {
    DEBUGIN("   isDecimalDigit(const char16_t ch)");
    DEBUGOUT("", false); 
    return (ch >= 0x30 && ch <= 0x39); //0..9
}

bool isHexDigit(const char16_t ch) {
    DEBUGIN("   isHexDigit(const char16_t ch)");
    DEBUGOUT("", false); 
    return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) 
           != std::string::npos);    
}

bool isOctalDigit(const char16_t ch) {
    DEBUGIN("   isOctalDigit(const char16_t ch)");
    DEBUGOUT("", false); 
    return (u16string({u"01234567"}).find_first_of(ch) 
            != std::string::npos);    
}

char16_t toLowercaseHex(const char16_t ch) { //used in scanHexEscape
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
bool isWhiteSpace(const char16_t ch) {
    DEBUGIN("   isWhiteSpace(const char16_t ch)");
    DEBUGOUT("", false); 
    return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || (ch >= 0x1680 && has(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators

bool isLineTerminator(const char16_t ch) {
    DEBUGIN("   isLineTerminator(const char16_t ch)");
    DEBUGOUT("", false); 
    return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}

 // 7.6 Identifier Names and Identifiers
bool intervalarr_contains(unsigned int key, vector< vector< unsigned int > > * arr) {
    vector< unsigned int > *range_starts = &(arr->at(0)),
        *range_ends = &(arr->at(1));
    int pos = lower_bound(range_starts->begin(), 
                          range_starts->end(), key) - range_starts->begin();
    if (range_starts->at(pos) > key) { 
        if (pos == 0) { return false; } pos--; 
    }
    return (key <= range_ends->at(pos) 
                   || (pos+1 < range_starts->size() 
                       && key == range_starts->at(pos+1)));
}


 bool isIdentifierStart(const char16_t ch) {
     DEBUGIN("   isIdentifierStart(const char16_t ch)");
     DEBUGOUT("", false); 
     return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch == 0x5C) ||                      // \ (backslash)
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                               &nonasciiIdentifierstart));
 } 

 bool isIdentifierPart(const char16_t ch) {
     DEBUGIN("   isIdentifierPart(const char16_t ch)");
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

 bool isFutureReservedWord(const string id) {
     DEBUGIN("   isFutureReservedWord(const u16string id)");
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

 bool isStrictModeReservedWord(const string id) {
     DEBUGIN("   isStrictModeReservedWord(const u16string id)");
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

 bool isRestrictedWord(const string id) {
     DEBUGIN("   isRestrictedWord(const u16string id)");
     DEBUGOUT("", false);
     return (id == "eval" || id == "arguments");
 }

 // 7.6.1.1 Keywords
 bool isKeyword(const string id) {
     DEBUGIN("   isKeyword(const u16string id)");
     if (strict && isStrictModeReservedWord(id)) { 
         DEBUGOUT("", false); 
         return true;
     }

     // 'const' is specialized as Keyword in V8.
     // 'yield' and 'let' are for compatiblity with SpiderMonkey and ES.next.
     // Some others are from future reserved words.

     switch (id.length()) {
         case 2:
             DEBUGOUT("", false); 
             return (id == "if") || (id == "in") || (id == "do");
         case 3:
             DEBUGOUT("", false); 
             return has(id, 
                        { "var", "for", "new", "try", "let"});
         case 4:
             DEBUGOUT("", false); 
             return has(id, 
                        { "this", "else", "case", "void", "with", "enum"});
         case 5:
             DEBUGOUT("", false); 
             return has(id, 
                        {"while", "break", "catch", "throw", 
                                "const", "yield", "class", "super"});
         case 6:
             DEBUGOUT("", false); 
             return has(id, 
                        {"return", "typeof", "delete",
                         "switch", "export", "import"});
         case 7:
             DEBUGOUT("", false); 
             return (id == "default") || 
                 (id == "finally") || (id == "extends");
         case 8:
             DEBUGOUT("", false); 
             return (id == "function") || 
                 (id == "continue") || (id == "debugger");
         case 10:
             DEBUGOUT("", false); 
             return (id == "instanceof");
         default:
             DEBUGOUT("", false); 
             return false;
     }
 }

 // 7.4 Comments


//# only called if extra.commentTracking
void addComment(const string type, const string value, 
                 const int start, const int end, const Loc loc) {
    DEBUGIN(" addComment(u16string type, u16string value, int start, int end, Loc loc)");
     Comment comment;

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



void skipSingleLineComment(const int offset) {
    DEBUGIN(" skipSingleLineComment(const int offset)");
    int start;
    Loc loc; 
    char16_t ch;
    string comment;
    
    start = idx - offset;
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart - offset;

    while (idx < length) {
        ch = source(idx);
        ++idx;
        if (isLineTerminator(ch)) {
            if (extra.commentTracking) {
                comment = toU8(slice(sourceraw, start + offset, idx-1));
                loc.end.line = lineNumber;
                loc.end.column = idx - lineStart - 1;
                addComment("Line", comment, start, idx - 1, loc);
            }
            if (ch == 13 && source(idx) == 10) {
                ++idx;
            }
            ++lineNumber;
            lineStart = idx;
            DEBUGOUT("", false); 
            return;
        }
    }

    if (extra.commentTracking) {
        comment = toU8(slice(sourceraw, start + offset, idx)); 
        loc.end.line = lineNumber;
        loc.end.column = idx - lineStart;
        addComment("Line", comment, start, idx, loc);
    }
    DEBUGOUT("", false);
}

void skipSingleLineComment() { 
    //? are we sure that in javascript the calls to this with no arg will default to 0?
    skipSingleLineComment(0);
}

//throw_
void skipMultiLineComment() {
    DEBUGIN(" skipMultiLineComment()");
    int start;
    Loc loc;
    char16_t ch;
    string comment;

    if (extra.commentTracking) {
        start = idx - 2;
        loc.start.line = lineNumber;
        loc.start.column = idx - lineStart - 2 ;
    }

    while (idx < length) {
        ch = source(idx);
        if (isLineTerminator(ch)) {
            if (ch == 0x0D && source(idx + 1) == 0x0A) {
                ++idx;
            }
            ++lineNumber;
            ++idx;
            lineStart = idx;
            if (idx >= length) {
                throwError(NULLTOKEN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
        } else if (ch == 0x2A) {
            // Block comment ends with ''.
            if (source(idx + 1) == 0x2F) {
                ++idx;
                ++idx;
                if (extra.commentTracking) {
                    comment = toU8(slice(sourceraw, start + 2, idx - 2));
                    loc.end.line = lineNumber;
                    loc.end.column = idx - lineStart;
                    addComment("Block", comment, start, idx, loc);
                }
                DEBUGOUT("", false); 
                return;
            }
            ++idx;
        } else {
            ++idx;
        }
    }

    throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    DEBUGOUT("", false);
    return; //#throw52
}

//throw_
void skipComment() {
    DEBUGIN(" skipComment()");
    char16_t ch;
    bool start;
    start = (idx == 0);
    while (idx < length) {
        ch = source(idx);

        if (isWhiteSpace(ch)) {
            ++idx;
        } else if (isLineTerminator(ch)) {
            ++idx;
            if (ch == 0x0D && source(idx) == 0x0A) {
                ++idx;
            }
            ++lineNumber;
            lineStart = idx;
            start = true;
        } else if (ch == 0x2F) { // U+002F is '/'
            ch = source(idx + 1);
            if (ch == 0x2F) {
                ++idx;
                ++idx;
                skipSingleLineComment(2);
                start = true;
            } else if (ch == 0x2A) {  // U+002A is '*'
                ++idx;
                ++idx;
                skipMultiLineComment(); 
            } else {
                break;
            }
        } else if (start && ch == 0x2D) { // U+002D is '-'
            // U+003E is '>'
            if ((source(idx + 1) == 0x2D) && (source(idx + 2) == 0x3E)) {
                // '-->' is a single-line comment
                idx += 3;
                skipSingleLineComment(3);
            } else {
                break;
            }
        } else if (ch == 0x3C) { // U+003C is '<'
            if (slice(sourceraw, idx + 1, idx + 4) == u"!--") {
                ++idx; // `<`
                ++idx; // `!`
                ++idx; // `-`
                ++idx; // `-`
                skipSingleLineComment(4);
            } else {
                break;
            }
        } else {
            break;
        }
    }
    DEBUGOUT("", false);
    return; //throw52; 
}

 
char16_t scanHexEscape(const char16_t prefix) {
    DEBUGIN("scanHexEscape(const char16_t prefix) {");
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
            DEBUGOUT("scanHexEscape"); 
            return NULL_CHAR16; 
        }
    }
    DEBUGOUT("scanHexEscape"); 
    return code;
}

//throw_
u16string scanUnicodeCodePointEscape() {
    DEBUGIN("scanUnicodeCodePointEscape");
    char16_t ch;
    int code;
    char16_t cu[2];

    ch = source(idx);
    code = 0;

    // At least, one hex digit is required.
    if (ch == u'}') {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
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
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    // UTF-16 Encoding
    if (code <= 0xFFFF) {
        DEBUGOUT("", false);
        return u16string({(char16_t) code});
    }

    cu[0] = ((code - 0x10000) >> 10) + 0xD800; 
    cu[1] = ((code - 0x10000) & 1023) + 0xDC00;
    DEBUGOUT("scanUnicodeCodePointEscape"); 
    return u16string({cu[0], cu[1]});
}

//throw_
string getEscapedIdentifier() {
    DEBUGIN("getEscapedIdentifier");
    char16_t ch;
    u16string id;

    ch = source(idx++);
    id = u16string({ch});

    // '\u' (U+005C, U+0075) denotes an escaped character.
    if (ch == 0x5C) {
        if (source(idx) != 0x75) {
            throwError(NULLTOKEN, 
                       Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierStart(ch)) { 
            throwError(NULLTOKEN, 
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
        append(id, ch);

        // '\u' (U+005C, U+0075) denotes an escaped character.
        if (ch == 0x5C) {
            id = id.substr(0, id.length() - 1);
            if (source(idx) != 0x75) {
                throwError(NULLTOKEN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierPart(ch)) {
                throwError(NULLTOKEN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            append(id, ch);
        }
    }

    DEBUGOUT("getEscapedIdentifier"); 
    return toU8(id);
}

//throw_
string getIdentifier() {
    DEBUGIN("getIdentifier()");
    int start;
    char16_t ch;

    start = idx++;
    while (idx < length) {
        ch = source(idx);
        if (ch == 0x5C) {
            // Blackslash (U+005C) marks Unicode escape sequence.
            idx = start;
            return DEBUGRET("", getEscapedIdentifier());
        }
        if (isIdentifierPart(ch)) {
            ++idx;
        } else {
            break;
        }
    }

    return DEBUGRET("getIdentifier", toU8(slice(sourceraw, start, idx))); 
}

//throw_
TokenStruct scanIdentifier() {
    DEBUGIN(" scanIdentifier()");
    TokenStruct t;
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

    t.type = type;
    t.strvalue = id;
    t.literaltype = LiteralType["String"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    DEBUGOUT("scanIdentifier");
    return t;
}

u16string emccu16str;

 // 7.7 Punctuators
 //throw_
TokenStruct scanPunctuator() {
    DEBUGIN(" scanPunctuator()");
    TokenStruct t;
    int start = idx;

    char16_t ch1 = source(idx);
    string ch2, ch3, ch4;
    char16_t code[2];

    code[0] = source(idx);

    t.type = TknType::Punctuator;
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.literaltype = LiteralType["String"];

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
        t.strvalue = toU8(u16string({ code[0] }));

        t.end = idx;
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
                t.strvalue = toU8(u16string({code[0], code[1]}));      
                t.end = idx;
                DEBUGOUT("", false); 
                return t;
            case 0x21: // !
            case 0x3D: // =
                idx += 2;

                // !== and ===
                if (source(idx) == 0x3D) {
                    ++idx;
                }
                t.strvalue = toU8(slice(sourceraw, start, idx));
                t.end = idx;
                DEBUGOUT("", false); 
                return t;
            }
        }
    }


    // 4-character punctuator: >>>=

    ch4 = toU8(slice(sourceraw, idx, idx+4)); 

    if (ch4 == ">>>=") {
        idx += 4;
        t.strvalue = ch4;
        t.end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 3-character punctuators: === !== >>> <<= >>=

    ch3 = ch4.substr(0, 3); 

    if (ch3 == ">>>" || ch3 == "<<=" || ch3 == ">>=") {
        idx += 3;
        t.strvalue = ch3;
        t.end = idx;
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
        t.strvalue = ch2;
        t.end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 1-character punctuators: < > = ! + - * % & | ^ /

    if (u16string({u"<>=!+-*%&|^/"}) //? is it necessary/correct to have this as ({u""}) and not just (u"")
        .find_first_of(ch1) != std::string::npos) {
        ++idx;
        t.strvalue = toU8(u16string({ch1}));
        t.end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});

    DEBUGOUT("", false); 
    return t;
  //# return avoids compile warnings bcos clang doesn't look into throwError.
}
     // 7.8.3 Numeric Literals
//throw_
TokenStruct scanHexLiteral(const int start) {
    DEBUGIN(" scanHexLiteral(const int start)");
    u16string number;
    TokenStruct t;

    while (idx < length) {
        if (!isHexDigit(source(idx))) {
            break;
        }
        append(number, source(idx++));
    }

    if (number.length() == 0) {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t.type = TknType::NumericLiteral;
    t.intvalue = parseInt(number, 16);
    t.literaltype = LiteralType["Int"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    DEBUGOUT("scanHexLiteral");
    return t;
}

//throw_
TokenStruct scanOctalLiteral(const int start) {
    DEBUGIN(" scanOctalLiteral(const int start)");
    u16string number = u"0";
    TokenStruct t;

    append(number, source(idx++));

    while (idx < length) {
        if (!isOctalDigit(source(idx))) {
            break;
        }
        append(number, source(idx++));
    }

    if (isIdentifierStart(source(idx)) || isDecimalDigit(source(idx))) {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t.type = TknType::NumericLiteral;
    t.intvalue = parseInt(number, 8);
    t.literaltype = LiteralType["Int"];
    t.octal = true;
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    DEBUGOUT("scanOctalLiteral");
    return t;
}

//#throw_
TokenStruct scanNumericLiteral() {
    DEBUGIN(" scanNumericLiteral()");
    int start;
    char16_t ch;
    TokenStruct t;
    u16string number;


    ch = source(idx);
    softAssert(isDecimalDigit(ch) || (ch == u'.'),
               "Numeric literal must start with a decimal digit or a decimal point");

    start = idx;
    number = u"";
    if (ch != u'.') {
        append(number, source(idx++));
        ch = source(idx);

        // Hex number starts with '0x'.
        // Octal number starts with '0'.
        if (number == u"0") {
            if (ch == u'x' || ch == u'X') {
                ++idx;
                return DEBUGRET("scanNumericLiteral", scanHexLiteral(start));
            }
            if (isOctalDigit(ch)) {
                return DEBUGRET("scanNumericLiteral", scanOctalLiteral(start));
            }
            // decimal number starts with '0' such as '09' is illegal.
            if (ch && isDecimalDigit(ch)) {
                throwError(NULLTOKEN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"}); 
            }
        }

        while (isDecimalDigit(source(idx))) {
            append(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'.') {
        //#JSON can't parse decimal numbers without
        //#a number preceding the decimal.
        if (start == idx) { append(number, u'0'); }

        append(number, source(idx++));
        while (isDecimalDigit(source(idx))) {
            //if (source(idx) != u'0') { hasDot = true; } //# js auto-casts dbls of negligible epsilon-to-int to int
            append(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'e' || ch == u'E') {
        append(number, source(idx++));
        ch = source(idx);
        if (ch == u'+' || ch == u'-') {
            append(number, source(idx++));
        }
        if (isDecimalDigit(source(idx))) {
            while (isDecimalDigit(source(idx))) {
                append(number, source(idx++));
            }
        } else {
            throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }
    t.type = TknType::NumericLiteral;
    t.strvalue = toU8(number);
    //t.dblvalue = //# want to allow browsers to use full allowance of whatever their local max int size is.
    //               //#which is same as it would work in esprima.
    //(hasSciNote)?
    //sciNoteToDouble(toU8string(number)) : stod(toU8string(number));
    t.literaltype = LiteralType["Double"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;

    DEBUGOUT("scanNumericLiteral");
    return t; 
}


// 7.8.4 String Literals
//throw_
TokenStruct scanStringLiteral() {
    DEBUGIN(" scanStringLiteral()");
    TokenStruct t;
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
                            append(str, unescaped);
                        } else {
                            idx = restore;
                            append(str, ch);
                        }
                    }    
                } else if (ch == u'n') {
                    append(str, u'\n');
                } else if (ch == u'r') {
                    append(str, u'\r');
                } else if (ch == u't') {
                    append(str, u'\t');
                } else if (ch == u'b') {
                    append(str, u'\b');
                } else if (ch == u'f') {
                    append(str, u'\f');
                } else if (ch == u'v') {
                    append(str, u'\x0B');
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
                        append(str, code);
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
            append(str, ch);
        }
    }

    if (quote != NULL_CHAR16) {
        throwError(NULLTOKEN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t.type = TknType::StringLiteral;
    t.strvalue = toU8(str);
    t.literaltype = LiteralType["String"];            
    t.octal = octal;
    t.startLineNumber = startLineNumber;
    t.startLineStart = startLineStart;
    t.lineNumber = lineNumber;
    t.lineStart = lineStart,
        t.start = start;
    t.end = idx;
    DEBUGOUT("scanStringLiteral");
    return t;
}

//throw_
RegexHalf scanRegExpBody() {
    DEBUGIN("scanRegExpBody()");
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexHalf rh;

    ch = source(idx);
    softAssert(ch == u'/',
               "Regular expression literal must start with a slash");
    append(str, source(idx++)); 

    classMarker = false;
    terminated = false;
    while (idx < length) {
        ch = source(idx++);
        append(str, ch);
        if (ch == u'\\') {
            ch = source(idx++);
            // ECMA-262 7.8.5
            if (isLineTerminator(ch)) {
                throwError(NULLTOKEN, Messages[Mssg::UnterminatedRegExp], {});
            }
            append(str, ch);
        } else if (isLineTerminator(ch)) {
            throwError(NULLTOKEN, Messages[Mssg::UnterminatedRegExp], {});
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
        throwError(NULLTOKEN, Messages[Mssg::UnterminatedRegExp], {});
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length() - 2);
    rh.value = toU8(body);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpBody");
    return rh;
}

//throw_
RegexHalf scanRegExpFlags() {
    DEBUGIN("scanRegExpFlags()");
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
                    append(flags, ch);
                    for (str.append(u16string({u'\\', u'u'})); 
                         restore < idx; ++restore) {                         
                        append(str, source(restore));
                    }
                } else {
                    idx = restore;
                    append(flags, u'u');
                    str.append(u"\\u");
                }

                throwErrorTolerant(NULLTOKEN, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {"ILLEGAL"});
            } else {
                append(str, u'\\');
                throwErrorTolerant(NULLTOKEN, 
                                   Messages[Mssg::UnexpectedToken],
                                   {"ILLEGAL"});
            }
        } else {
            append(flags, ch);
            append(str, ch);
        }
    }

    rh.value = toU8(flags);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpFlags");
    return rh;
}

//throw_
TokenStruct scanRegExp() {
    DEBUGIN(" scanRegExp()");
    int start;
    RegexHalf body; 
    RegexHalf flags; 
    TokenStruct t;
    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = NULLTOKEN;
    skipComment(); 
    start = idx;

    body = scanRegExpBody(); //eregexhalf
    flags = scanRegExpFlags(); //eregexhalf

    //value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t.type = TknType::RegularExpression;
        t.strvalue = "regexDummy"; //?
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = start;
        t.end = idx;
        DEBUGOUT("", false); 
        return t; //not polymorphic right now. not going to work... :!
    }

    t.literal = body.literal; 
    t.literal.append(flags.literal);
    t.literaltype = LiteralType["Regexp"];
    t.strvalue = body.value;
    t.flags = flags.value;
    t.start = start;
    t.end = idx;
    DEBUGOUT("scanRegExp");
    return t;
}

//throw_
TokenStruct collectRegex() {
    DEBUGIN(" collectRegex()");
    int pos;
    Loc loc;
    TokenStruct regex;
    string tokval;

    skipComment();

    pos = idx;
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart;

    regex = scanRegExp(); //etkns
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (!extra.tokenize) {
        TokenRecord token, tr;
        // Pop the previous token, which is likely '/' or '/='
        if (extra.tokenRecords.size() > 0) {
            token = extra.tokenRecords[extra.tokenRecords.size() - 1];
            if (token.range[0] == pos 
                && token.typestring == "Punctuator") {
                
                tokval = token.valuestring; 
                if (tokval == "/" || tokval == "/=") {
                    extra.tokenRecords.pop_back();
                }
            }
        }

        tr.typestring = "RegularExpression";
        tr.valuestring = regex.literal;
        tr.range[0] = pos;
        tr.range[1] = idx;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectRegex");
    return regex;
}

bool isIdentifierName(const TokenStruct token) {
    DEBUGIN("   isIdentifierName(TokenStruct token)");
    DEBUGOUT("", false); 
    return token.type == TknType::Identifier ||
        token.type == TknType::Keyword ||
        token.type == TknType::BooleanLiteral ||
        token.type == TknType::NullLiteral;
}

//throw_
TokenStruct advanceSlash() {
    DEBUGIN(" advanceSlash()");
    //# only gets called if extra.tokenize == true

    TokenRecord prevToken, checkToken;
    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    if (extra.tokenRecords.size() == 0) {
        // Nothing before that: it cannot be a division.
        return DEBUGRET("advSlash1", collectRegex()); 
    }    
    prevToken = extra.tokenRecords[extra.tokenRecords.size() - 1];

    if (prevToken.typestring == "Punctuator") { 
        if (prevToken.valuestring == "]") { 
            return DEBUGRET("advSlash2", scanPunctuator());
        }
        if (prevToken.valuestring == ")") {
            //checkToken && 
            //# instead of checking for existence, we add
            //# the openParenToken value check to the condition above.
            //# remember exta.tokens() is already size > 0 bcos 
            //# check at top of func.

            if (extra.openParenToken > 0
                && extra.tokenRecords.size() > (extra.openParenToken - 1)) { 
                checkToken = extra.tokenRecords[extra.openParenToken - 1];
                if (checkToken.typestring == "Keyword" && 
                    has(checkToken.valuestring, 
                        {"if", "while", "for", "with"})) {
                    return DEBUGRET("advSlash3", collectRegex()); 
                }
            }
            return DEBUGRET("advSlash4", scanPunctuator());
        }
        if (prevToken.valuestring == "}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokenRecords.size() > (extra.openCurlyToken -3) &&
                extra.tokenRecords[extra.openCurlyToken - 3].typestring 
                == "Keyword") { 
                // Anonymous function.

                if (extra.openCurlyToken > 3
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 4)) {
                    checkToken = 
                        extra.tokenRecords[extra.openCurlyToken -4];
                } else { 
                    return DEBUGRET("advSlash5", scanPunctuator());
                }
            } else if (extra.openCurlyToken >= 4 
                       && extra.tokenRecords.size()
                       > (extra.openCurlyToken -4) 
                       && extra.tokenRecords[extra.openCurlyToken - 4].typestring
                       == "Keyword") {
                // again previously had checked type against string in this cond.
                // Named function.
                if (extra.openCurlyToken > 4
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 5)) {
                    checkToken = extra.tokenRecords[extra.openCurlyToken -5];
                } else { 
                    return DEBUGRET("advSlash6", collectRegex());
                }
            } else {
                return DEBUGRET("advSlash7", scanPunctuator());
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (has(checkToken.valuestring, FnExprTokens)) {
                // It is an expression.
                return DEBUGRET("advSlash8", scanPunctuator());
            }
            // It is a declaration.
            return DEBUGRET("advSlash9", collectRegex()); 
        }
        return DEBUGRET("advSlash10", collectRegex());
    }
    if (prevToken.typestring == "Keyword") { 
        return DEBUGRET("advSlash11", collectRegex()); 
    }
    return DEBUGRET("advSlash12", scanPunctuator());
}

//throw_
TokenStruct advance() {
    DEBUGIN(" advance()");
    char16_t ch;
    TokenStruct t;

    skipComment(); 

    if (idx >= length) {
        t.type = TknType::EOFF;
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = idx;
        t.end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    ch = source(idx);
    if (isIdentifierStart(ch)) {
        return DEBUGRET("adv", scanIdentifier());
    }

    // Very common: ( and ) and ;
    if (ch == 0x28 || ch == 0x29 || ch == 0x3B) {
        return DEBUGRET("adv", scanPunctuator());
    }

    // String literal starts with single quote (U+0027) or double quote (U+0022).
    if (ch == 0x27 || ch == 0x22) {
        return DEBUGRET("adv", scanStringLiteral());
    }

    // Dot (.) U+002E can also start a floating-point number, hence the need
    // to check the next character.
    if (ch == 0x2E) {
        if (isDecimalDigit(source(idx + 1))) {
            return DEBUGRET("adv", scanNumericLiteral());
        }
        return DEBUGRET("adv", scanPunctuator());
    }

    if (isDecimalDigit(ch)) {
        return DEBUGRET("adv", scanNumericLiteral());
    }

    // Slash (/) U+002F can also start a regex.
    if (extra.tokenize && ch == 0x2F) {
        return DEBUGRET("adv", advanceSlash());
    }

    return DEBUGRET("adv", scanPunctuator());
}

//throw_
TokenStruct collectToken() {
    DEBUGIN(" collectToken()");
    Loc loc;
    TokenStruct token;
    TokenRecord tr;
    u16string value;

    skipComment(); //ev
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart;

    token = advance(); //etkns
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (token.type != TknType::EOFF) { //this didn't check against string. is fine.
        tr.valuestring = toU8(slice(sourceraw, token.start, token.end));
        tr.typestring = TokenName[token.type];
        tr.range[0] = token.start;
        tr.range[1] = token.end;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectToken");
    return token;
}

//throw_
TokenStruct lex() {
    DEBUGIN(" lex()");
    TokenStruct token;

    token = lookahead;
    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    //#ternary operators not supported by throw52
    if (extra.tokenTracking) {
        lookahead = collectToken();
    } else { 
        lookahead = advance(); 
    }

    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    DEBUGOUT("lex");
    return token;
}

//throw_
void peek() {
    DEBUGIN(" peek()");
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
    DEBUGOUT("peek");
    return; //#throw52
}

 //# Position and SourceLocation are defined as structs near the top.



 
 //Loc 



 

//#this is the ONLY constructor in this code capable of 
//#modifying state, it ALWAYS and ONLY changes state if lookaheadAvail
//#is true. Important to keep in mind when making 
//#1:1 updates.
Node::Node(bool lookaheadAvail, bool storeStats) { 
    //DEBUGIN("Node::Node(bool, bool)", true);
#ifndef THROWABLE
    err = false;
#endif
    hasJv = false;
    isNull = false;
    hasRange = false;
    hasLoc = false;
    if (lookaheadAvail) {
        //#not true for nullnode or parseProgram's node.
        lookavailInit();
    } 
    if (lookaheadAvail || storeStats) {
        if (extra.range) {
            hasRange = true;
            range[0] = idx;
            range[1] = 0;
        }

        if (extra.loc) {
            hasLoc = true;
            loc.start = Position();
        } 
    }
    //DEBUGOUT("", true);
}
Node::Node() : Node(false, true) {} 

json_object* Node::toJson() { 
  return this->jv;
}

string Node::s(const u16string in) { return toU8string(in); }

void Node::lookavailInit() {
    hasJv = true;
    jv = json_newmap();

    idx = lookahead.start;
    if (lookahead.type == TknType::StringLiteral) {
        lineNumber = lookahead.startLineNumber;
        lineStart = lookahead.startLineStart;
    } else {
        lineNumber = lookahead.lineNumber;
        lineStart = lookahead.lineStart;
    }
    if (hasRange) { //#should always be true, but keep it open while testing.
        loc.start = Position();
        range[0] = idx;
    }
}

void Node::clear() {
    regexPaths.clear();
    expressions.clear();

    hasLoc = false;
    hasRange = false;
}

void Node::unused() {
    if (hasJv) {
        json_object_put(this->jv);
    }
}

void Node::jvput(const string path, const string b) 
    {json_put(jv, path.data(), b); }
void Node::jvput(const string path, const int b) 
    {json_put(jv, path.data(), b); }
void Node::jvput(const string path, const bool b) 
    {json_put(jv, path.data(), b); }
void Node::jvput_dbl(const string path, const string b) 
    {json_put_dbl(jv, path.data(), b); }
void Node::jvput_null(const string path) 
    { json_put_null(jv, path.data()); }

//# different name to prevent easy bug of forgetting the string.
//# root path, should be first in vector, then path within it, etc.
void Node::regNoadd(const vector<string> paths, const Node &child) { 
    string debugmsg = " Node::regNoadd(vector<string> paths, Node &child) :::";
    debugmsg.append(paths[0]);
    //DEBUGIN(debugmsg);
    if (child.isNull) { return; }

    if (child.hasRange) {
        json_put(child.jv, "range", 
                 vec2json<int>({child.range[0], child.range[1]}));
    }
    if (child.hasLoc) {
        json_put(child.jv, "loc", locToJson(child.loc));
    }
    if (child.regexPaths.size() > 0) {
        if (child.regexPaths[0][0] == ".") {
            regexPaths.push_back(paths);
        } else {
            for (int i=0; i<child.regexPaths.size(); i++) {
                regexPaths.push_back(child.regexPaths[i]);
                for (int j=paths.size()-1; j>=0; j--) {
                    regexPaths.back().push_back(paths[j]);
                }
            }
        }
    }
    //DEBUGOUT("Node::regNoAdd");
}

void Node::reg(const string path, const Node &child) { 
    //DEBUGIN("reg(string path, Node &child)");
    regNoadd({path}, child);
    if (! child.isNull && child.jv != nullptr) {
        json_put(jv, path.data(), child.jv);
    } else {
        json_put_null(jv, path.data());
    }
    //DEBUGOUT("node::reg");
}

void Node::nodeVec(const string path, const vector< Node > & nodes) { 
    //DEBUGIN("nodeVec(string path, vector< Node > & nodes)");
    json_object * root = json_newarr();
    for (int i=0; i<nodes.size(); i++) {
        if (nodes[i].isNull) {
            json_push_null(root);
        } else {
            regNoadd({path, to_string(i)}, nodes[i]);
            json_push(root, nodes[i].jv);
        }
    } 
    json_put(jv, path.data(), root);
    //DEBUGOUT("node::nodeVec");
}
void Node::addType(const Synt in) { 
    type = Syntax[in];
    json_put(jv, "type", type);
}
json_object* Node::regexPaths2json() { 
    //DEBUGIN("Node::regexPaths2json()");
    json_object *tmp, *root = json_newarr();
    for (int i=0; i<regexPaths.size(); i++) {
        tmp = json_newarr();
        for (int j=regexPaths[i].size()-1; j>=0; j--) {
            json_push(tmp, regexPaths[i][j]);            
        } 
        json_push(root, tmp);
    }
    //DEBUGOUT("", false); 
    return root;
}


//#not the most efficient way to do this. would be easy to choke
//#on big comments. TODO move NodesComments to heap, free
//on removal from bottomright and end of use.
void Node::processComment() {
    DEBUGIN("processComment()");
    //# assumes attachComments 
    //# so that means range is already true.

    vector< Comment > trailingComments;

    vector< NodesComments > * bottomRight = &(extra.bottomRightStack);
    NodesComments lastChild;
    NodesComments last;
    last.isNull = true; lastChild.isNull = true;
    NodesComments thisnc(jv);
    bool LEADING = true, TRAILING= false;
    if (bottomRight->size() > 0) {
        last = bottomRight->back();
    }

    if (type == Syntax[Synt::Program]) {  
        if (json_object_array_length(
                                     json_require(jv,"body", false)) > 0) {
            DEBUGOUT("", false); 
            return;
        }
    }

    thisnc.range[0] = range[0];
    thisnc.range[1] = range[1];

    if (extra.trailingComments.size() > 0) {
        if (extra.trailingComments[0].range[0] >= thisnc.range[1]) {
            trailingComments = extra.trailingComments;
            extra.trailingComments.clear();
        } else {
            extra.trailingComments.clear(); 
            //# originally first clause had =[] and this has .length = 0
            //# don't think there's an effective difference thoug
        }
    } else {
        if (!(last.isNull) && 
            last.trailingComments.size() > 0 && 
            last.trailingComments[0].range[0] >= thisnc.range[1]) {
            trailingComments = last.trailingComments;
            last.trailingComments.clear();
            last.commentsIntoJson(TRAILING);
            //delete last.trailingComments; 
            //? maybe have a boolean to say no trailing comments? length will prob. be workable.
        }
    }

    // Eating the stack.
    if (!(last.isNull)) {
        while ((!(last.isNull)) && last.range[0] >= thisnc.range[0]) {
            lastChild = last;
            if (bottomRight->size() > 0) { 
                last = bottomRight->back(); 
                bottomRight->pop_back();
            } else { 
                last.isNull = true; 
            }
        }
    }

    if (!(lastChild.isNull)) {
        if (lastChild.leadingComments.size() > 0 &&
            lastChild.leadingComments.back()
            .range[1] <= thisnc.range[0]) {
            thisnc.leadingComments = lastChild.leadingComments;
            lastChild.leadingComments.clear();
            lastChild.commentsIntoJson(LEADING);
            thisnc.commentsIntoJson(LEADING);
        }
    } else if (extra.leadingComments.size() > 0 && 
               extra.leadingComments[extra.leadingComments.size() - 1]
               .range[1] <= thisnc.range[0]) {
        thisnc.leadingComments = extra.leadingComments;
        extra.leadingComments.clear();
        thisnc.commentsIntoJson(LEADING);
    }

    if (trailingComments.size() > 0) {
        thisnc.trailingComments = trailingComments;
        thisnc.commentsIntoJson(TRAILING);
    }

    bottomRight->push_back(thisnc);
    DEBUGOUT("", false);
}


void Node::finish() {
    DEBUGIN("finish()");
    if (extra.range) {
        this->range[1] = idx; 
    }
    if (extra.loc) {
        Position newpos;
        loc.end = newpos;
        if (extra.hasSource) {
            loc.source = extra.source; 
            loc.hasSource = true;
        }
    }

    if (extra.attachComment) {
        this->processComment();
    }
    DEBUGOUT("node::finish");
}


void Node::finishArrayExpression(const vector< Node >& elements) {
    DEBUGIN("finishArrayExpression(vector< Node >& elements)");
    addType(Synt::ArrayExpression);
    nodeVec("elements", elements);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishArrowFunctionExpression(const vector< Node >& params,
                                         const vector< Node >& defaults, 
                                         const Node& body, 
                                         const bool expression) {
    DEBUGIN("finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression)");
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
                                      const Node& left, 
                                      const Node& right) {
    DEBUGIN("finishAssignmentExpression(u16string oper, Node& left, Node& right)");

    addType(Synt::AssignmentExpression);
    jvput("operator", oper);

    reg("left", left);

    Node * tmpleft = new Node(false, true);
    *tmpleft = left;
    shared_ptr<Node> tmpleftshared (tmpleft);
    this->left = tmpleftshared; 
    reg("right", right);
    Node * tmpright = new Node(false, true);
    *tmpright = right;
    shared_ptr<Node> tmprightshared (tmpright);
    this->right = tmprightshared; 
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBinaryExpression(const string oper, 
                                  const Node& left, 
                                  const Node& right) {
    DEBUGIN("finishBinaryExpression(u16string oper, Node& left, Node& right)");
    addType((oper == "||" || oper == "&&") ? 
            Synt::LogicalExpression : Synt::BinaryExpression);
    jvput("operator", oper);
    reg("left", left); 
    reg("right", right);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBlockStatement(const vector< Node >& body) {
    DEBUGIN("finishBlockStatement(vector< Node >& body)");
    addType(Synt::BlockStatement);
    nodeVec("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBreakStatement(const Node& label) {
    DEBUGIN("finishBreakStatement(Node& label)");
    addType(Synt::BreakStatement);
    reg("label", label);
    this->finish();  
    DEBUGOUT("", false);
}


void Node::finishCallExpression(const Node& callee, 
                                const vector< Node >& args) {
    DEBUGIN("finishCallExpression(Node& callee, vector< Node >& args)");
    addType(Synt::CallExpression);
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish(); 
    DEBUGOUT("", false);
}


void Node::finishCatchClause(const Node& param, 
                             const Node& body) {
    DEBUGIN("finishCatchClause(Node& param, Node& body)");
    addType(Synt::CatchClause);
    reg("param", param);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishConditionalExpression(const Node& test, 
                                       const Node& consequent,
                                       const Node& alternate) {
    DEBUGIN("finishConditionalExpression(Node& test, Node& consequent, Node& alternate)");
    addType(Synt::ConditionalExpression);
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishContinueStatement(const Node& label) {
    DEBUGIN("finishContinueStatement(Node& label)");
    addType(Synt::ContinueStatement);
    reg("label", label);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishDebuggerStatement() {
    DEBUGIN("finishDebuggerStatement()");
    addType(Synt::DebuggerStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishDoWhileStatement(const Node& body, const Node& test) {
    DEBUGIN("finishDoWhileStatement(Node& body, Node& test)");
    addType(Synt::DoWhileStatement);
    reg("body", body);
    reg("test", test);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishEmptyStatement() {
    DEBUGIN("finishEmptyStatement()");
    addType(Synt::EmptyStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishExpressionStatement(const Node expression) {
    DEBUGIN("finishExpressionStatement(Node expression)");
    addType(Synt::ExpressionStatement);
    reg("expression", expression);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishForStatement(const Node& init, 
                              const Node& test, 
                              const Node& update, 
                              const Node& body) {
    DEBUGIN("finishForStatement(Node& init, Node& test, Node& update, Node& body)");
    addType(Synt::ForStatement);
    reg("init", init);
    reg("test", test);
    reg("update", update);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishForInStatement(const Node& left, 
                                const Node& right, 
                                const Node& body) {
    DEBUGIN("finishForInStatement(Node& left, Node& right, Node& body)");
    addType(Synt::ForInStatement);
    reg("left", left);
    reg("right", right);
    reg("body", body);
    jvput("each", false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionDeclaration(const Node& id, 
                                     const vector< Node >& params, 
                                     const vector< Node >& defaults, 
                                     const Node& body) {
    DEBUGIN("Node::finishFunctionDeclaration(Node, vector<Node>, vector<Node>, Node");
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


void Node::finishFunctionExpression(const Node& id, 
                                    const vector< Node >& params, 
                                    const vector< Node >& defaults, 
                                    const Node& body) {  DEBUGIN("Node::finishFunctionExpression(Node, vector<Node>, vector<Node>, Node");
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
    DEBUGIN("finishIdentifier(u16string name)");
    addType(Synt::Identifier);
    this->name = name;
    jvput("name", name);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishIfStatement(const Node& test, 
                             const Node& consequent, 
                             const Node& alternate) {
    DEBUGIN("finishIfStatement(Node& test, Node& consequent, Node& alternate)");
    addType(Synt::IfStatement);
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishLabeledStatement(const Node& label, 
                                  const Node& body) {
    DEBUGIN("finishLabeledStatement(Node label, Node body)");
    addType(Synt::LabeledStatement);
    reg("label", label);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}

//# ?maybe check against js to make sure we're not missing anything.
void Node::finishLiteral(TokenStruct& token) {
    DEBUGIN("finishLiteral(TokenStruct token)");
    addType(Synt::Literal);
    if (token.literaltype == LiteralType["String"]) {
        jvput("value", token.strvalue);
    } else if (token.literaltype == LiteralType["Int"]) {
        jvput("value", token.intvalue);
    } else if (token.literaltype == LiteralType["Double"]) {
        jvput_dbl("value", token.strvalue);
    } else if (token.literaltype == LiteralType["Bool"]) {
        jvput("value", token.bvalue);
    } else if (token.literaltype == LiteralType["Null"]) {
        jvput_null("value");
    } else if (token.literaltype == LiteralType["Regexp"]) {
        json_put(jv, "value", 
                 vec2json<string>({token.strvalue, token.flags,
                             to_string(lineNumber),
                             to_string(token.end), to_string(token.end+1)}));
        regexPaths.push_back({"."});
    }
    jvput("raw", s(slice(sourceraw, token.start, token.end)));
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishMemberExpression(const char16_t accessor, 
                                  const Node& object, 
                                  const Node& property) {
    DEBUGIN("finishMemberExpression(char16_t accessor, Node& object, Node& property)");
    addType(Synt::MemberExpression);
    jvput("computed", (accessor == u'['));
    reg("object", object);
    reg("property", property);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishNewExpression(const Node& callee, 
                               const vector<Node>& args) {
    DEBUGIN("finishNewExpression(Node& callee, vector<Node>& args)");
    addType(Synt::NewExpression);
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishObjectExpression(const vector<Node>& properties) {
    DEBUGIN("finishObjectExpression(vector<Node>& properties)");
    addType(Synt::ObjectExpression);
    nodeVec("properties", properties);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishPostfixExpression(const string oper, 
                                   const Node& argument) {
    DEBUGIN("finishPostfixExpression(u16string oper, Node& argument)");
    addType(Synt::UpdateExpression);
    jvput("operator", oper);
    reg("argument", argument);
    jvput("prefix", false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishProgram(const vector< Node >& body) {
    DEBUGIN("finishProgram(vector< Node >& body)");
    addType(Synt::Program);
    nodeVec("body", body);
    this->finish(); 
    //no parent node to call reg so add these atts. here.
    if (extra.range) {
        json_put(jv, "range",vec2json<int>({range[0], range[1]}));
    }
    if (extra.loc) {
        json_put(jv, "loc", locToJson(loc));
    }
    DEBUGOUT("", false);    
}


void Node::finishProperty(const string kind,
                          const Node& key, 
                          const Node& value) {
    DEBUGIN("finishProperty(u16string kind, Node& key, Node& value)");
    addType(Synt::Property);
    reg("key", key);
    reg("value", value);
    jvput("kind", kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishReturnStatement(const Node& argument) {
    DEBUGIN("finishReturnStatement(Node& argument)");
    addType(Synt::ReturnStatement);
    reg("argument", argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSequenceExpression(const vector< Node >& expressions) {
    DEBUGIN("finishSequenceExpression(vector< Node >& expressions)");
    addType(Synt::SequenceExpression);
    this->expressions = expressions;
    nodeVec("expressions", expressions);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchCase(const Node& test, 
                            const vector< Node >& consequent) {
    DEBUGIN("finishSwitchCase(Node& test, vector< Node >& consequent)");
    addType(Synt::SwitchCase);
    reg("test", test);
    nodeVec("consequent", consequent);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchStatement(const Node& discriminant, 
                                 const vector < Node >& cases) {
    DEBUGIN("finishSwitchStatement(Node& discriminant, vector < Node >& cases)");
    addType(Synt::SwitchStatement);
    reg("discriminant", discriminant);
    nodeVec("cases", cases);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThisExpression() {
    DEBUGIN("finishThisExpression()");
    addType(Synt::ThisExpression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThrowStatement(const Node& argument) {
    DEBUGIN("finishThrowStatement(Node& argument)");
    addType(Synt::ThrowStatement);
    reg("argument", argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishTryStatement(const Node& block, 
                              const vector<Node>& guardedHandlers, 
                              const vector<Node>& handlers, 
                              const Node& finalizer) {
    addType(Synt::TryStatement);
    reg("block", block);
    nodeVec("guardedHandlers", guardedHandlers);
    nodeVec("handlers", handlers);
    reg("finalizer", finalizer);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishUnaryExpression(const string oper, 
                                 const Node& argument) {
    DEBUGIN("finishUnaryExpression(u16string oper, Node& argument)");
    addType((oper == "++" || oper == "--") ? 
            Synt::UpdateExpression : Synt::UnaryExpression);
    jvput("operator", oper);
    reg("argument", argument);
    jvput("prefix", true);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclaration(const vector< Node >& declarations, 
                                     const string kind) {

    addType(Synt::VariableDeclaration);
    nodeVec("declarations", declarations);
    jvput("kind", kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclarator(const Node& id, 
                                    const Node& init) {
    DEBUGIN("finishVariableDeclarator(Node& id, Node& init)");
    addType(Synt::VariableDeclarator);
    reg("id", id);
    reg("init", init);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWhileStatement(const Node& test, 
                                const Node& body) {
    DEBUGIN("finishWhileStatement(Node& test, Node& body)");
    addType(Synt::WhileStatement);
    reg("test", test);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWithStatement(const Node& object, 
                               const Node& body) {
    DEBUGIN("finishWithStatement(Node& object, Node& body)");
    addType(Synt::WithStatement);
    reg("object", object);
    reg("body", body);
    this->finish();
    DEBUGOUT("", false);
}


class WrappingNode : public Node {
public:
    WrappingNode(TokenStruct startToken) : Node(false, true) {
        DEBUGIN("WrappingNode(Token)", true);
        if (!hasJv) { 
            jv = json_newmap(); 
            hasJv=true;
        }
        if (extra.range) {
            hasRange = true;
            range[0] = startToken.start;
            range[1] = 0;
        }

        if (extra.loc) {
            loc = this->WrappingSourceLocation(startToken);
        }

        DEBUGOUT("Wr", true);
    }
    Loc WrappingSourceLocation(TokenStruct startToken) {
        DEBUGIN("WrappingSourceLocation (Token)", true);
        Loc result;
        if (startToken.type == TknType::StringLiteral) {
            result.start.line = startToken.startLineNumber;
            result.start.column = 
                startToken.start - startToken.startLineStart;
        } else {
            result.start.line = startToken.lineNumber;
            result.start.column = startToken.start - startToken.lineStart;
        }
        result.end.line = -1;
        result.end.column = -1;
        //return result;
        DEBUGOUT("WraSrcLoc", true); 
        return result;
    }
};

// Return true if there is a line terminator before the next token.

//throw_
bool peekLineTerminator() {
    DEBUGIN(" peekLineTerminator()");
    int pos = idx,
        line = lineNumber,
        start = lineStart;
    bool found;
    skipComment();
    found = (lineNumber != line);
    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("", false);
    return found;
}

#ifndef THROWABLE
//throw_
int throwToJS(ExError err) {
    DEBUGIN(" throwToJS(ExError err)");
    retError = err;
    errorType = 0;
    ErrWrapint evoid;
    evoid.err = true;
    DEBUGOUT("throwToJs", false);
    return evoid;
}
#endif
#ifdef THROWABLE
void throwToJS(const ExError err) {
    DEBUGIN(" throwToJS(ExError err)");
    DEBUGOUT("throwToJs", false);    
    throw err;
}
#endif


ExError genExError(TokenStruct& token, const string messageFormat, 
                   vector< string > args) {
    DEBUGIN(" genExError");
    ExError error;
    int searchresult;
    string searchkey, msg = messageFormat;
    for (int i=0; i<args.size(); i++) {
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

    if (token.lineNumber != -1) {
        error.index = token.start;
        error.lineNumber = token.lineNumber;
        error.column = token.start - lineStart + 1;
    } else {
        error.index = idx;
        error.lineNumber = lineNumber;
        error.column = idx - lineStart + 1;
    }

    error.description = msg;
    DEBUGOUT("genExErr");
    return error;
}

//throw_
void throwError(TokenStruct& token, 
                const string messageFormat, 
                vector< string > args) {
    DEBUGIN(" throwError(TokenStruct token, u16string messageFormat, vector<u16string> args)");
    throwToJS(genExError(token, messageFormat, args));
    DEBUGOUT(" throwError()");
    return;
}

//throw_
void throwErrorTolerant(TokenStruct& token, 
                        const string messageFormat, 
                        vector<string> args) {
    DEBUGIN(" throwErrorTolerant(TokenStruct token, u16string messageFormat, vector<u16string> args)");
    ExError result = genExError(token, messageFormat, args);
    if (extra.errorTolerant) {
        extra.errors.push_back(result);
    } else {
        throwToJS(result);
    }
    DEBUGOUT("throwErrTol");
    return;
}

// Throw an exception because of the token.
//throw_
void throwUnexpected(TokenStruct& token) {
    DEBUGIN(" throwUnexpected(TokenStruct token)");
    if (token.type == TknType::EOFF) {
        throwError(token, Messages[Mssg::UnexpectedEOS], {});
    }

    if (token.type == TknType::NumericLiteral) {
        throwError(token, Messages[Mssg::UnexpectedNumber], {});
    }

    if (token.type == TknType::StringLiteral) {
        throwError(token, Messages[Mssg::UnexpectedString], {});
    }

    if (token.type == TknType::Identifier) {
        throwError(token, Messages[Mssg::UnexpectedIdentifier], {});
    }

    if (token.type == TknType::Keyword) {
        if (isFutureReservedWord(token.strvalue)) {
            throwError(token, Messages[Mssg::UnexpectedReserved],{});
        } else if (strict && isStrictModeReservedWord(token.strvalue)) {
            throwErrorTolerant(token, Messages[Mssg::StrictReservedWord], {});
            DEBUGOUT("", false); 
            return;
        }
        throwError(token, Messages[Mssg::UnexpectedToken], {token.strvalue});
    }

    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages[Mssg::UnexpectedToken], {token.strvalue});
    return; //#throw52
}
    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.



//throw_
void expect(const string value) { 
    //DEBUGIN(" expect(u16string value)");

    TokenStruct token = lex();


    if (token.type != TknType::Punctuator || 
          token.strvalue != value) {
        throwUnexpected(token); 
    }
    // DEBUGOUT("expect");
    return;
}



//throw_
void expectTolerant(const string value) {
    // DEBUGIN(" expectTolerant(u16string value)");
    if (extra.errorTolerant) {
        TokenStruct token = lookahead;


        if (token.type != TknType::Punctuator || 
              token.strvalue != value) {
            throwErrorTolerant(token, Messages[Mssg::UnexpectedToken], 
                               {token.strvalue});
        } else {
            lex();
        }
    } else {
        expect(value);
    }
    // DEBUGOUT("expectTol");
    return;
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
//throw_
void expectKeyword(const string keyword) { 
    TokenStruct token = lex();
    if (token.type != TknType::Keyword || 
        token.strvalue != keyword) {
        throwUnexpected(token);
    }
    return;
}


// Return true if the next token matches the specified punctuator.

bool match(const string value) { 
    return lookahead.type == TknType::Punctuator 
        && lookahead.strvalue == value;
}

// Return true if the next token matches the specified keyword


bool matchKeyword(const string keyword) {
    // DEBUGIN(" matchKeyword(const u16string keyword)");
    //  DEBUGOUT("matchKey"); 
    return lookahead.type == TknType::Keyword 
        && lookahead.strvalue == keyword;
}

    // Return true if the next token is an assignment operator


bool matchAssign() { 
    if (lookahead.type != TknType::Punctuator) {
        return false;
    }
    return has(lookahead.strvalue, {
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
//throw_
Node parseAssignmentExpression();
//throw_
Node parseFunctionSourceElements();
//throw_
Node parseVariableIdentifier();
//throw_
Node parseExpression();
//throw_
Node parseFunctionExpression();
//throw_
Node parseLeftHandSideExpression();
//throw_
Node parseSourceElement();
//throw_
Node parseStatement();
//throw_
vector<Node> parseVariableDeclarationList(const u16string in);
//throw_
Node parseFunctionDeclaration();


//#can't dynamically initialize empty vectors
//#if func is passed by reference.
vector<Node> EMPTY_NODE_LIST;

//throw_
void consumeSemicolon() {
    DEBUGIN(" consumeSemicolon()");
    int line;

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B || match(";")) { 
        lex();
        DEBUGOUT("", false); 
        return;
    }

    line = lineNumber;
    skipComment(); 
    if (lineNumber != line) {
        DEBUGOUT("", false); 
        return;
    }

    if (lookahead.type != TknType::EOFF && !match("}")) { 
        throwUnexpected(lookahead); 
    }
    DEBUGOUT("consumeSemi");
    return;
}

    // Return true if provided expression is LeftHandSideExpression

bool isLeftHandSide(Node expr) {
    DEBUGIN("   isLeftHandSide(Node expr)");
    DEBUGOUT("isLeft");
    return expr.type == Syntax[Synt::Identifier] || expr.type == Syntax[Synt::MemberExpression];
};


// 11.1.4 Array Initialiser

//throw_
Node parseArrayInitialiser() {
    DEBUGIN(" parseArrayInitialiser()");
    vector< Node > elements;
    Node node(true, true);    

    expect("[");

    while (!match("]")) {

        if (match(",")) {
            lex();
            elements.push_back(NULLNODE);
        } else {
#ifndef THROWABLE
            Node tmp = parseAssignmentExpression();
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

    lex();

    node.finishArrayExpression(elements);
    DEBUGOUT("parseArrInit");
    return node;
}

// 11.1.5 Object Initialiser

//throw_
Node parsePropertyFunction(vector<Node>& param, TokenStruct first) {
    DEBUGIN(" parsePropertyFunction(vector<Node>& param, TokenStruct first)");
    bool previousStrict;
    Node body(false, true), node(true, true);

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (!(first.isNull) && strict && isRestrictedWord(param[0].name)) {
        throwErrorTolerant(first, Messages[Mssg::StrictParamName],{});
    }
    strict = previousStrict;
    node.finishFunctionExpression(NULLNODE, param, 
                                  EMPTY_NODE_LIST, body); 
    DEBUGOUT("parsePropFunction");
    return node;
}

//throw_
Node parseObjectPropertyKey() {
    DEBUGIN(" parseObjectPropertyKey()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token.type == TknType::StringLiteral || 
        token.type == TknType::NumericLiteral) {
        if (strict && token.octal) {
            throwErrorTolerant(token, Messages[Mssg::StrictOctalLiteral], {});
        }
        node.finishLiteral(token);
        DEBUGOUT("", false); 
        return node;
    }

    node.finishIdentifier(token.strvalue);
    DEBUGOUT("ParseObjPropKey");
    return node;
}



//@Empty_node_list is passed as a vector_of_arrays,
//@ where normally what would be passed into .finishFunctionExpression is
//@ an undefined variable. this might make the value null expectedly,
//@ while making this version render empty list.
//throw_
Node parseObjectProperty() {
    DEBUGIN(" parseObjectProperty()");
    TokenStruct token;
    vector<Node> param;
    Node id(false, true), key(false, true), value(false, true), 
        node(true,true);

    token = lookahead;

    if (token.type == TknType::Identifier) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token.strvalue == "get" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            expect(")");
            value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                          NULLTOKEN);
            node.finishProperty("get", key, value);
            DEBUGOUT("parseObjProp"); 
            return node;
        }
        if (token.strvalue == "set" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            token = lookahead;
            if (token.type != TknType::Identifier) {
                expect(")");
                throwErrorTolerant(token, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {token.strvalue});
                value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                              NULLTOKEN);
            } else {
#ifndef THROWABLE
                Node tmp = parseVariableIdentifier();
                param = vector< Node >({ tmp });
#endif
#ifdef THROWABLE
                param = vector< Node >({ parseVariableIdentifier() });
#endif
                expect(")");
                value = parsePropertyFunction(param, token);
            }
            node.finishProperty("set", key, value);
            DEBUGOUT("parseObjProp"); 
            return node;
        }
        expect(":");
        value = parseAssignmentExpression();
        node.finishProperty("init", id, value);
        DEBUGOUT("parseObjProp"); 
        return node;
    }
    if (token.type == TknType::EOFF || token.type == TknType::Punctuator) {
        throwUnexpected(token);
        DEBUGOUT("parseObjProp"); 
        return NULLNODE; //#just to satisfy warnings.
    } else {
        key = parseObjectPropertyKey();
        expect(":");
        value = parseAssignmentExpression();
        node.finishProperty("init", key, value);
        DEBUGOUT("parseObjProp"); 
        return node;
    }
}

string json_tostring(json_object * in) {
    DEBUGIN("json_tostring");
    json_type objtype = json_object_get_type(in);
    if (objtype == json_type_string) {
        return DEBUGRET("json_tostring", json_object_get_string(in));
    } else if (objtype == json_type_double) {
        return DEBUGRET("json_tostring", to_string(json_object_get_double(in)));
    } else if (objtype == json_type_boolean) {
        return DEBUGRET("json_tostring", to_string(json_object_get_boolean(in)));
    }
    DEBUGOUT("json_tostring");
    return "";
}

//throw_
Node parseObjectInitialiser() {
    DEBUGIN(" parseObjectInitialiser()");
    vector<Node> properties;
    TokenStruct token;
    Node node(true, true), property(false, true);
    json_object * keyobj;
    string keytype, key, name, kindname;
    int kind;
    map<string, int> kmap;

    expect("{");

    while (!match("}")) {
        property = parseObjectProperty();
        keyobj = json_require(property.jv, "key", false);
        keytype = json_object_get_string(
                       json_require(keyobj, "type", false));

        if (keytype == Syntax[Synt::Identifier]) {
            name = json_object_get_string(
                      json_require(keyobj,  "name", false));
        } else {
            name = json_tostring(
                      json_require(keyobj,  "value", false));
        }
        kindname = json_object_get_string(
                      json_require(property.jv,  "kind", false));
        kind = (kindname == "init") ? PropertyKind["Data"] : 
            (kindname == "get") ? PropertyKind["Get"] : PropertyKind["Set"];

        key = "$";
        key.append(name);
        if (hasStringKey<int>(key,kmap)) {
            if (kmap[key] == PropertyKind["Data"]) {
                if (strict && kind == PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, 
                            Messages[Mssg::StrictDuplicateProperty],{});
                } else if (kind != PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, 
                            Messages[Mssg::AccessorDataProperty],{});
                }
            } else {
                if (kind == PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, 
                            Messages[Mssg::AccessorDataProperty],{});
                } else if (kmap[key] & kind) {
                    throwErrorTolerant(NULLTOKEN, 
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

    node.finishObjectExpression(properties);
    DEBUGOUT("parseObjectInit");
    return node;
}

 // 11.1.6 The Grouping Operator
//throw_
Node parseGroupExpression() {
    DEBUGIN(" parseGroupExpression()");
    Node expr(false, true);
    expect("(");
    if (match(")")) {
        lex();
        DEBUGOUT("", false); 
        return PlaceHolders["ArrowParameterPlaceHolder"];
    }
    ++(state.parenthesisCount);
    expr = parseExpression();
    expect(")");
    string debugmsg="parseGroupExpr()";
    DEBUGOUT(debugmsg);
    return expr;
}


// 11.1 Primary Expressions
//throw_
Node parsePrimaryExpression() {
    DEBUGIN(" parsePrimaryExpression()");
    //# there's some opportunity here for nested function calls
    //# by using preprocess if defined / else / endifs 
    //# that use nested in non-asm. But they're just tokens so no
    //# depth (and high copy cost) like nodes.
    //# and because there are so many it makes this function visually
    //# hard to follow if we use nested whenever possible in this.
    TknType type; 
    TokenStruct token;
    Node expr(false, true), node(false, true);

    if (match("(")) {
        return DEBUGRET("", parseGroupExpression());
    }

    if (match("[")) {
        return DEBUGRET("", parseArrayInitialiser());
    }

    if (match("{")) {
        return DEBUGRET("", parseObjectInitialiser());
    }

    type = lookahead.type;
    node.lookavailInit();

    expr = node;

    if (type == TknType::Identifier) {
        TokenStruct tmp = lex();
        expr.finishIdentifier(tmp.strvalue);
    } else if (type == TknType::StringLiteral || 
               type == TknType::NumericLiteral) {

        if (strict && lookahead.octal) {
            throwErrorTolerant(lookahead, 
                               Messages[Mssg::StrictOctalLiteral], {});
        }
        TokenStruct tmp = lex();
        expr.finishLiteral(tmp);
    } else if (type == TknType::Keyword) {
        if (matchKeyword("function")) {
            expr.unused();
            return DEBUGRET("", parseFunctionExpression());
        }
        if (matchKeyword("this")) {
            lex();
            expr.finishThisExpression();
        } else {
            TokenStruct tmp = lex();
            throwUnexpected(tmp);
        }
    } else if (type == TknType::BooleanLiteral) {
        token = lex();
        token.bvalue = (token.strvalue == "true");
        token.literaltype = LiteralType["Bool"];
        expr.finishLiteral(token);
    } else if (type == TknType::NullLiteral) {
        token = lex();
        token.isNull = true;
        token.literaltype = LiteralType["Null"];
        expr.finishLiteral(token);
    } else if (match("/") || match("/=")) {
        TokenStruct tmp;
        if (extra.tokenTracking) {
            tmp = collectRegex();
            expr.finishLiteral(tmp);
        } else {
            tmp = scanRegExp();
            expr.finishLiteral(tmp);
        }
        peek();
    } else {
        TokenStruct tmp = lex();
        throwUnexpected(tmp);
    }

    DEBUGOUT("", false);
    return expr;
}

// 11.2 Left-Hand-Side Expressions
//throw_
vector< Node > parseArguments() {
    DEBUGIN(" parseArguments()");
    vector< Node > args; 
    expect("(");
    if (!match(")")) {
        while (idx < length) {
#ifndef THROWABLE
            Node tmp = parseAssignmentExpression();
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
    DEBUGOUT("parseArgu");
    return args;
}

//throw_
Node parseNonComputedProperty() {
    DEBUGIN(" parseNonComputedProperty()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    if (!isIdentifierName(token)) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.strvalue);
    DEBUGOUT("", false);
    return node;
}

//throw_
Node parseNonComputedMember() {
    DEBUGIN(" parseNonComputedMember()");
    expect(".");
    return DEBUGRET("parseNonComp", parseNonComputedProperty());
}

//throw_
Node parseComputedMember() {
    DEBUGIN(" parseComputedMember()");
    Node expr(false, true);
    expect("[");
    expr = parseExpression();
    expect("]");
    DEBUGOUT("parseComp");
    return expr;
}

//throw_
Node parseNewExpression() {
    DEBUGIN(" parseNewExpression()");
    vector< Node > args;
    Node callee(false, true), node(true, true);

    expectKeyword("new");
    callee = parseLeftHandSideExpression();
    if (match("(")) { 
        args = parseArguments(); 
    }

    node.finishNewExpression(callee, args);
    DEBUGOUT("parseNewExpr");
    return node;
}

//throw_
Node parseLeftHandSideExpressionAllowCall() {
    DEBUGIN(" parseLeftHandSideExpressionAllowCall()");
    vector< Node > args;
    Node expr(false, true), property(false, true), tmpnode(false,true);
    TokenStruct startToken;
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
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else if (match("(")) {
            args = parseArguments();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishCallExpression(expr, args);
            expr = tmpnode;
        } else if (match("[")) {
            property = parseComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else {
            break;
        }
    }

    state.allowIn = previousAllowIn;

    DEBUGOUT("parseLeftHandSideExprAllow"); 
    return expr; 
}

//throw_
Node parseLeftHandSideExpression() {
    DEBUGIN(" parseLeftHandSideExpression()");
    Node tmpnode(false, true), expr(false, true), property(false, true);
    TokenStruct startToken;

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
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else if (match(".")) {
            property = parseNonComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else {            
            break;
        }
    }
    DEBUGOUT("parseLeftHandSideExpr");
    return expr;
}

// 11.3 Postfix Expressions
//throw_
Node parsePostfixExpression() {
    DEBUGIN(" parsePostfixExpression()");
    Node expr(false, true),tmpnode(false, true);
    bool pltresult;
    TokenStruct token, startToken = lookahead;

    expr = parseLeftHandSideExpressionAllowCall();
    
    if (lookahead.type == TknType::Punctuator) {
        if (match("++") || match("--")) {
            pltresult = peekLineTerminator(); //#throw52
            if (!pltresult) {
                // 11.3.1, 11.3.2
                if (strict && expr.type == Syntax[Synt::Identifier] && 
                    isRestrictedWord(expr.name)) {
                    throwErrorTolerant(NULLTOKEN,
                                       Messages[Mssg::StrictLHSPostfix],{});
                }

                if (!isLeftHandSide(expr)) {
                    throwErrorTolerant(NULLTOKEN,
                                       Messages[Mssg::InvalidLHSInAssignment],
                                       {});
                }

                token = lex();
                tmpnode = WrappingNode(startToken);
                tmpnode.finishPostfixExpression(token.strvalue, expr);
                DEBUGOUT("parsePostfix"); 
                return tmpnode;
            }
        }
    }

    DEBUGOUT("parsePostfix");
    return expr;
}

// 11.4 Unary Operators
//throw_
Node parseUnaryExpression() {
    DEBUGIN(" parseUnaryExpression()");
    TokenStruct token, startToken;
    Node expr(false, true), tmpnode(false, true);
    u16string exprname;

    if (lookahead.type != TknType::Punctuator 
        && lookahead.type != TknType::Keyword) {
        expr = parsePostfixExpression();
    } else if (match("++") || match("--")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        // 11.4.4, 11.4.5
        if (strict && expr.type == Syntax[Synt::Identifier] 
            && isRestrictedWord(expr.name)) {
            throwErrorTolerant(NULLTOKEN, Messages[Mssg::StrictLHSPrefix],{});
        }

        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        DEBUGOUT("parseUnary"); 
        return tmpnode;
    } else if (match("+") || match("-") || match("~") || match("!")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        DEBUGOUT("parseUnary"); 
        return tmpnode;
    } else if (matchKeyword("delete") 
               || matchKeyword("void") 
               || matchKeyword("typeof")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        if (strict && token.strvalue == "delete" 
            && expr.type == Syntax[Synt::Identifier]) {
            throwErrorTolerant(NULLTOKEN, Messages[Mssg::StrictDelete], {});
        }
        DEBUGOUT("parseUnary"); 
        return tmpnode;
    } else {
        expr = parsePostfixExpression();
    }

    DEBUGOUT("parseUnary");
    return expr;
}

int binaryPrecedence(const TokenStruct token, 
                     const bool allowIn) {
    DEBUGIN(" binaryPrecedence(Tokenstruct token, bool allowIn)");
    int prec = 0;
    string tokval;

    if (token.type != TknType::Punctuator 
        && token.type != TknType::Keyword) {
        DEBUGOUT("binaryPrec"); 
        return 0;
    }
    tokval = token.strvalue;

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

    DEBUGOUT("binaryPrec");
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
Node parseBinaryExpression() {
    DEBUGIN(" parseBinaryExpression()");

    Node tmpnode(false, true), expr(false, true), left(false, true), 
        right(false, true);
    vector < Node > nodestack;
    TokenStruct marker, token;
    vector< TokenStruct > markers, tokstack;
    string oper;
    int i, prec;

    marker = lookahead;

    left = parseUnaryExpression();

    if (left.type == PlaceHolders["ArrowParameterPlaceHolder"].type) {
        //? placeholder
        DEBUGOUT("parseBinary1"); 
        return left;
    }

    token = lookahead;    
    prec = binaryPrecedence(token, state.allowIn);
    if (prec == 0) {
        DEBUGOUT("parseBinary2"); 
        return left;
    }
    token.prec = prec;
    lex();

    markers.push_back(marker);
    markers.push_back(lookahead);
    right = parseUnaryExpression();

    //we want to mirror esprima's js as close as possible. because it uses
    //a stack of mixed tokens or nodes (and the math is done so it always 
    //knows the type of the position its using) we've simply made two different
    //type stackeds (where a position is always only occupied by a non-null value in one of the stacks)
    //and access the right stack at the right time.
    nodestack.push_back(left);
    tokstack.push_back(NULLTOKEN);
    nodestack.push_back(NULLNODE);
    tokstack.push_back(token);
    nodestack.push_back(right);
    tokstack.push_back(NULLTOKEN);

    while ((prec = binaryPrecedence(lookahead, state.allowIn)) > 0) { 

        // Reduce: make a binary expression from the three topmost entries.
        while ((tokstack.size() > 2) && 
               (prec <= tokstack[tokstack.size() - 2].prec)) { 
            //? will this work the same in c++ as in js?
            right = nodestack.back(); 
            nodestack.pop_back(); tokstack.pop_back();
            oper = tokstack.back().strvalue;
            nodestack.pop_back(); tokstack.pop_back();
            left = nodestack.back();
            nodestack.pop_back(); tokstack.pop_back();
            markers.pop_back();

            expr = WrappingNode(markers[markers.size() - 1]);

            expr.finishBinaryExpression(oper, left, right);

            nodestack.push_back(expr); tokstack.push_back(NULLTOKEN);
        }

        // Shift.
        token = lex();
        token.prec = prec;
        nodestack.push_back(NULLNODE);
        tokstack.push_back(token);
        markers.push_back(lookahead);
        expr = parseUnaryExpression();
        nodestack.push_back(expr);
        tokstack.push_back(NULLTOKEN);
    }

    // Final reduce to clean-up the stack.
    i = nodestack.size() - 1;
    expr = nodestack[i];
    markers.pop_back();
    while (i > 1) {
        tmpnode = WrappingNode(markers.back());
        markers.pop_back();
        tmpnode.finishBinaryExpression(
                                       tokstack[i - 1].strvalue, nodestack[i - 2], expr);

        expr = tmpnode;
        i -= 2;
    }

    DEBUGOUT("parseBinary3");
    return expr;
}


// 11.12 Conditional Operator

//throw_
Node parseConditionalExpression() {
    DEBUGIN(" parseConditionalExpression()");
    Node expr(false, true), tmpnode(false, true), 
        consequent(false, true), alternate(false, true);
    bool previousAllowIn;
    TokenStruct startToken;

    startToken = lookahead;

    expr = parseBinaryExpression();
    if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type) { 
        //? ever supposed to eval. to true? cause it might in some cases
        //? even tho it seems in javascript it never ever will.

        DEBUGOUT("parseCondExpr1"); 
        return expr;
    }
    if (match("?")) {
        lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        state.allowIn = previousAllowIn;
        expect(":");
        alternate = parseAssignmentExpression();

        tmpnode = WrappingNode(startToken);
        tmpnode.finishConditionalExpression(expr, consequent, alternate);
        DEBUGOUT("parseCondExpr2"); 
        return tmpnode;
    }

    DEBUGOUT("parseCondExpr3");
    return expr;
}

// [ES6] 14.2 Arrow Function
//throw_
Node parseConciseBody() {
    DEBUGIN(" parseConciseBody()");
    if (match("{")) {
        return DEBUGRET("parseConciseBody", parseFunctionSourceElements());
    }
    return DEBUGRET("parseConciseBody", parseAssignmentExpression());
}

void validateParamNode(ReinterpretOptions& options,
                       Node param, const string name) {
    DEBUGIN("validateParamNode(Reinterp, Node, u16str)");
    string key = "$";
    key.append(name);
    if (strict) {
        if (isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamName];
        }
        if (has(key, options.paramSet)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    } else if (options.firstRestricted.isNull) {
        if (isRestrictedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamName];
        } else if (isStrictModeReservedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictReservedWord];
        } else if (has(key, options.paramSet)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    }
    options.paramSet.insert(key);
    DEBUGOUT("validateParamNode");
}

//throw_
ReinterpretOut reinterpretAsCoverFormalsList(vector< Node >& expressions) { 
    DEBUGIN("reinterpretAsCover");
    int i, len, defaultCount;
    Node param(false, true);
    vector< Node > params;
    vector< Node > defaults;
    ReinterpretOptions opts;
    ReinterpretOut reOut; 
    void* rest;

    defaultCount = 0;
    rest = nullptr;

    for (i = 0, len = expressions.size(); i < len; i += 1) {
        param = expressions[i];
        if (param.type == Syntax[Synt::Identifier]) {
            params.push_back(param);
            defaults.push_back(NULLNODE);
            validateParamNode(opts, param, param.name);
        } else if (param.type == Syntax[Synt::AssignmentExpression]) {
            params.push_back(*(param.left));
            defaults.push_back(*(param.right));
            ++defaultCount;
            validateParamNode(opts, *(param.left), (*(param.left)).name);
        } else {
            reOut.isNull = true;
            DEBUGOUT("", false); 
            return reOut; 
        }
    }

    if (opts.message == Messages[Mssg::StrictParamDupe]) {
        throwError(
                   //strict ? opts.stricted : opts.firstRestricted, //? replicate?
                   NULLTOKEN,
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

    DEBUGOUT("reinterpret");
    return reOut;
}

//throw_
Node parseArrowFunctionExpression(const ReinterpretOut options, Node node) {
    DEBUGIN(" parseArrowFunctionExpression(ReinterpretOut options, Node node)");
    bool previousStrict;
    Node body(false, true);

    expect("=>");
    previousStrict = strict;

    body = parseConciseBody();

    if (strict && !(options.firstRestricted.isNull)) { 
        throwError(NULLTOKEN, options.message, {});
        //        throwError(options.firstRestricted, options.message, {});
    }
    if (strict && !(options.stricted.isNull)) {
        throwError(NULLTOKEN, options.message, {});
        //        throwErrorTolerant(options.stricted, options.message, {});
    }

    strict = previousStrict;

    node.finishArrowFunctionExpression(options.params, 
                                       options.defaults, 
                                       body, 
                                       body.type != Syntax[Synt::BlockStatement]);
    DEBUGOUT("parseArrowFuncExpr");
    return node;
}

    // 11.13 Assignment Operators


//throw_
Node parseAssignmentExpression() {
    DEBUGIN(" parseAssignmentExpression()");
    int oldParenthesisCount;
    TokenStruct token, startToken;
    Node expr(false, true), right(false, true), tmpnode(false, true);
    vector<Node> reIn;
    ReinterpretOut list;


    oldParenthesisCount = state.parenthesisCount;

    startToken = lookahead;
    token = lookahead;

    expr = parseConditionalExpression();
    list.isNull = true;
    if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type //? will work?
        || match("=>")) {

        if (state.parenthesisCount == oldParenthesisCount ||
            state.parenthesisCount == (oldParenthesisCount + 1)) {      
            if (expr.type == Syntax[Synt::Identifier]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn); 
            } else if (expr.type == Syntax[Synt::AssignmentExpression]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn);
            } else if (expr.type == Syntax[Synt::SequenceExpression]) {
                list = reinterpretAsCoverFormalsList(expr.expressions);
            } else if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type) {
                list = reinterpretAsCoverFormalsList(reIn); 
            }
            if (!(list.isNull)) {
                return DEBUGRET("parseAssignExpr1", parseArrowFunctionExpression(list, WrappingNode(startToken)));
            }
        }
    }

    if (matchAssign()) {
        // LeftHandSideExpression


        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        // 11.13.1
        if (strict 
            && expr.type == Syntax[Synt::Identifier] 
            && isRestrictedWord(expr.name)) {
            throwErrorTolerant(token, Messages[Mssg::StrictLHSAssignment], {});
        }

        token = lex();
        right = parseAssignmentExpression();
        tmpnode = WrappingNode(startToken);

        tmpnode.finishAssignmentExpression(token.strvalue, expr, right); 
        DEBUGOUT("parseAssignExpr2"); 
        return tmpnode;
    }

    DEBUGOUT("parseAssignExpr3"); 
    return expr;
}

// 11.14 Comma Operator
//throw_
Node parseExpression() { 
    DEBUGIN(" parseExpression()");
    Node expr(false, true); 
    TokenStruct startToken = lookahead;
    vector< Node > expressions;

    expr = parseAssignmentExpression();

    if (match(",")) {
        expressions.push_back(expr);

        while (idx < length) {
            if (!match(",")) {
                break;
            }
            lex();
#ifndef THROWABLE
            Node tmp = parseAssignmentExpression();
            expressions.push_back(tmp);
#endif
#ifdef THROWABLE
            expressions.push_back(parseAssignmentExpression());
#endif
        }

        expr = WrappingNode(startToken);
        expr.finishSequenceExpression(expressions);
    }
    string debugmsg = "parseExpr()";
    debugmsg.append(expr.type); 
    DEBUGOUT(debugmsg); 
    return expr;
}

// 12.1 Block
//throw_
vector< Node > parseStatementList() { 
    DEBUGIN(" parseStatementList()");
    vector< Node > list;
    Node statement;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement.isNull) { 
            break;
        }
        list.push_back(statement);
    }

    DEBUGOUT("parseStatementList"); 
    return list;
}

//throw_
Node parseBlock() { 
    DEBUGIN(" parseBlock()");
    vector< Node > block;
    Node node(true, true);

    expect("{");
    block = parseStatementList();
    expect("}");
    node.finishBlockStatement(block);
    DEBUGOUT("parseBlock"); 
    return node;
}


// 12.2 Variable Statement

//throw_
Node parseVariableIdentifier() { 
    DEBUGIN(" parseVariableIdentifier()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    if (token.type != TknType::Identifier) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.strvalue);
    DEBUGOUT("parseVariableIdent"); 
    return node;
}

//throw_
Node parseVariableDeclaration(const string kind) { 
    DEBUGIN(" parseVariableDeclaration(u16string kind)");
    Node id(false, true), node(true, true), init(false, true);
    init = NULLNODE;

    id = parseVariableIdentifier();

    // 12.2.1
    if (strict && isRestrictedWord(id.name)) {
        throwErrorTolerant(NULLTOKEN, Messages[Mssg::StrictVarName], {});
    }

    if (kind == "const") {
        expect("=");
        init = parseAssignmentExpression();
    } else if (match("=")) {
        lex();
        init = parseAssignmentExpression();
    }

    node.finishVariableDeclarator(id, init);
    DEBUGOUT("parseVariableDecl"); 
    return node;
}

//throw_
vector< Node > parseVariableDeclarationList(const string kind) {
    vector< Node > list; 

    do {
#ifndef THROWABLE
        Node tmp = parseVariableDeclaration(kind);
        list.push_back(tmp);
#endif
#ifdef THROWABLE
        list.push_back(parseVariableDeclaration(kind));
#endif
        if (!match(",")) {
            break;
        }
        lex();
    } while (idx < length);

    DEBUGOUT("parseVariableDeclarationList"); 
    return list;
}

//throw_
Node parseVariableStatement(Node& node) { 
    DEBUGIN(" parseVariableStatement(Node node)");
    vector< Node > declarations;

    expectKeyword("var");
    declarations = parseVariableDeclarationList(""); 
    consumeSemicolon();

    node.finishVariableDeclaration(declarations, "var");
    DEBUGOUT("parseVariableStatement"); 
    return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//throw_
Node parseConstLetDeclaration(const string kind) { 
    DEBUGIN(" parseConstLetDeclaration(u16string kind)");
    vector< Node > declarations;
    Node node(true, true);

    expectKeyword(kind);
    declarations = parseVariableDeclarationList(kind);
    consumeSemicolon();
    node.finishVariableDeclaration(declarations, kind);
    DEBUGOUT("parseConstLetDeclaration"); 
    return node;
}

// 12.3 Empty Statement

//throw_
Node parseEmptyStatement() { 
    DEBUGIN(" parseEmptyStatement()");
    Node node(true, true);
    expect(";");
    node.finishEmptyStatement();
    DEBUGOUT("parseEmptyStatement"); 
    return node;
}

// 12.4 Expression Statement
//throw_
Node parseExpressionStatement(Node& node) { 
    DEBUGIN(" parseExpressionStatement(Node node)");
    Node expr(false, true);
    expr = parseExpression();
    consumeSemicolon();
    node.finishExpressionStatement(expr);
    DEBUGOUT("", false); 
    return node;
}

// 12.5 If statement
//throw_
Node parseIfStatement(Node& node) { 
    DEBUGIN(" parseIfStatement(Node node)");
    Node test(false, true), 
        consequent(false, true), alternate(false, true);
    expectKeyword("if");
    expect("(");
    test = parseExpression();
    expect(")");
    consequent = parseStatement();
    if (matchKeyword("else")) {
        lex();
        alternate = parseStatement();
    } else {
        alternate = NULLNODE;
    }
    node.finishIfStatement(test, consequent, alternate);
    DEBUGOUT("parseIfStatement"); 
    return node;
}

// 12.6 Iteration Statements

//throw_
Node parseDoWhileStatement(Node& node) { 
    DEBUGIN(" parseDoWhileStatement(Node node)");
    Node body(false, true), test(false, true);
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
        lex();
    }
    node.finishDoWhileStatement(body, test);
    DEBUGOUT("parseDoWhileStatement");
    return node;
}

//throw_
Node parseWhileStatement(Node& node) { 
    DEBUGIN(" parseWhileStatement(Node node)");
    Node test(false, true), body(false, true);
    bool oldInIteration;
    expectKeyword("while");
    expect("(");
    test = parseExpression();
    expect(")");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    node.finishWhileStatement(test, body);
    DEBUGOUT("parseWhileStatement");
    return node;
}

//throw_
Node parseForVariableDeclaration() { 
    DEBUGIN(" parseForVariableDeclaration()");
    TokenStruct token;
    vector< Node > declarations;
    Node node(true, true);

    token = lex();
    declarations = parseVariableDeclarationList("");
    node.finishVariableDeclaration(declarations, token.strvalue);
    DEBUGOUT("parseForVariableDeclaration"); 
    return node;
}

//throw_
Node parseForStatement(Node& node) { 
    DEBUGIN(" parseForStatement(Node node)");

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node body(false, true), left(false, true), right(false, true), 
        update(false, true), test(false, true), init(false, true);
    left=NULLNODE;
    update=NULLNODE; test=NULLNODE; init=NULLNODE;

    expectKeyword("for");
    expect("(");

    if (match(";")) {
        lex();
    } else {
        if (matchKeyword("var") || matchKeyword("let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (json_object_array_length(
                 json_require(init.jv,  "declarations", false)) == 1 
                && matchKeyword("in")) { 

                lex();
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
                    throwErrorTolerant(NULLTOKEN, 
                                       Messages[Mssg::InvalidLHSInForIn],{});
                }

                lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        }

        if (left.isNull) {
            expect(";");
        }
    }

    if (left.isNull) {

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

    if (left.isNull) {
        node.finishForStatement(init, test, update, body);
    } else {
        node.finishForInStatement(left, right, body);
    }
    DEBUGOUT("parseForStatement");
    return node;
}

// 12.7 The continue statement
//throw_
Node parseContinueStatement(Node& node) {
    DEBUGIN(" parseContinueStatement(Node node)");
    Node label(false, true);
    label = NULLNODE;
    string key;
    bool pltresult;

    expectKeyword("continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration)) {
            throwError(NULLTOKEN, Messages[Mssg::IllegalContinue],{});
        }

        node.finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement"); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!state.inIteration) {
            throwError(NULLTOKEN, Messages[Mssg::IllegalContinue],{});
        }

        node.finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement"); 
        return node;
    }

    if (lookahead.type == TknType::Identifier) {

        label = parseVariableIdentifier();

        key = "$";
        key.append(label.name);
        if (!(has(key, state.labelSet))) {
            throwError(NULLTOKEN, Messages[Mssg::UnknownLabel], {label.name});
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration)) {
        throwError(NULLTOKEN, Messages[Mssg::IllegalContinue],{});
    }

    node.finishContinueStatement(label);
    DEBUGOUT("parseContinueStatement");
    return node;
}

// 12.8 The break statement
//throw_
Node parseBreakStatement(Node& node) {
    DEBUGIN(" parseBreakStatement(Node node)");
    Node label(false, true);
    string key;
    bool pltresult;
    label.isNull = true;

    expectKeyword("break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages[Mssg::IllegalBreak],{});
        }

        node.finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement"); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages[Mssg::IllegalBreak],{});
        }

        node.finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement"); 
        return node;
    }

    if (lookahead.type == TknType::Identifier) {
        label = parseVariableIdentifier();

        key = "$";
        key.append(label.name);

        if (!(has(key, state.labelSet))) {
            throwError(NULLTOKEN, Messages[Mssg::UnknownLabel], {label.name});
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration || state.inSwitch)) {
        throwError(NULLTOKEN, Messages[Mssg::IllegalBreak], {});
    }

    node.finishBreakStatement(label);
    DEBUGOUT("parseBreakStatement");
    return node;
}

// 12.9 The return statement
//throw_
Node parseReturnStatement(Node& node) {
    DEBUGIN(" parseReturnStatement(Node node)");
    Node argument(false, true);
    bool pltresult;
    argument = NULLNODE;

    expectKeyword("return");

    if (!(state.inFunctionBody)) {
        throwErrorTolerant(NULLTOKEN, Messages[Mssg::IllegalReturn], {});
    }

    // 'return' followed by a space and an identifier is very common.
    if (source(idx) == 0x20) {
        if (isIdentifierStart(source(idx + 1))) {
            argument = parseExpression();
            consumeSemicolon();
            node.finishReturnStatement(argument);
            DEBUGOUT("parseReturnStatement"); 
            return node;
        }
    }

    pltresult = peekLineTerminator();
    if (pltresult) {
        node.finishReturnStatement(NULLNODE);
        DEBUGOUT("parseReturnStatement");
        return node;
    }

    if (!match(";")) {
        if (!match("}") && lookahead.type != TknType::EOFF) {
            argument = parseExpression();
        }
    }

    consumeSemicolon();

    node.finishReturnStatement(argument);
    DEBUGOUT("parseReturnStatement");
    return node;
}

// 12.10 The with statement
//throw_
Node parseWithStatement(Node& node) {
    DEBUGIN(" parseWithStatement(Node node)");
    Node object(false, true), body(false, true);

    if (strict) {
        // TODO(ikarienator): Should we update the test cases instead?
        skipComment(); //ev
        throwErrorTolerant(NULLTOKEN, 
                           Messages[Mssg::StrictModeWith], {});
    }

    expectKeyword("with");
    expect("(");
    object = parseExpression();
    expect(")");
    body = parseStatement();
    node.finishWithStatement(object, body);
    DEBUGOUT("parseWithStatement");
    return node;
}

// 12.10 The swith statement
//throw_
Node parseSwitchCase() {
    DEBUGIN(" parseSwitchCase()");
    Node test(false, true), statement(false, true), node(true, true);
    vector< Node > consequent;

    if (matchKeyword("default")) {
        lex();
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

    node.finishSwitchCase(test, consequent);
    DEBUGOUT("parseSwitchCase");
    return node;
}

//throw_
Node parseSwitchStatement(Node& node) {
    DEBUGIN(" parseSwitchStatement(Node node)");
    Node discriminant(false, true), clause(false, true); 
    vector< Node > cases; 
    bool oldInSwitch, defaultFound;

    expectKeyword("switch");
    expect("(");
    discriminant = parseExpression();
    expect(")");
    expect("{");
    if (match("}")) {
        lex();
        node.finishSwitchStatement(discriminant, cases);
        DEBUGOUT("parseSwitchStatement"); 
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
        if (json_object_is_type(
                                json_require(clause.jv,  "test", false), 
                                json_type_null)) {
            if (defaultFound) {
                throwError(NULLTOKEN, 
                           Messages[Mssg::MultipleDefaultsInSwitch],{});
            }
            defaultFound = true;
        }
        cases.push_back(clause);
    }

    state.inSwitch = oldInSwitch;
    expect("}");
    node.finishSwitchStatement(discriminant, cases);
    DEBUGOUT("parseSwitchStatement");
    return node;
}

// 12.13 The throw statement
//throw_
Node parseThrowStatement(Node& node) {
    DEBUGIN(" parseThrowStatement(Node node)");
    Node argument(false, true);
    bool pltresult;

    expectKeyword("throw");
    pltresult = peekLineTerminator();
    if (pltresult) {
        throwError(NULLTOKEN, 
                   Messages[Mssg::NewlineAfterThrow],{});
    }
    argument = parseExpression();
    consumeSemicolon();
    node.finishThrowStatement(argument);
    DEBUGOUT("parseThrowStatement");
    return node;
}

// 12.14 The try statement

//throw_
Node parseCatchClause() {
    DEBUGIN(" parseCatchClause()");
    Node body(false, true), param(false, true), node(true, true);

    expectKeyword("catch");

    expect("(");
    if (match(")")) {
        throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (strict && isRestrictedWord(param.name)) { 
        throwErrorTolerant(NULLTOKEN, 
                           Messages[Mssg::StrictCatchVariable],{});
    }

    expect(")");
    body = parseBlock();
    node.finishCatchClause(param, body);
    DEBUGOUT("parseCatchClause");
    return node;
}

//throw_
Node parseTryStatement(Node& node) {
    DEBUGIN(" parseTryStatement(Node node)");
    Node block(false, true), finalizer(false, true); 
    vector< Node > handlers;

    finalizer = NULLNODE;

    expectKeyword("try");

    block = parseBlock();

    if (matchKeyword("catch")) {
#ifndef THROWABLE
        Node tmp = parseCatchClause();
        handlers.push_back(tmp);
#endif
#ifdef THROWABLE
        handlers.push_back(parseCatchClause());
#endif
    }

    if (matchKeyword("finally")) {
        lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && finalizer.isNull) {
        throwError(NULLTOKEN, Messages[Mssg::NoCatchOrFinally], {});
    }

    node.finishTryStatement(block, EMPTY_NODE_LIST, 
                            handlers, finalizer); 
    DEBUGOUT("parseTryStatement");
    return node;
}

// 12.15 The debugger statement

//throw_
Node parseDebuggerStatement(Node& node) {
    DEBUGIN(" parseDebuggerStatement(Node node)");
    expectKeyword("debugger");
    consumeSemicolon();
    node.finishDebuggerStatement();
    DEBUGOUT("parseDebuggerStatement");
    return node;
}

// 12 Statements

//#partial
//throw_
Node parseStatement() {
    DEBUGIN(" parseStatement()");
    TknType type = lookahead.type;
    string key, tokval;
    Node expr(false, true), node(false, true), labeledBody(false, true);

    if (type == TknType::EOFF) {
        throwUnexpected(lookahead);
    }

    if (type == TknType::Punctuator && lookahead.strvalue == "{") {
        return DEBUGRET("parseStatement", parseBlock());
    }

    node.lookavailInit();

    if (type == TknType::Punctuator) {
        tokval = lookahead.strvalue;
        if (tokval == ";") {
            return DEBUGRET("parseStatement", parseEmptyStatement());
        } else if (tokval == "(") {
            return DEBUGRET("parseStatement", parseExpressionStatement(node));
        }
    } else if (type == TknType::Keyword) {
        tokval = lookahead.strvalue;
        if (tokval == "break") {
            return DEBUGRET("parseStatement", parseBreakStatement(node));
        } else if (tokval == "continue") {
            return DEBUGRET("parseStatement", parseContinueStatement(node));
        } else if (tokval == "debugger") {
            return DEBUGRET("parseStatement", parseDebuggerStatement(node));
        } else if (tokval == "do") {
            return DEBUGRET("parseStatement", parseDoWhileStatement(node));
        } else if (tokval == "for") {
            return DEBUGRET("parseStatement", parseForStatement(node));
        } else if (tokval == "function") {
            //#oddly enough in js passes node here.
            //#even though has no param even in js.
            return DEBUGRET("parseStatement", parseFunctionDeclaration());
        } else if (tokval == "if") {
            return DEBUGRET("parseStatement", parseIfStatement(node));
        } else if (tokval == "return") {
            return DEBUGRET("parseStatement", parseReturnStatement(node));
        } else if (tokval == "switch") {
            return DEBUGRET("parseStatement", parseSwitchStatement(node));
        } else if (tokval == "throw") {
            return DEBUGRET("parseStatement", parseThrowStatement(node));
        } else if (tokval == "try") {
            return DEBUGRET("parseStatement", parseTryStatement(node));
        } else if (tokval == "var") {
            return DEBUGRET("parseStatement", parseVariableStatement(node));
        } else if (tokval == "while") {
            return DEBUGRET("parseStatement", parseWhileStatement(node));
        } else if (tokval == "with") {
            return DEBUGRET("parseStatement", parseWithStatement(node));
        }
    }

    expr = parseExpression(); 

    // 12.12 Labelled Statements
    if ((expr.type == Syntax[Synt::Identifier]) && match(":")) {
        lex();

        key = "$";
        key.append(expr.name);

        if (has(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages[Mssg::Redeclaration], 
                       {"Label", expr.name}); 
        }
        state.labelSet.insert(key);
        labeledBody = parseStatement();
        state.labelSet.erase(key);
        node.finishLabeledStatement(expr, labeledBody);
        DEBUGOUT("parseStatement"); 
        return node;
    }

    consumeSemicolon();

    node.finishExpressionStatement(expr);
    DEBUGOUT("parseStatement");
    return node;
}

// 13 Function Definition

//throw_
Node parseFunctionSourceElements() {
    DEBUGIN(" parseFunctionSourceElements()");
    Node sourceElement(false, true), node(true, true);
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive;
    StateStruct oldstate;

    expect("{");

    firstRestricted.isNull = true;
    while (idx < length) {
        if (lookahead.type != TknType::StringLiteral) {
            break;
        }
        token = lookahead;

        sourceElement = parseSourceElement(); 
        //# returns in turn the value of parseStatement for stringLiteral 
        //# so returns a string literal expression node wrapped in an expressionStatement node.
        sourceElements.push_back(sourceElement); 
        if (strcmp(
                   json_object_get_string(
json_require(json_require(sourceElement.jv, "expression", false),
             "type", false)), 
                   (Syntax[Synt::Literal]).data()) != 0) {
            //? this one I doubt there's more an efficient way to do this
            //? then json-c accesses. Storing node hierarchies just to fix this call seems to 
            //? be likely less performant.
            // this is not directive
            break;
        }
        directive = slice(sourceraw, token.start + 1, token.end - 1);
        if (directive == u"use strict") {
            strict = true;
            if (!(firstRestricted.isNull)) {
                throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral], {});
            }
        } else {
            if (firstRestricted.isNull && token.octal) {
                firstRestricted = token;
                firstRestricted.isNull = false;
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
        if (sourceElement.isNull) {
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

    node.finishBlockStatement(sourceElements);
    DEBUGOUT("parseFunctionSourceElements");
    return node;
}

//throw_ 
void validateParam(ParseParamsOptions& options, 
                    TokenStruct param, const string name) {
     DEBUGIN("validateParam");
     string key = "$";
     key.append(name);
     if (strict) {
         if (isRestrictedWord(name)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamName];
         }

         if (has(key, options.paramSet)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     } else if (options.firstRestricted.isNull) {
         if (isRestrictedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamName];
         } else if (isStrictModeReservedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictReservedWord];
         } else if (has(key, options.paramSet)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     }
     options.paramSet.insert(key);
    DEBUGOUT("validateParam");
    return;
}


//throw_ 
bool parseParam(ParseParamsOptions& options) {
    DEBUGIN(" parseParam(ParseParamsOptions options)");
    TokenStruct token; 
    Node param(false, true), def(false, true);

    token = lookahead;
    param = parseVariableIdentifier();
    //printf("token strvalue %s \n", (toU8string(token.strvalue)).data());
    //! MAJOR CONCERN: if a number is put where an argument is here,
    //! will that cause an ungraceful crash because validateParam is expecting
    //! a string so we cast it here, but the token grabbed might be
    //! a numeric literal?? if so perhaps the thing to do is check
    //! for token.type and if it is of numeric types cast it?
    validateParam(options, token, token.strvalue);

    if (match("=")) {
        lex();
        def = parseAssignmentExpression();
        ++(options.defaultCount);
    }

    options.params.push_back(param);
    options.defaults.push_back(def);

    return DEBUGRET("parseParam", !match(")"));
}

//throw_ 
ParseParamsOut parseParams(TokenStruct firstRestricted) {
    DEBUGIN(" parseParamS(TokenStruct firstRestricted)");
    ParseParamsOptions options;
    ParseParamsOut out;
    options.defaultCount = 0;
    options.firstRestricted = firstRestricted;
    options.stricted.isNull = true;

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
    DEBUGOUT("parseParamS");
    return out;
}

//throw_ 
Node parseFunctionDeclaration() {
    DEBUGIN(" parseFunctionDeclaration()");
    TokenStruct token, firstRestricted, stricted;
    string message;
    Node body(false, true), id(false, true), node(true, true);    
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    bool previousStrict;
    id = NULLNODE;

    expectKeyword("function");

    token = lookahead;
    id = parseVariableIdentifier(); 

    firstRestricted.isNull = true;
    if (strict) {
        if (isRestrictedWord(token.strvalue)) {
            throwErrorTolerant(token, Messages[Mssg::StrictFunctionName], {});
        }
    } else {
        if (isRestrictedWord(token.strvalue)) {
            firstRestricted = token;
            message = Messages[Mssg::StrictFunctionName];
        } else if (isStrictModeReservedWord(token.strvalue)) {
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
    if (strict && !(firstRestricted.isNull)) {
        throwError(firstRestricted, message, {});
    }
    if (strict && !(stricted.isNull)) {
        throwErrorTolerant(stricted, message, {});
    }
    strict = previousStrict;

    node.finishFunctionDeclaration(id, params, defaults, body);
    DEBUGOUT("parseFunctionDecl");
    return node;
} 

//throw_ 
Node parseFunctionExpression() {
    DEBUGIN(" parseFunctionExpression()");
    TokenStruct token, firstRestricted, stricted;
    firstRestricted.isNull = true;
    string message, tokval;
    Node body(false, true), id(false, true), node(true, true);    
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    bool previousStrict;


    id = NULLNODE;
    expectKeyword("function");

    if (!match("(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 
        tokval = token.strvalue;
        if (strict) {
            if (isRestrictedWord(tokval)) {
                throwErrorTolerant(token, 
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
    if (strict && !(firstRestricted.isNull)) {
        throwError(firstRestricted, message, {});
    }
    if (strict && !(stricted.isNull)) {
        throwErrorTolerant(stricted, message, {});
    }
    strict = previousStrict;

    node.finishFunctionExpression(id, params, defaults, body);

    DEBUGOUT("parseFuncExpr");
    return node;
}

    // 14 Program

//throw_ 
Node parseSourceElement() {
    DEBUGIN(" parseSourceElement()");
    string val;
    if (lookahead.type == TknType::Keyword) {
        val = lookahead.strvalue;
        if (val == "const" || val == "let") {
            return DEBUGRET("", parseConstLetDeclaration(val));
        } else if (val == "function") {
            return DEBUGRET("", parseFunctionDeclaration()); 
        } else {

            return DEBUGRET("", parseStatement());
        }
    }

    if (lookahead.type != TknType::EOFF) {
        return DEBUGRET("", parseStatement());
    }

    DEBUGOUT("parseSourceElement");
    return NULLNODE;
}

//throw_ 
vector< Node > parseSourceElements() {
    DEBUGIN(" parseSourceElementS() ");
    Node sourceElement(false, true);
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive;

    firstRestricted.isNull = true;
    while (idx < length) {
        token = lookahead;
        if (token.type != TknType::StringLiteral) {
            break;
        }

        sourceElement = parseSourceElement();
        sourceElements.push_back(sourceElement);
        //#todo make a function that accepts vector of nested finds
        //#so we can make tests like this more legible.
        if (strcmp(json_object_get_string(
json_require(json_require(sourceElement.jv, "expression", false), 
                                                       "type", false)), 
                   (Syntax[Synt::Literal]).data()) != 0) {         
            // this is not directive
            break;
        }
        directive = slice(sourceraw, token.start + 1, token.end - 1);
        if (directive == u"use strict") {
            strict = true;

            if (!(firstRestricted.isNull)) { 

                throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral],{});
            }
        } else {
            if (firstRestricted.isNull && token.octal) {
                firstRestricted = token;
                firstRestricted.isNull = false; //#probably not neces.
            }
        }
    }

    while (idx < length) {
        sourceElement = parseSourceElement();

        if (sourceElement.isNull) {
            break;
        }
        sourceElements.push_back(sourceElement);
    }

    DEBUGOUT("parseSourceElementS");
    return sourceElements;
}

//throw_ 
Node parseProgram() {
    DEBUGIN(" parseProgram()");
    Node node(false, true);
    vector< Node > body;

    skipComment(); //ev
    peek();
    node.lookavailInit();
    strict = false;
    body = parseSourceElements();
    node.finishProgram(body);

    DEBUGOUT("parseProgram");
    return node;
}

//throw_ 
void filterTokenLocation() {
    DEBUGIN(" filterTokenLocation()");
    int i;
    TokenRecord token, entry;
    vector<TokenRecord> tokens;

    for (i = 0; i < extra.tokenRecords.size(); ++i) {
        entry = extra.tokenRecords[i];
        token.typestring = entry.typestring;
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

    DEBUGOUT("filterToken");    
    return;
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
 
json_object* tokenizeImpl(const u16string code, 
                          OptionsStruct options,
                          const bool retErrorsAsJson) { 
    vector<TokenRecord> tokens;
    json_object *outJson = json_newmap();


    initglobals();
    sourceraw = code.data();
    idx = 0;
    lineNumber = (code.size() > 0) ? 1 : 0;
    lineStart = 0;
    length = code.length();
    lookahead = NULLTOKEN;

    state.allowIn = true;
    state.inFunctionBody = false;
    state.inIteration = false;
    state.inSwitch = false;
    state.lastCommentStart = -1;
    //? parenthesisCount for state not provided here normally as in parse. 
    //? That going to be a problem for us later?

    // Of course we collect tokens here.
    options.tokens = true;

    extra = ExtraStruct();
    extra.tokenTracking = true; 
    extra.tokenize = true;
    // The following two fields are necessary to compute the Regex tokens.
    extra.openParenToken = -1;
    extra.openCurlyToken = -1;

    extra.range = options.range; 
    extra.loc = options.loc;
    extra.commentTracking = options.comment;
    extra.errorTolerant = options.tolerant;


#ifndef THROWABLE
    ErrWrapint tmp = peek();
    if (tmp.err) {
        if (!extra.errorTolerant) {
            json_object_put(outJson);
            if (errorType == 0) {
                return retError.toJson();
            }
            return retAssertError.toJson();
        }
    }
#endif
#ifdef THROWABLE
    peek();
#endif

    if (lookahead.type == TknType::EOFF) {
        json_put(outJson, "tokenlist", 
                 vec2jsonCallback<TokenRecord>(extra.tokenRecords, 
                       &TokenRecord::toJson));
      return outJson;
    }

    lex();
    while (lookahead.type != TknType::EOFF) {
#ifndef THROWABLE
        TokenStruct out = lex();
        if (out.err) { 
            if (extra.errorTolerant) {
                extra.errors.push_back(retError); 
                break;
            } else {
                json_object_put(outJson);
                if (errorType == 0) {
                    return retError.toJson();
                }
                return retAssertError.toJson();   
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
                    json_object_put(outJson);
                    return e.toJson();
                }
                throw e;
            }
        }
#endif
    }

    filterTokenLocation();
    json_put(outJson, "tokenlist", 
             vec2jsonCallback<TokenRecord>(extra.tokenRecords, 
                                                    &TokenRecord::toJson)); 
    if (extra.commentTracking) {
        json_put(outJson, "comments", 
                 vec2jsonCallback<Comment>(extra.comments,
                                               &Comment::toJson));
    }
    if (extra.errorTolerant) {
        json_put(outJson, "errors",  
                 vec2jsonCallback<ExError>(extra.errors,
                                             &ExError::toJsonTolerant));
    }

  return outJson;
}


json_object*  tokenize(const u16string code, const OptionsStruct options) { 
    return tokenizeImpl(code, options, false);
}
json_object*  tokenize(const string code, const OptionsStruct options) { 
    return tokenizeImpl(toU16string(code), options, false);
}
json_object*  tokenize(const string code) { 
    OptionsStruct o;
    return tokenizeImpl(toU16string(code), o, false);
}
json_object*  tokenize(const u16string code) { 
    OptionsStruct o;
    return tokenizeImpl(code, o, false);
}

string tokenizeRetString(const u16string code, const OptionsStruct options){
    json_object * m = tokenizeImpl(code, options, true);
    string result = json_object_to_json_string_ext(
                                 m, JSON_C_TO_STRING_PLAIN); 
    json_object_put(m);
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

json_object* parseImpl(const u16string code, 
                       OptionsStruct options, //# nonconst 1:1
                       const bool retErrorsAsJson) { 

    Node programNode;
    json_object * programJson = json_newmap();

    initglobals();

    sourceraw = code.data();

    idx = 0;
    lineNumber = (code.size() > 0) ? 1 : 0;
    lineStart = 0;
    length = code.length();
    lookahead = NULLTOKEN;
    state.allowIn = true;
    state.parenthesisCount = 0;
    state.inFunctionBody = false;
    state.inIteration = false;
    state.inSwitch = false;
    state.lastCommentStart = -1;

    extra = ExtraStruct();

    extra.range = options.range;
    extra.loc = options.loc;
    extra.attachComment = options.attachComment;

    if (extra.loc && options.hasSource) {
        extra.hasSource = true;
        extra.source = options.source;
    } else { extra.hasSource = false; }

    extra.tokenTracking = options.tokens;
    extra.commentTracking = options.comment;
    extra.errorTolerant = options.tolerant;

    //values which aren't strictly dependent on attachComment being true
    //but attachComment is sufficient for them to be true.
    if (extra.attachComment) {
        extra.range = true;
        extra.commentTracking = true;
    }

#ifndef THROWABLE
    programNode = parseProgram();
    if (programNode.err) {
        json_object_put(programJson);
        if (errorType == 0) {
            return retError.toJson();
        }
        return retAssertError.toJson();        
    }
#endif
#ifdef THROWABLE
    try {
        programNode = parseProgram();
    } catch(ExError& e) {        
        if (retErrorsAsJson) {
            json_object_put(programJson);
            return e.toJson();

        }
        throw e;
    }
#endif
   json_put(programJson, "program", programNode.jv);
   json_put(programJson, "regexp", programNode.regexPaths2json());


   if (extra.commentTracking) {
       json_put(programJson, "comments", 
                vec2jsonCallback<Comment>(extra.comments,
                                          &Comment::toJson)); 
   }

   if (extra.tokenTracking) {
       filterTokenLocation();
       json_put(programJson, "tokens", 
                vec2jsonCallback<TokenRecord>(extra.tokenRecords,
                                              &TokenRecord::toJson));
   }

   if (extra.errorTolerant) {
       json_put(programJson, "errors", 
                vec2jsonCallback<ExError>(extra.errors,
                                          &ExError::toJsonTolerant));
   }


   extra = ExtraStruct();

 return programJson;
}
json_object*  parse(const u16string code, OptionsStruct options) {    
    return parseImpl(code, options, false);
}
json_object*  parse(const string code, OptionsStruct options) {    
    return parseImpl(toU16string(code), options, false);
}
json_object*  parse(const string code) { 
    OptionsStruct o;
    return parseImpl(toU16string(code), o, false);
}
json_object*  parse(const u16string code) { 
    OptionsStruct o;
    return parseImpl(code, o, false);
}

//# return json as string.
string parseRetString(const u16string code, OptionsStruct options) {    
    json_object * m = parseImpl(code, options, true);
    string result = json_object_to_json_string_ext(
                     m, JSON_C_TO_STRING_PRETTY); 
    json_object_put(m);
    return result; 
}
string parseRetString(const string code, OptionsStruct options) { 
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


int main() {
    string somecode = "done: while (true) { continue done }";

    string someopt = "{ 'loc':true, 'range':true, 'tokens':true }";
    string result = string(parseExtern(somecode.data(), someopt.data()));
    result.append("\n");
    printf("%s", result.data());
}
