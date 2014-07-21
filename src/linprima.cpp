#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <regex>
#include <functional>
#include <unordered_set>
#include <memory>
#include <exception>

//#include "json-c/json.h"
//#include "json-c/json_object.h"
//#include "json-c/printbuf.h"
//algorithm is for find(vector.begin()
using namespace std;

int debuglevel = 0;
vector<string> stackfuncs;

static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}


bool DEBUG_ON= (bool) 0;
bool HIPRI_ONLY= (bool) 1;

void DEBUGIN(string in, bool lowprio) {    
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    debuglevel++;
    string msg = "";

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }    
    if (lowprio) { msg.append("\033[1;30m"); } 
    msg.append(ltrim(in));
    if (lowprio) { msg.append("\033[0m"); } 
    msg.append("\n");
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

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }
    if (lowprio) { msg.append("\033[1;30m"); } 
    msg.append("~");
    //    msg.append(ltrim(in));
    if (stackfuncs.size() > 0) {
        //    msg.append("@@ ");
        msg.append(stackfuncs.back());
    }
    if (lowprio) { msg.append("\033[0m"); } 
    msg.append("\n");
    printf("%s", msg.data());

    stackfuncs.pop_back();
    debuglevel--;
}
void DEBUGOUT(string in) {
    DEBUGOUT(in, false);
}

void DEBUGOUT() {
    DEBUGOUT("", false);
}


string toU8string(u16string input){ 
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;
  return myconv.to_bytes(input);
}

string toU8string(char16_t* input) {
  return toU8string(u16string(input));
}
wstring toWstring(string input) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(input);
}

u16string toU16string(string input){ 
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
inline json_object* json_put(json_object *a, const char *b, u16string c) 
{ return json_put(a,b,toU8string(c)); }

json_object* json_put(json_object *a, const char *b, int c) { 
    json_object *tmp = json_object_new_double(c);
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_put(json_object *a, const char *b, bool c) { 
    json_object *tmp = json_object_new_boolean(c?1:0);
    json_object_object_add(a, b, tmp);
   return tmp;
}
json_object* json_put(json_object *a, const char *b, json_object* c) { 
    json_object_object_add(a, b, c);
   return c;
}

json_object* json_push(json_object *a, string c) { 
    json_object *tmp = json_object_new_string(c.data());
    json_object_array_add(a, tmp);
   return tmp;
}
inline json_object* json_push(json_object *a, u16string c) { return json_push(a,toU8string(c)); }
json_object* json_push(json_object *a, int c) { 
    json_object *tmp = json_object_new_double(c);
    json_object_array_add(a, tmp);
   return tmp;
}
json_object* json_push(json_object *a, bool c) { 
    json_object *tmp = json_object_new_boolean(c?1:0);
    json_object_array_add(a, tmp);
   return tmp;
}
json_object* json_push(json_object *a, json_object *c) { 
    json_object_array_add(a, c);
   return c;
}

json_object* json_find(json_object *a, const char* eqkey) { 
    json_object_object_foreach(a, jkey, jval) {
        if (strcmp(jkey, eqkey) == 0) {
           return json_object_get(jval); //increment the reference count, so no segfault if you drop the owner node.
        }
    }    
   return nullptr;
}

json_object* json_require(json_object *a, const char* eqkey) { 
    json_object * out = json_find(a, eqkey);
    if (out == nullptr) {
        string errormsg = "json_find failed to find key : ";
        errormsg.append(string(eqkey));
        throw runtime_error(errormsg);
    }
   return out;
}

json_object* json_arrfind(json_object* a, int idx) { 
    if (idx >= json_object_array_length(a)) {
        throw runtime_error("json_arrIdx was asked to find an index outside of the array bounds");
    }
   return json_object_array_get_idx(a, idx);
}

void json_arrput(json_object* a, int idx, json_object *c) {        
    if (idx > json_object_array_length(a)) {
        throw runtime_error("json_arrIdx was asked to set an index higher than array length");
    }
    json_object_array_put_idx(a, idx, c);
 
}

void json_del(json_object *a, const char* key) { 
    if (json_find(a, key) != nullptr) {
        json_object_object_del(a, key);
    }
    
}

// we could make this generic, but practically it's only valid for overrides of json_push
// that 
template <typename T>
json_object* vec2json(vector<T> in) { //only practically valid for vectors of ints and strings.
        DEBUGIN("vec2json");
    json_object * arr = json_object_new_array();
    for (int i=0; i<in.size(); i++) {
        json_push(arr, in[i]);
    }
  DEBUGOUT(); return arr;
}

template <typename T>
json_object* vec2jsonCallback(vector<T> in,
                              function<json_object*(T&)> const& f) {
        DEBUGIN("vec2JsonCallback");
    json_object * arr = json_object_new_array();
    for (int i=0; i<in.size(); i++) {
        json_push(arr, f(in[i]));
    }
  DEBUGOUT(); return arr;
}

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
template<typename T> bool has(const T needle, const unordered_set<T> haystack) {
        DEBUGIN("has");
    auto result = haystack.find(needle);
  DEBUGOUT(); return (result != haystack.end());
}
template<typename T> bool hasStringKey(const string needle, const map<string,T> haystack) {
        DEBUGIN("hasStringKey");
    auto result = haystack.find(needle);
  DEBUGOUT(); return (result != haystack.end());
}



u16string res_u16(void* pos) {
        DEBUGIN("res_u16");
  DEBUGOUT(); return *((u16string *) pos);
}

u16string slice(const char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
        DEBUGIN("slice");
    const char16_t * startptr = arr + start;    
  DEBUGOUT(); return u16string(startptr, (end-start));
}

void append(u16string &base, char16_t tail) { DEBUGIN(" append(u16string &base, char16_t tail)");
    base.append(u16string({tail})); 
    //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.
    DEBUGOUT();
}


int parseInt(u16string in_u16, int radix) {  // !!!
        DEBUGIN("parseInt");
        string in = toU8string(in_u16);
        int length, 
            result = 0;
        length = in.length();
        for (int i=0; i<length; i++) {
            result += (((int) in[i]) * pow(radix,length-(i+1)));
        }
  DEBUGOUT(); return result; 
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
    u16string value;
    u16string literal;
    int start;
    int end;
    RegexHalf() { start = -1; end = -1; }
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

//#CLEAR 
json_object*  posToJson(Position p) { DEBUGIN(" posToJson(Position p)");
    json_object * root = json_newmap();
    json_put(root, "line", p.line);
    json_put(root, "column", p.column);
  DEBUGOUT("posToJSon(Position)"); return root;
}

json_object*  locToJson(Loc l) { DEBUGIN(" locToJson(Loc l)");
    json_object * root = json_newmap();
    json_put(root, "start", posToJson(l.start));
    if (l.end.line != -1) {
        json_put(root, "end", posToJson(l.end));
    }
    if (l.hasSource) {
        json_put(root, "source", l.source.data());
    }
  DEBUGOUT("locToJson"); return root;
}

struct Comment {
    u16string type;
    u16string value;
    int range[2];
    Loc loc;
    Comment() {
        DEBUGIN("Comment()");
        this->type = -1;
        this->value = u"";
        this->range[0] = -1;
        this->range[1] = -1;
        DEBUGOUT("Comment()");
    }
    json_object * toJson() {
        DEBUGIN("Comment::toJson");
        json_object *root = json_newmap(), *rangearr = json_newarr();
        json_put(root, "type", toU8string(this->type));
        json_put(root, "value", toU8string(this->value));
        json_push(rangearr, this->range[0]);
        json_push(rangearr, this->range[1]);
        json_put(root, "range", rangearr);
        json_put(root, "loc", locToJson(this->loc));
      DEBUGOUT("comment::toJson"); return root;
    }
};

//# called ExError to prevent forseeable 
//# exception-handling namespace conflict.
class ExError {
public:

    u16string description;
    int index;
    int lineNumber;
    int column;
    ExError() {
    }
    json_object * toJson() {
                DEBUGIN("Error::toJSON");
        json_object * root = json_newmap();
        json_put(root, "description", toU8string(description));
        json_put(root, "index", this->index);
        json_put(root, "lineNumber", this->lineNumber);
        json_put(root, "column", this->column);
      DEBUGOUT("Error::toJSON"); return root;
    }
};


//used in initial scannig
struct TokenStruct {
    bool isNull; 
    int type;
    u16string strvalue;
    double dblvalue;
    int intvalue;
    bool bvalue;

    int literaltype; //lin only.

    u16string literal; //regex literal only
    u16string flags; //regex literal only

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
        isNull = false;
        type = -1;
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
    u16string valuestring;
    u16string typestring;
    TokenRecord() {
        DEBUGIN("TokenRecord()", true);
        range[0] = -1;
        range[1] = -1;
        DEBUGOUT("TokenRecord()", true);
    }
    json_object * toJson();
};


class Node {
public:
    bool isNull;
    u16string type;
    bool hasJv;
    json_object * jv;
    Loc loc;
    
    bool hasLoc;
    bool hasRange;
    int range[2];
    vector<Comment> trailingComments;
    vector<Comment> leadingComments;
    vector< vector<string> > regexPaths; //lin only. obv.



    u16string name;//for identifiers
    vector< Node > expressions; //for sequence expressions.
    shared_ptr<Node> left; //for assignment+reinterpretAsCover...
    shared_ptr<Node> right; //same

    string s(u16string in);
    Node();
    Node(bool lookaheadAvail, bool storeStats);
    json_object* toJson();
    void lookavailInit();
    void clear();
    void unused();
    void jvput(string path, string b);
    void jvput(string path, int b); 
    void jvput(string path, bool b);
    void jvput_dbl(string path, string b);
    void jvput_null(string path); 
    void regNoadd(vector<string> paths, Node&child);
    void reg(string path, Node& child);
    void nodeVec(string path, vector<Node>& nodes);
    void addType(string in);
    json_object* regexPaths2json();
    void commentsIntoJson(const bool leading);
    void processComment();
    void finish();
    void finishArrayExpression(vector< Node >& elements);
    void finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression);
    void finishAssignmentExpression(u16string oper, Node& left, Node& right);
    void finishBinaryExpression(u16string oper, Node& left, Node& right);
    void finishBlockStatement(vector< Node >& body);
    void finishBreakStatement(Node& label);
    void finishCallExpression(Node& callee, vector< Node >& args);
    void finishCatchClause(Node& param, Node& body);
    void finishConditionalExpression(Node& test, Node& consequent, Node& alternate);
    void finishContinueStatement(Node& label);
    void finishDebuggerStatement();
    void finishDoWhileStatement(Node& body, Node& test);
    void finishEmptyStatement();
    void finishExpressionStatement(Node expression);
    void finishForStatement(Node& init, Node& test, Node& update, Node& body);
    void finishForInStatement(Node& left, Node& right, Node& body);
    void finishFunctionDeclaration(Node& id, vector< Node >& params, 
                                   vector< Node >& defaults, Node& body);
    void finishFunctionExpression(Node& id, vector< Node >& params, 
                                  vector< Node >& defaults, Node& body);
    void finishIdentifier(u16string name);
    void finishIfStatement(Node& test, Node& consequent, Node& alternate); 
    void finishLabeledStatement(Node label, Node body);
    void finishLiteral(TokenStruct token);
    void finishMemberExpression(char16_t accessor, Node& object, Node& property);
    void finishNewExpression(Node& callee, vector<Node>& args);
    void finishObjectExpression(vector<Node>& properties);
    void finishPostfixExpression(u16string oper, Node& argument);
    void finishProgram(vector<Node>& body);
    void finishProperty(u16string kind, Node& key, Node& value);
    void finishReturnStatement(Node& argument);
    void finishSequenceExpression(vector<Node>& expressions);
    void finishSwitchCase(Node& test, vector<Node>& consequent);
    void finishSwitchStatement(Node& discriminant, vector<Node>& cases);
    void finishThisExpression();
    void finishThrowStatement(Node& argument);
    void finishTryStatement(Node& block, vector<Node>& guardedHandlers, 
                            vector<Node>& handlers, Node& finalizer);
    void finishUnaryExpression(u16string oper, Node& argument);
    void finishVariableDeclaration(vector<Node>& declarations, u16string kind);
    void finishVariableDeclarator(Node& id, Node& init);
    void finishWhileStatement(Node& test, Node& body);
    void finishWithStatement(Node& object, Node& body);
    
}; 



TokenStruct NULLTOKEN;
Node NULLNODE(false,false);

struct ExtraStruct {
    bool tokenTracking; //port-specific member to replace "if (extra.tokens)"
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

    bool commentTracking; //port-specific member to replace "if (extra.comments)"
    vector<Comment> comments;

    bool errorTolerant;  //port specific member to replace "if (extra.errors)"
    vector<ExError> errors;

    bool attachComment;
    vector<Comment> leadingComments;
    vector<Comment> trailingComments;
    vector< Node * > bottomRightStack; //! todo Node header text.

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
    unordered_set<u16string> labelSet;
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
        DEBUGIN("OptionsStruct()");
        range = false;
        loc = false;
        comment = false;
        tolerant = false;
        attachComment = false;
        tokens = false;
        hasSource = false;
        DEBUGOUT("OptionsStruct()");
    }
    bool json_getbool(json_object* in, string key, bool defaultVal) {
        json_object* tmp = json_find(in, key.data());
        if (tmp == nullptr) { return defaultVal; }
        bool result = (bool) json_object_get_boolean(tmp);
        json_object_put(tmp);
        return result;
    }
    OptionsStruct(const char *in_o) {
        DEBUGIN("OptionsStruct(char*)");
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
            tmp = json_find(in, "source");        
            hasSource = (tmp != nullptr);
            if (hasSource) { 
                source = json_object_get_string(tmp); 
                json_object_put(tmp);
            }
        }
        json_object_put(in);
        DEBUGOUT("OptionsStruct(char*)");
    }
};

//#CLEAR
struct ParseParamsOptions {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    TokenStruct firstRestricted;
    TokenStruct stricted;
    unordered_set<u16string> paramSet;
    u16string message;
};


//#CLEAR
struct ParseParamsOut {
    TokenStruct firstRestricted;
    TokenStruct stricted;
    u16string message;
    vector< Node > params;
    vector< Node > defaults;
    ParseParamsOut() {
        message=u"";
    }
};


struct ReinterpretOptions {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    Node firstRestricted;
    Node stricted;
    unordered_set<u16string> paramSet;
    u16string message;
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
    u16string message;
    vector< Node > params;
    vector< Node > defaults;
    void* rest; //seems to be a dummy var?
    ReinterpretOut() {
        isNull=false;
        firstRestricted = NULLNODE;
        stricted = NULLNODE;
    }
};

class SciNoteHandler {

private:
    string src;
    int intrep;
    double dblrep;
    bool _hasVal;  
    bool _canInt;

public:
    SciNoteHandler() {
        _hasVal = false;
        _canInt = false;
    }
    bool hasVal() { return _hasVal; }
    bool canInt() { return _canInt; }
    void process(string in) {
        intrep = -1;
        dblrep = -1;
        src = in;
    }
    double asDouble() {
        DEBUGIN(".");
      DEBUGOUT(); return dblrep;
    }
    int asInt() {
        DEBUGIN(".");
      DEBUGOUT(); return intrep;
    }
};
//---- ----------  -----------------------------
// signatures (temporary until we set up a header file):

void throwError(TokenStruct token, u16string messageFormat, 
                vector<u16string> args);
void throwErrorTolerant(TokenStruct token, u16string messageFormat, 
                        vector<u16string> args);
void throwUnexpected(TokenStruct token);

template<typename T> T DEBUGRET(string a, T b) { DEBUGOUT(a); return b; }

OptionsStruct options;
ExtraStruct extra;
StateStruct state;
TokenStruct lookahead;

const char16_t * sourceraw;
char16_t source(int idx) { return *(sourceraw + idx); }

json_object*  TokenRecord::toJson() { DEBUGIN(" TokenRecord::toJson()");
    json_object *root = json_newmap(), *rangearr;
    json_put(root, "type", toU8string(this->typestring));
    json_put(root, "value", toU8string(this->valuestring));
    if (extra.range) {
        rangearr = json_newarr();
        json_push(rangearr, this->range[0]);
        json_push(rangearr, this->range[1]);
        json_put(root, "range", rangearr);
    }
    if (extra.loc) {
        json_put(root, "loc", locToJson(this->loc));
    }
  DEBUGOUT("TokenRecord::toJson()"); return root;
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

map<string, int> Token = {
    {"BooleanLiteral", 1},
    {"EOF", 2},
    {"Identifier", 3},
    {"Keyword", 4},
    {"NullLiteral", 5},
    {"NumericLiteral", 6},
    {"Punctuator", 7},
    {"StringLiteral", 8},
    {"RegularExpression", 9}
};
  
map<int, u16string> TokenName = {
    {Token["BooleanLiteral"], u"Boolean"},
    {Token["EOF"], u"<end>"},
    {Token["Identifier"], u"Identifier"},
    {Token["Keyword"], u"Keyword"},
    {Token["NullLiteral"], u"Null"},
    {Token["NumericLiteral"], u"Numeric"},
    {Token["Punctuator"], u"Punctuator"},
    {Token["StringLiteral"], u"String"},
    {Token["RegularExrpession"], u"RegularExpression"}
};


unordered_set< u16string > FnExprTokens = {
    // A function following one of those tokens is an expression.                                                
    u"(", u"{", u"[", u"in", u"typeof", u"instanceof", u"new",
    u"return", u"case", u"delete", u"throw", u"void",
    // assignment operators                                                                      
    u"=", u"+=", u"-=", u"*=", u"/=", u"%=", u"<<=", u">>=", u">>>=",
    u"&=", u"|=", u"^=", u",",
    // binary/unary operators                                                                    
    u"+", u"-", u"*", u"/", u"%", u"++", u"--", u"<<", u">>", u">>>", u"&",
    u"|", u"^", u"!", u"~", u"&&", u"||", u"?", u":", u"===", u"==", u">=",
    u"<=", u"<", u">", u"!=", u"!=="
};

map<string, u16string> Syntax = {
    {"AssignmentExpression", u"AssignmentExpression"},
    {"ArrayExpression", u"ArrayExpression"},
    {"ArrowFunctionExpression", u"ArrowFunctionExpression"},
    {"BlockStatement", u"BlockStatement"},
    {"BinaryExpression", u"BinaryExpression"},
    {"BreakStatement", u"BreakStatement"},
    {"CallExpression", u"CallExpression"},
    {"CatchClause", u"CatchClause"},
    {"ConditionalExpression", u"ConditionalExpression"},
    {"ContinueStatement", u"ContinueStatement"},
    {"DoWhileStatement", u"DoWhileStatement"},
    {"DebuggerStatement", u"DebuggerStatement"},
    {"EmptyStatement", u"EmptyStatement"},
    {"ExpressionStatement", u"ExpressionStatement"},
    {"ForStatement", u"ForStatement"},
    {"ForInStatement", u"ForInStatement"},
    {"FunctionDeclaration", u"FunctionDeclaration"},
    {"FunctionExpression", u"FunctionExpression"},
    {"Identifier", u"Identifier"},
    {"IfStatement", u"IfStatement"},
    {"Literal", u"Literal"},
    {"LabeledStatement", u"LabeledStatement"},
    {"LogicalExpression", u"LogicalExpression"},
    {"MemberExpression", u"MemberExpression"},
    {"NewExpression", u"NewExpression"},
    {"ObjectExpression", u"ObjectExpression"},
    {"Program", u"Program"},
    {"Property", u"Property"},
    {"ReturnStatement", u"ReturnStatement"},
    {"SequenceExpression", u"SequenceExpression"},
    {"SwitchStatement", u"SwitchStatement"},
    {"SwitchCase", u"SwitchCase"},
    {"ThisExpression", u"ThisExpression"},
    {"ThrowStatement", u"ThrowStatement"},
    {"TryStatement", u"TryStatement"},
    {"UnaryExpression", u"UnaryExpression"},
    {"UpdateExpression", u"UpdateExpression"},
    {"VariableDeclaration", u"VariableDeclaration"},
    {"VariableDeclarator", u"VariableDeclarator"},
    {"WhileStatement", u"WhileStatement"},
    {"WithStatement", u"WithStatement"}
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
map<string, u16string> Messages = {
    {"UnexpectedToken", u"Unexpected token %0"},
    {"UnexpectedNumber", u"Unexpected number"},
    {"UnexpectedString", u"Unexpected string"},
    {"UnexpectedIdentifier", u"Unexpected identifier"},
    {"UnexpectedReserved", u"Unexpected reserved word"},
    {"UnexpectedEOS", u"Unexpected end of input"},
    {"NewlineAfterThrow", u"Illegal newline after throw"},
    {"InvalidRegExp", u"Invalid regular expression"},
    {"UnterminatedRegExp", u"Invalid regular expression: missing /"},
    {"InvalidLHSInAssignment", u"Invalid left-hand side in assignment"},
    {"InvalidLHSInForIn", u"Invalid left-hand side in for-in"},
    {"MultipleDefaultsInSwitch", u"More than one default clause in switch statement"},
    {"NoCatchOrFinally", u"Missing catch or finally after try"},
    {"UnknownLabel", u"Undefined label \'%0\'"},
    {"Redeclaration", u"%0 \'%1\' has already been declared"},
    {"IllegalContinue", u"Illegal continue statement"},
    {"IllegalBreak", u"Illegal break statement"},
    {"IllegalReturn", u"Illegal return statement"},
    {"StrictModeWith", u"Strict mode code may not include a with statement"},
    {"StrictCatchVariable", u"Catch variable may not be eval or arguments in strict mode"},
    {"StrictVarName", u"Variable name may not be eval or arguments in strict mode"},
    {"StrictParamName", u"Parameter name eval or arguments is not allowed in strict mode"},
    {"StrictParamDupe", u"Strict mode function may not have duplicate parameter names"},
    {"StrictFunctionName", u"Function name may not be eval or arguments in strict mode"},
    {"StrictOctalLiteral", u"Octal literals are not allowed in strict mode."},
    {"StrictDelete", u"Delete of an unqualified identifier in strict mode."},
    {"StrictDuplicateProperty", u"Duplicate data property in object literal not allowed in strict mode"},
    {"AccessorDataProperty", u"Object literal may not have data and accessor property with the same name"},
    {"AccessorGetSet", u"Object literal may not have multiple get/set accessors with the same name"},
    {"StrictLHSAssignment", u"Assignment to eval or arguments is not allowed in strict mode"},
    {"StrictLHSPostfix", u"Postfix increment/decrement may not have eval or arguments operand in strict mode"},
    {"StrictLHSPrefix", u"Prefix increment/decrement may not have eval or arguments operand in strict mode"},
    {"StrictReservedWord", u"Use of future reserved word in strict mode"}
};

regex NonAsciiIdentifierStart (toU8string(u"[\xAA\xB5\xBA\xC0-\xD6\xD8-\xF6\xF8-\u02C1\u02C6-\u02D1\u02E0-\u02E4\u02EC\u02EE\u0370-\u0374\u0376\u0377\u037A-\u037D\u037F\u0386\u0388-\u038A\u038C\u038E-\u03A1\u03A3-\u03F5\u03F7-\u0481\u048A-\u052F\u0531-\u0556\u0559\u0561-\u0587\u05D0-\u05EA\u05F0-\u05F2\u0620-\u064A\u066E\u066F\u0671-\u06D3\u06D5\u06E5\u06E6\u06EE\u06EF\u06FA-\u06FC\u06FF\u0710\u0712-\u072F\u074D-\u07A5\u07B1\u07CA-\u07EA\u07F4\u07F5\u07FA\u0800-\u0815\u081A\u0824\u0828\u0840-\u0858\u08A0-\u08B2\u0904-\u0939\u093D\u0950\u0958-\u0961\u0971-\u0980\u0985-\u098C\u098F\u0990\u0993-\u09A8\u09AA-\u09B0\u09B2\u09B6-\u09B9\u09BD\u09CE\u09DC\u09DD\u09DF-\u09E1\u09F0\u09F1\u0A05-\u0A0A\u0A0F\u0A10\u0A13-\u0A28\u0A2A-\u0A30\u0A32\u0A33\u0A35\u0A36\u0A38\u0A39\u0A59-\u0A5C\u0A5E\u0A72-\u0A74\u0A85-\u0A8D\u0A8F-\u0A91\u0A93-\u0AA8\u0AAA-\u0AB0\u0AB2\u0AB3\u0AB5-\u0AB9\u0ABD\u0AD0\u0AE0\u0AE1\u0B05-\u0B0C\u0B0F\u0B10\u0B13-\u0B28\u0B2A-\u0B30\u0B32\u0B33\u0B35-\u0B39\u0B3D\u0B5C\u0B5D\u0B5F-\u0B61\u0B71\u0B83\u0B85-\u0B8A\u0B8E-\u0B90\u0B92-\u0B95\u0B99\u0B9A\u0B9C\u0B9E\u0B9F\u0BA3\u0BA4\u0BA8-\u0BAA\u0BAE-\u0BB9\u0BD0\u0C05-\u0C0C\u0C0E-\u0C10\u0C12-\u0C28\u0C2A-\u0C39\u0C3D\u0C58\u0C59\u0C60\u0C61\u0C85-\u0C8C\u0C8E-\u0C90\u0C92-\u0CA8\u0CAA-\u0CB3\u0CB5-\u0CB9\u0CBD\u0CDE\u0CE0\u0CE1\u0CF1\u0CF2\u0D05-\u0D0C\u0D0E-\u0D10\u0D12-\u0D3A\u0D3D\u0D4E\u0D60\u0D61\u0D7A-\u0D7F\u0D85-\u0D96\u0D9A-\u0DB1\u0DB3-\u0DBB\u0DBD\u0DC0-\u0DC6\u0E01-\u0E30\u0E32\u0E33\u0E40-\u0E46\u0E81\u0E82\u0E84\u0E87\u0E88\u0E8A\u0E8D\u0E94-\u0E97\u0E99-\u0E9F\u0EA1-\u0EA3\u0EA5\u0EA7\u0EAA\u0EAB\u0EAD-\u0EB0\u0EB2\u0EB3\u0EBD\u0EC0-\u0EC4\u0EC6\u0EDC-\u0EDF\u0F00\u0F40-\u0F47\u0F49-\u0F6C\u0F88-\u0F8C\u1000-\u102A\u103F\u1050-\u1055\u105A-\u105D\u1061\u1065\u1066\u106E-\u1070\u1075-\u1081\u108E\u10A0-\u10C5\u10C7\u10CD\u10D0-\u10FA\u10FC-\u1248\u124A-\u124D\u1250-\u1256\u1258\u125A-\u125D\u1260-\u1288\u128A-\u128D\u1290-\u12B0\u12B2-\u12B5\u12B8-\u12BE\u12C0\u12C2-\u12C5\u12C8-\u12D6\u12D8-\u1310\u1312-\u1315\u1318-\u135A\u1380-\u138F\u13A0-\u13F4\u1401-\u166C\u166F-\u167F\u1681-\u169A\u16A0-\u16EA\u16EE-\u16F8\u1700-\u170C\u170E-\u1711\u1720-\u1731\u1740-\u1751\u1760-\u176C\u176E-\u1770\u1780-\u17B3\u17D7\u17DC\u1820-\u1877\u1880-\u18A8\u18AA\u18B0-\u18F5\u1900-\u191E\u1950-\u196D\u1970-\u1974\u1980-\u19AB\u19C1-\u19C7\u1A00-\u1A16\u1A20-\u1A54\u1AA7\u1B05-\u1B33\u1B45-\u1B4B\u1B83-\u1BA0\u1BAE\u1BAF\u1BBA-\u1BE5\u1C00-\u1C23\u1C4D-\u1C4F\u1C5A-\u1C7D\u1CE9-\u1CEC\u1CEE-\u1CF1\u1CF5\u1CF6\u1D00-\u1DBF\u1E00-\u1F15\u1F18-\u1F1D\u1F20-\u1F45\u1F48-\u1F4D\u1F50-\u1F57\u1F59\u1F5B\u1F5D\u1F5F-\u1F7D\u1F80-\u1FB4\u1FB6-\u1FBC\u1FBE\u1FC2-\u1FC4\u1FC6-\u1FCC\u1FD0-\u1FD3\u1FD6-\u1FDB\u1FE0-\u1FEC\u1FF2-\u1FF4\u1FF6-\u1FFC\u2071\u207F\u2090-\u209C\u2102\u2107\u210A-\u2113\u2115\u2119-\u211D\u2124\u2126\u2128\u212A-\u212D\u212F-\u2139\u213C-\u213F\u2145-\u2149\u214E\u2160-\u2188\u2C00-\u2C2E\u2C30-\u2C5E\u2C60-\u2CE4\u2CEB-\u2CEE\u2CF2\u2CF3\u2D00-\u2D25\u2D27\u2D2D\u2D30-\u2D67\u2D6F\u2D80-\u2D96\u2DA0-\u2DA6\u2DA8-\u2DAE\u2DB0-\u2DB6\u2DB8-\u2DBE\u2DC0-\u2DC6\u2DC8-\u2DCE\u2DD0-\u2DD6\u2DD8-\u2DDE\u2E2F\u3005-\u3007\u3021-\u3029\u3031-\u3035\u3038-\u303C\u3041-\u3096\u309D-\u309F\u30A1-\u30FA\u30FC-\u30FF\u3105-\u312D\u3131-\u318E\u31A0-\u31BA\u31F0-\u31FF\u3400-\u4DB5\u4E00-\u9FCC\uA000-\uA48C\uA4D0-\uA4FD\uA500-\uA60C\uA610-\uA61F\uA62A\uA62B\uA640-\uA66E\uA67F-\uA69D\uA6A0-\uA6EF\uA717-\uA71F\uA722-\uA788\uA78B-\uA78E\uA790-\uA7AD\uA7B0\uA7B1\uA7F7-\uA801\uA803-\uA805\uA807-\uA80A\uA80C-\uA822\uA840-\uA873\uA882-\uA8B3\uA8F2-\uA8F7\uA8FB\uA90A-\uA925\uA930-\uA946\uA960-\uA97C\uA984-\uA9B2\uA9CF\uA9E0-\uA9E4\uA9E6-\uA9EF\uA9FA-\uA9FE\uAA00-\uAA28\uAA40-\uAA42\uAA44-\uAA4B\uAA60-\uAA76\uAA7A\uAA7E-\uAAAF\uAAB1\uAAB5\uAAB6\uAAB9-\uAABD\uAAC0\uAAC2\uAADB-\uAADD\uAAE0-\uAAEA\uAAF2-\uAAF4\uAB01-\uAB06\uAB09-\uAB0E\uAB11-\uAB16\uAB20-\uAB26\uAB28-\uAB2E\uAB30-\uAB5A\uAB5C-\uAB5F\uAB64\uAB65\uABC0-\uABE2\uAC00-\uD7A3\uD7B0-\uD7C6\uD7CB-\uD7FB\uF900-\uFA6D\uFA70-\uFAD9\uFB00-\uFB06\uFB13-\uFB17\uFB1D\uFB1F-\uFB28\uFB2A-\uFB36\uFB38-\uFB3C\uFB3E\uFB40\uFB41\uFB43\uFB44\uFB46-\uFBB1\uFBD3-\uFD3D\uFD50-\uFD8F\uFD92-\uFDC7\uFDF0-\uFDFB\uFE70-\uFE74\uFE76-\uFEFC\uFF21-\uFF3A\uFF41-\uFF5A\uFF66-\uFFBE\uFFC2-\uFFC7\uFFCA-\uFFCF\uFFD2-\uFFD7\uFFDA-\uFFDC]"));

regex NonAsciiIdentifierPart (toU8string(u"[\xAA\xB5\xBA\xC0-\xD6\xD8-\xF6\xF8-\u02C1\u02C6-\u02D1\u02E0-\u02E4\u02EC\u02EE\u0300-\u0374\u0376\u0377\u037A-\u037D\u037F\u0386\u0388-\u038A\u038C\u038E-\u03A1\u03A3-\u03F5\u03F7-\u0481\u0483-\u0487\u048A-\u052F\u0531-\u0556\u0559\u0561-\u0587\u0591-\u05BD\u05BF\u05C1\u05C2\u05C4\u05C5\u05C7\u05D0-\u05EA\u05F0-\u05F2\u0610-\u061A\u0620-\u0669\u066E-\u06D3\u06D5-\u06DC\u06DF-\u06E8\u06EA-\u06FC\u06FF\u0710-\u074A\u074D-\u07B1\u07C0-\u07F5\u07FA\u0800-\u082D\u0840-\u085B\u08A0-\u08B2\u08E4-\u0963\u0966-\u096F\u0971-\u0983\u0985-\u098C\u098F\u0990\u0993-\u09A8\u09AA-\u09B0\u09B2\u09B6-\u09B9\u09BC-\u09C4\u09C7\u09C8\u09CB-\u09CE\u09D7\u09DC\u09DD\u09DF-\u09E3\u09E6-\u09F1\u0A01-\u0A03\u0A05-\u0A0A\u0A0F\u0A10\u0A13-\u0A28\u0A2A-\u0A30\u0A32\u0A33\u0A35\u0A36\u0A38\u0A39\u0A3C\u0A3E-\u0A42\u0A47\u0A48\u0A4B-\u0A4D\u0A51\u0A59-\u0A5C\u0A5E\u0A66-\u0A75\u0A81-\u0A83\u0A85-\u0A8D\u0A8F-\u0A91\u0A93-\u0AA8\u0AAA-\u0AB0\u0AB2\u0AB3\u0AB5-\u0AB9\u0ABC-\u0AC5\u0AC7-\u0AC9\u0ACB-\u0ACD\u0AD0\u0AE0-\u0AE3\u0AE6-\u0AEF\u0B01-\u0B03\u0B05-\u0B0C\u0B0F\u0B10\u0B13-\u0B28\u0B2A-\u0B30\u0B32\u0B33\u0B35-\u0B39\u0B3C-\u0B44\u0B47\u0B48\u0B4B-\u0B4D\u0B56\u0B57\u0B5C\u0B5D\u0B5F-\u0B63\u0B66-\u0B6F\u0B71\u0B82\u0B83\u0B85-\u0B8A\u0B8E-\u0B90\u0B92-\u0B95\u0B99\u0B9A\u0B9C\u0B9E\u0B9F\u0BA3\u0BA4\u0BA8-\u0BAA\u0BAE-\u0BB9\u0BBE-\u0BC2\u0BC6-\u0BC8\u0BCA-\u0BCD\u0BD0\u0BD7\u0BE6-\u0BEF\u0C00-\u0C03\u0C05-\u0C0C\u0C0E-\u0C10\u0C12-\u0C28\u0C2A-\u0C39\u0C3D-\u0C44\u0C46-\u0C48\u0C4A-\u0C4D\u0C55\u0C56\u0C58\u0C59\u0C60-\u0C63\u0C66-\u0C6F\u0C81-\u0C83\u0C85-\u0C8C\u0C8E-\u0C90\u0C92-\u0CA8\u0CAA-\u0CB3\u0CB5-\u0CB9\u0CBC-\u0CC4\u0CC6-\u0CC8\u0CCA-\u0CCD\u0CD5\u0CD6\u0CDE\u0CE0-\u0CE3\u0CE6-\u0CEF\u0CF1\u0CF2\u0D01-\u0D03\u0D05-\u0D0C\u0D0E-\u0D10\u0D12-\u0D3A\u0D3D-\u0D44\u0D46-\u0D48\u0D4A-\u0D4E\u0D57\u0D60-\u0D63\u0D66-\u0D6F\u0D7A-\u0D7F\u0D82\u0D83\u0D85-\u0D96\u0D9A-\u0DB1\u0DB3-\u0DBB\u0DBD\u0DC0-\u0DC6\u0DCA\u0DCF-\u0DD4\u0DD6\u0DD8-\u0DDF\u0DE6-\u0DEF\u0DF2\u0DF3\u0E01-\u0E3A\u0E40-\u0E4E\u0E50-\u0E59\u0E81\u0E82\u0E84\u0E87\u0E88\u0E8A\u0E8D\u0E94-\u0E97\u0E99-\u0E9F\u0EA1-\u0EA3\u0EA5\u0EA7\u0EAA\u0EAB\u0EAD-\u0EB9\u0EBB-\u0EBD\u0EC0-\u0EC4\u0EC6\u0EC8-\u0ECD\u0ED0-\u0ED9\u0EDC-\u0EDF\u0F00\u0F18\u0F19\u0F20-\u0F29\u0F35\u0F37\u0F39\u0F3E-\u0F47\u0F49-\u0F6C\u0F71-\u0F84\u0F86-\u0F97\u0F99-\u0FBC\u0FC6\u1000-\u1049\u1050-\u109D\u10A0-\u10C5\u10C7\u10CD\u10D0-\u10FA\u10FC-\u1248\u124A-\u124D\u1250-\u1256\u1258\u125A-\u125D\u1260-\u1288\u128A-\u128D\u1290-\u12B0\u12B2-\u12B5\u12B8-\u12BE\u12C0\u12C2-\u12C5\u12C8-\u12D6\u12D8-\u1310\u1312-\u1315\u1318-\u135A\u135D-\u135F\u1380-\u138F\u13A0-\u13F4\u1401-\u166C\u166F-\u167F\u1681-\u169A\u16A0-\u16EA\u16EE-\u16F8\u1700-\u170C\u170E-\u1714\u1720-\u1734\u1740-\u1753\u1760-\u176C\u176E-\u1770\u1772\u1773\u1780-\u17D3\u17D7\u17DC\u17DD\u17E0-\u17E9\u180B-\u180D\u1810-\u1819\u1820-\u1877\u1880-\u18AA\u18B0-\u18F5\u1900-\u191E\u1920-\u192B\u1930-\u193B\u1946-\u196D\u1970-\u1974\u1980-\u19AB\u19B0-\u19C9\u19D0-\u19D9\u1A00-\u1A1B\u1A20-\u1A5E\u1A60-\u1A7C\u1A7F-\u1A89\u1A90-\u1A99\u1AA7\u1AB0-\u1ABD\u1B00-\u1B4B\u1B50-\u1B59\u1B6B-\u1B73\u1B80-\u1BF3\u1C00-\u1C37\u1C40-\u1C49\u1C4D-\u1C7D\u1CD0-\u1CD2\u1CD4-\u1CF6\u1CF8\u1CF9\u1D00-\u1DF5\u1DFC-\u1F15\u1F18-\u1F1D\u1F20-\u1F45\u1F48-\u1F4D\u1F50-\u1F57\u1F59\u1F5B\u1F5D\u1F5F-\u1F7D\u1F80-\u1FB4\u1FB6-\u1FBC\u1FBE\u1FC2-\u1FC4\u1FC6-\u1FCC\u1FD0-\u1FD3\u1FD6-\u1FDB\u1FE0-\u1FEC\u1FF2-\u1FF4\u1FF6-\u1FFC\u200C\u200D\u203F\u2040\u2054\u2071\u207F\u2090-\u209C\u20D0-\u20DC\u20E1\u20E5-\u20F0\u2102\u2107\u210A-\u2113\u2115\u2119-\u211D\u2124\u2126\u2128\u212A-\u212D\u212F-\u2139\u213C-\u213F\u2145-\u2149\u214E\u2160-\u2188\u2C00-\u2C2E\u2C30-\u2C5E\u2C60-\u2CE4\u2CEB-\u2CF3\u2D00-\u2D25\u2D27\u2D2D\u2D30-\u2D67\u2D6F\u2D7F-\u2D96\u2DA0-\u2DA6\u2DA8-\u2DAE\u2DB0-\u2DB6\u2DB8-\u2DBE\u2DC0-\u2DC6\u2DC8-\u2DCE\u2DD0-\u2DD6\u2DD8-\u2DDE\u2DE0-\u2DFF\u2E2F\u3005-\u3007\u3021-\u302F\u3031-\u3035\u3038-\u303C\u3041-\u3096\u3099\u309A\u309D-\u309F\u30A1-\u30FA\u30FC-\u30FF\u3105-\u312D\u3131-\u318E\u31A0-\u31BA\u31F0-\u31FF\u3400-\u4DB5\u4E00-\u9FCC\uA000-\uA48C\uA4D0-\uA4FD\uA500-\uA60C\uA610-\uA62B\uA640-\uA66F\uA674-\uA67D\uA67F-\uA69D\uA69F-\uA6F1\uA717-\uA71F\uA722-\uA788\uA78B-\uA78E\uA790-\uA7AD\uA7B0\uA7B1\uA7F7-\uA827\uA840-\uA873\uA880-\uA8C4\uA8D0-\uA8D9\uA8E0-\uA8F7\uA8FB\uA900-\uA92D\uA930-\uA953\uA960-\uA97C\uA980-\uA9C0\uA9CF-\uA9D9\uA9E0-\uA9FE\uAA00-\uAA36\uAA40-\uAA4D\uAA50-\uAA59\uAA60-\uAA76\uAA7A-\uAAC2\uAADB-\uAADD\uAAE0-\uAAEF\uAAF2-\uAAF6\uAB01-\uAB06\uAB09-\uAB0E\uAB11-\uAB16\uAB20-\uAB26\uAB28-\uAB2E\uAB30-\uAB5A\uAB5C-\uAB5F\uAB64\uAB65\uABC0-\uABEA\uABEC\uABED\uABF0-\uABF9\uAC00-\uD7A3\uD7B0-\uD7C6\uD7CB-\uD7FB\uF900-\uFA6D\uFA70-\uFAD9\uFB00-\uFB06\uFB13-\uFB17\uFB1D-\uFB28\uFB2A-\uFB36\uFB38-\uFB3C\uFB3E\uFB40\uFB41\uFB43\uFB44\uFB46-\uFBB1\uFBD3-\uFD3D\uFD50-\uFD8F\uFD92-\uFDC7\uFDF0-\uFDFB\uFE00-\uFE0F\uFE20-\uFE2D\uFE33\uFE34\uFE4D-\uFE4F\uFE70-\uFE74\uFE76-\uFEFC\uFF10-\uFF19\uFF21-\uFF3A\uFF3F\uFF41-\uFF5A\uFF66-\uFFBE\uFFC2-\uFFC7\uFFCA-\uFFCF\uFFD2-\uFFD7\uFFDA-\uFFDC]"));

// See also tools/generate-unicode-regex.py.
map<string, regex> Regex = {
    {"NonAsciiIdentifierStart", NonAsciiIdentifierStart},
    {"NonAsciiIdentifierPart", NonAsciiIdentifierPart}
};


void initglobals() { DEBUGIN(" initglobals()");
    PlaceHolders["ArrowParameterPlaceHolder"].type=u"ArrowParameterPlaceholder";
    NULLTOKEN.isNull = true;
    NULLNODE.isNull = true;
 DEBUGOUT();
}

 // Ensure the condition is true, otherwise throw an error.
 // This is only to have a better contract semantic, i.e. another safety net
 // to catch a logic error. The condition shall be fulfilled in normal case.
 // Do NOT use this to enforce a certain condition on any user input.

void assert(bool condition, string message) { DEBUGIN(" assert(bool condition, string message)");
    string providedMessage = "ASSERT: ";
    providedMessage.append(message);
    if (!condition) {
        throw std::runtime_error(providedMessage);
    }
 DEBUGOUT();
}

bool isDecimalDigit(const char16_t ch) { DEBUGIN("   isDecimalDigit(const char16_t ch)");
  DEBUGOUT(); return (ch >= 0x30 && ch <= 0x39); //0..9
}

bool isHexDigit(const char16_t ch) { DEBUGIN("   isHexDigit(const char16_t ch)");
  DEBUGOUT(); return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) != std::string::npos);    
}

bool isOctalDigit(const char16_t ch) { DEBUGIN("   isOctalDigit(const char16_t ch)");
  DEBUGOUT(); return (u16string({u"01234567"}).find_first_of(ch) != std::string::npos);    
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
  DEBUGOUT(); return out;
}

//7.2 White Space
bool isWhiteSpace(const char16_t ch) { DEBUGIN("   isWhiteSpace(const char16_t ch)");
  DEBUGOUT(); return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || (ch >= 0x1680 && has<int>(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators

bool isLineTerminator(const char16_t ch) { DEBUGIN("   isLineTerminator(const char16_t ch)");
  DEBUGOUT(); return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}

// 7.6 Identifier Names and Identifiers

bool isIdentifierStart(const char16_t ch) { DEBUGIN("   isIdentifierStart(const char16_t ch)");
    smatch m;
  DEBUGOUT(); return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierStart"]));
} 

bool isIdentifierPart(const char16_t ch) { DEBUGIN("   isIdentifierPart(const char16_t ch)");
    smatch m;
  DEBUGOUT(); return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch >= 0x30 && ch <= 0x39) ||         // 0..9
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierPart"]));
}

// 7.6.1.2 Future Reserved Words

bool isFutureReservedWord(const u16string id) { DEBUGIN("   isFutureReservedWord(const u16string id)");
  DEBUGOUT(); return has<u16string>(id, { //
            u"class",
            u"enum",
            u"export",
            u"extends",
            u"import",
            u"super"
            });
}

bool isStrictModeReservedWord(const u16string id) { DEBUGIN("   isStrictModeReservedWord(const u16string id)");
  DEBUGOUT(); return has<u16string>(id, { 
            u"implements",
            u"interface",
            u"package",
            u"private",
            u"protected",
            u"public",
            u"static",
            u"yield",
            u"let"
            });
}

bool isRestrictedWord(const u16string id) { DEBUGIN("   isRestrictedWord(const u16string id)");
  DEBUGOUT(); return (id == u"eval" || id == u"arguments");
}

// 7.6.1.1 Keywords
bool isKeyword(const u16string id) { DEBUGIN("   isKeyword(const u16string id)");
    if (strict && isStrictModeReservedWord(id)) { 
      DEBUGOUT(); return true;
    }

    // 'const' is specialized as Keyword in V8.
    // 'yield' and 'let' are for compatiblity with SpiderMonkey and ES.next.
    // Some others are from future reserved words.

    switch (id.length()) {
        case 2:
          DEBUGOUT(); return (id == u"if") || (id == u"in") || (id == u"do");
        case 3:
          DEBUGOUT(); return has<u16string>(id, 
              { u"var", u"for", u"new", u"try", u"let"});
        case 4:
          DEBUGOUT(); return has<u16string>(id, 
              {u"this", u"else", u"case", u"void", u"with", u"enum"});
        case 5:
          DEBUGOUT(); return has<u16string>(id, 
              {u"while", u"break", u"catch",
                        u"throw", u"const", u"yield", u"class", u"super"});
        case 6:
          DEBUGOUT(); return has<u16string>(id, 
              {u"return", u"typeof", u"delete",
                        u"switch", u"export", u"import"});
        case 7:
          DEBUGOUT(); return (id == u"default") || 
                          (id == u"finally") || (id == u"extends");
        case 8:
          DEBUGOUT(); return (id == u"function") || 
                          (id == u"continue") || (id == u"debugger");
        case 10:
          DEBUGOUT(); return (id == u"instanceof");
        default:
          DEBUGOUT(); return false;
    }
}

// 7.4 Comments

//#CLEAR+
//# only called if extra.commentTracking
void addComment(u16string type, u16string value, 
                int start, int end, Loc loc) { DEBUGIN(" addComment(u16string type, u16string value, int start, int end, Loc loc)");
    Comment comment;

    //assert(typeof start === 'number', 'Comment must have valid position');

    // Because the way the actual token is scanned, often the comments
    // (if any) are skipped twice during the lexical analysis.
    // Thus, we need to skip adding a comment if the comment array already
    // handled it.
    if (state.lastCommentStart >= start) {
      DEBUGOUT(); return;
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
    DEBUGOUT();
}


//#CLEAR+
void skipSingleLineComment(const int offset) { DEBUGIN(" skipSingleLineComment(const int offset)");
    int start;
    Loc loc; 
    char16_t ch;
    u16string comment;

    start = idx - offset;
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart - offset;

    while (idx < length) {
        ch = source(idx);
        ++idx;
        if (isLineTerminator(ch)) {
            if (extra.commentTracking) {
                comment = slice(sourceraw, start + offset, idx-1);
                loc.end.line = lineNumber;
                loc.end.column = idx - lineStart - 1;
                addComment(u"Line", comment, start, idx - 1, loc);
            }
            if (ch == 13 && source(idx) == 10) {
                ++idx;
            }
            ++lineNumber;
            lineStart = idx;
          DEBUGOUT(); return;
        }
    }

    if (extra.commentTracking) {
        comment = slice(sourceraw, start + offset, idx); 
        loc.end.line = lineNumber;
        loc.end.column = idx - lineStart;
        addComment(u"Line", comment, start, idx, loc);
    }
    DEBUGOUT();
}

void skipSingleLineComment() { //? are we sure that in javascript the calls to this with no arg will default to 0?
    skipSingleLineComment(0);
}

//#CLEAR+
void skipMultiLineComment() { DEBUGIN(" skipMultiLineComment()");
    int start;
    Loc loc;
    char16_t ch;
    u16string comment;

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
                           Messages["UnexpectedToken"], {u"ILLEGAL"});
            }
        } else if (ch == 0x2A) {
            // Block comment ends with ''.
            if (source(idx + 1) == 0x2F) {
                ++idx;
                ++idx;
                if (extra.commentTracking) {
                    comment = slice(sourceraw, start + 2, idx - 2);
                    loc.end.line = lineNumber;
                    loc.end.column = idx - lineStart;
                    addComment(u"Block", comment, start, idx, loc);
                }
              DEBUGOUT(); return;
            }
            ++idx;
        } else {
            ++idx;
        }
    }

    throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
 DEBUGOUT();
}

//#CLEAR+
void skipComment() { DEBUGIN(" skipComment()");
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
 DEBUGOUT();
}

//#CLEAR+
char16_t scanHexEscape(const char16_t prefix) { DEBUGIN("scanHexEscape(const char16_t prefix) {");
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
          DEBUGOUT("scanHexEscape"); return NULL_CHAR16; 
        }
    }
  DEBUGOUT("scanHexEscape"); return code;
}

//#CLEAR+
u16string scanUnicodeCodePointEscape() { DEBUGIN("scanUnicodeCodePointEscape");
    char16_t ch;
    int code;
    char16_t cu[2];

    ch = source(idx);
    code = 0;

    // At least, one hex digit is required.
    if (ch == u'}') {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }

    // UTF-16 Encoding
    if (code <= 0xFFFF) {
      DEBUGOUT(); return u16string({(char16_t) code});
    }

    cu[0] = ((code - 0x10000) >> 10) + 0xD800; 
    cu[1] = ((code - 0x10000) & 1023) + 0xDC00;
  DEBUGOUT("scanUnicodeCodePointEscape"); return u16string(cu);
}

//#CLEAR+
u16string getEscapedIdentifier() { DEBUGIN("getEscapedIdentifier");
    char16_t ch;
    u16string id;

    ch = source(idx++);
    id = u16string({ch});

    // '\u' (U+005C, U+0075) denotes an escaped character.
    if (ch == 0x5C) {
        if (source(idx) != 0x75) {
            throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierStart(ch)) { 
            throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
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
                           Messages["UnexpectedToken"], {u"ILLEGAL"});
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierPart(ch)) {
                throwError(NULLTOKEN, 
                           Messages["UnexpectedToken"], {u"ILLEGAL"});
            }
            append(id, ch);
        }
    }

  DEBUGOUT("getEscapedIdentifier"); return id;
}

//#CLEAR+
u16string getIdentifier() { DEBUGIN("getIdentifier()");
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

  return DEBUGRET("getIdentifier", slice(sourceraw, start, idx)); 
}

//#CLEAR+
TokenStruct scanIdentifier() { DEBUGIN(" scanIdentifier()");
    TokenStruct t;
    int start, type;
    u16string id;

    start = idx;

    // Backslash (U+005C) starts an escaped character.
    id = (source(idx) == 0x5C) ? 
        getEscapedIdentifier() : getIdentifier();

    // There is no keyword or literal with only one character.
    // Thus, it must be an identifier.
    if (id.length() == 1) {
        type = Token["Identifier"];
    } else if (isKeyword(id)) {
        type = Token["Keyword"];
    } else if (id == u"null") {
        type = Token["NullLiteral"];
    } else if (id == u"true" || id == u"false") {
        type = Token["BooleanLiteral"];
    } else {
        type = Token["Identifier"];
    }

    t.type = type;
    t.strvalue = id;
    t.literaltype = LiteralType["String"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
  DEBUGOUT("scanIdentifier"); return t;
}

// 7.7 Punctuators
//#CLEAR+
TokenStruct scanPunctuator() { DEBUGIN(" scanPunctuator()");
    TokenStruct t;
    int start = idx;

    char16_t ch1 = source(idx);
    u16string ch2, ch3, ch4;
    char16_t code[2];

    code[0] = source(idx);
    
    t.type = Token["Punctuator"];
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
        t.strvalue = u16string({ code[0] });
        
        t.end = idx;
      DEBUGOUT(); return t;
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
                t.strvalue = u16string(code);
                t.end = idx;
              DEBUGOUT(); return t;
            case 0x21: // !
            case 0x3D: // =
                idx += 2;

                // !== and ===
                if (source(idx) == 0x3D) {
                    ++idx;
                }
                t.strvalue = slice(sourceraw, start, idx);
                t.end = idx;
              DEBUGOUT(); return t;
            }
        }
    }


    // 4-character punctuator: >>>=

    ch4 = slice(sourceraw, idx, idx+4); 

    if (ch4 == u">>>=") {
        idx += 4;
        t.strvalue = ch4;
        t.end = idx;
      DEBUGOUT(); return t;
    }

    // 3-character punctuators: === !== >>> <<= >>=

    ch3 = ch4.substr(0, 3); 

    if (ch3 == u">>>" || ch3 == u"<<=" || ch3 == u">>=") {
        idx += 3;
        t.strvalue = ch3;
        t.end = idx;
      DEBUGOUT(); return t;
    }

    // Other 2-character punctuators: ++ -- << >> && ||
    ch2 = ch3.substr(0, 2); 

    if ((ch1 == ch2[1] 
         && (u16string({u"+-<>&|"}).find_first_of(ch1) != std::string::npos))
        || ch2 == u"=>") {
        idx += 2;
        t.strvalue = ch2;
        t.end = idx;
      DEBUGOUT(); return t;
    }

    // 1-character punctuators: < > = ! + - * % & | ^ /

    if (u16string({u"<>=!+-*%&|^/"}) //? is it necessary/correct to have this as ({u""}) and not just (u"")
        .find_first_of(ch1) != std::string::npos) {
        ++idx;
        t.strvalue = u16string({ch1});
        t.end = idx;
      DEBUGOUT(); return t;
    }

    throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
  DEBUGOUT(); return t;
  //# return avoids compile warnings bcos clang doesn't look into throwError.
}
    // 7.8.3 Numeric Literals
//#CLEAR+
TokenStruct scanHexLiteral(const int start) { DEBUGIN(" scanHexLiteral(const int start)");
    u16string number;
    TokenStruct t;
    u16string val;

    while (idx < length) {
        if (!isHexDigit(source(idx))) {
            break;
        }
        append(number, source(idx++));
    }

    if (number.length() == 0) {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }
   
    t.type = Token["NumericLiteral"];
    val = u"0x";
    val.append(number);
    t.intvalue = parseInt(val, 16);
    t.literaltype = LiteralType["Int"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
  DEBUGOUT("scanHexLiteral"); return t;
}

//#CLEAR+
TokenStruct scanOctalLiteral(const int start) { DEBUGIN(" scanOctalLiteral(const int start)");
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }

    t.type = Token["NumericLiteral"];
    t.intvalue = parseInt(number, 8);
    t.literaltype = LiteralType["Int"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
  DEBUGOUT("scanOctalLiteral"); return t;
}

//#CLEAR+
TokenStruct scanNumericLiteral() { DEBUGIN(" scanNumericLiteral()");
    int start;
    char16_t ch;
    TokenStruct t;
    u16string number;


    ch = source(idx);
    assert(isDecimalDigit(ch) || (ch == u'.'),
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
                           Messages["UnexpectedToken"], {u"ILLEGAL"}); 
            }
        }

        while (isDecimalDigit(source(idx))) {
            append(number, source(idx++));
        }
        ch = source(idx);
    }
    
    if (ch == u'.') {
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
            throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
        }
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }
    
    t.type = Token["NumericLiteral"];
    t.strvalue = number;
        //t.dblvalue = //# want to allow browsers to use full allowance of whatever their local max int size is.
        //               //#which is same as it would work in esprima.
        //(hasSciNote)?
        //sciNoteToDouble(toU8string(number)) : stod(toU8string(number));
    t.literaltype = LiteralType["Double"];
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    
  DEBUGOUT("scanNumericLiteral"); return t; 
}

// 7.8.4 String Literals
//#CLEAR+
TokenStruct scanStringLiteral() { DEBUGIN(" scanStringLiteral()");
    TokenStruct t;
    bool octal = false;
    u16string str;
    int start, restore, code;
    char16_t quote, ch, unescaped;
    int startLineNumber = lineNumber;
    int startLineStart = lineStart;

    quote = source(idx);
    assert((quote == u'\'' || quote == u'"'),
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
                        str.append(scanUnicodeCodePointEscape());
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], {u"ILLEGAL"});
    }
    
    t.type = Token["StringLiteral"];
    t.strvalue = str;
    t.literaltype = LiteralType["String"];            
    t.octal = octal;
    t.startLineNumber = startLineNumber;
    t.startLineStart = startLineStart;
    t.lineNumber = lineNumber;
    t.lineStart = lineStart,
    t.start = start;
    t.end = idx;
  DEBUGOUT("scanStringLiteral"); return t;
}

//#CLEAR+
RegexHalf scanRegExpBody() { DEBUGIN("scanRegExpBody()");
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexHalf rh;

    ch = source(idx);
    assert(ch == u'/', "Regular expression literal must start with a slash");
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
                throwError(NULLTOKEN, Messages["UnterminatedRegExp"], {});
            }
            append(str, ch);
        } else if (isLineTerminator(ch)) {
            throwError(NULLTOKEN, Messages["UnterminatedRegExp"], {});
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
        throwError(NULLTOKEN, Messages["UnterminatedRegExp"], {});
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length() - 2);
    rh.value = body;
    rh.literal = str;
  DEBUGOUT("scanRegExpBody"); return rh;
}

//#CLEAR+
RegexHalf scanRegExpFlags() { DEBUGIN("scanRegExpFlags()");
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
                                   Messages["UnexpectedToken"], 
                                   {u"ILLEGAL"});
            } else {
                append(str, u'\\');
                throwErrorTolerant(NULLTOKEN, 
                                   Messages["UnexpectedToken"],
                                   {u"ILLEGAL"});
            }
        } else {
            append(flags, ch);
            append(str, ch);
        }
    }
    
    rh.value = flags;
    rh.literal = str;
  DEBUGOUT("scanRegExpFlags"); return rh;
}

TokenStruct scanRegExp() { DEBUGIN(" scanRegExp()");
    int start;
    RegexHalf body; 
    RegexHalf flags; 
    TokenStruct t;
    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = NULLTOKEN;
    skipComment();
    start = idx;

    body = scanRegExpBody();
    flags = scanRegExpFlags();
    //value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t.type = Token["RegularExpression"];
        t.strvalue = u"regexDummy"; //?
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = start;
        t.end = idx;
      DEBUGOUT(); return t; //not polymorphic right now. not going to work... :!
    }

    t.literal = body.literal; 
    t.literal.append(flags.literal);
    t.literaltype = LiteralType["Regexp"];
    t.strvalue = body.value;
    t.flags = flags.value;
    t.start = start;
    t.end = idx;
  DEBUGOUT("scanRegExp"); return t;
}

//#CLEAR
TokenStruct collectRegex() { DEBUGIN(" collectRegex()");
    int pos;
    Loc loc;
    TokenStruct regex;
    u16string tokval;

    skipComment();

    pos = idx;
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart;

    regex = scanRegExp();
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (!extra.tokenize) {
        TokenRecord token, tr;
        // Pop the previous token, which is likely '/' or '/='
        if (extra.tokenRecords.size() > 0) {
            token = extra.tokenRecords[extra.tokenRecords.size() - 1];
            if (token.range[0] == pos && token.typestring == u"Punctuator") {
                tokval = token.valuestring; 
                if (tokval == u"/" || tokval == u"/=") {
                    extra.tokenRecords.pop_back();
                }
            }
        }

        tr.typestring = u"RegularExpression";
        tr.valuestring = regex.literal;
        tr.range[0] = pos;
        tr.range[1] = idx;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

  DEBUGOUT("collectRegex"); return regex;
}

//#CLEAR+
bool isIdentifierName(TokenStruct token) { DEBUGIN("   isIdentifierName(TokenStruct token)");
  DEBUGOUT(); return has<int>(token.type, { Token["Identifier"], Token["Keyword"],
                Token["BooleanLiteral"], Token["NullLiteral"]});
}

//#CLEAR+
TokenStruct advanceSlash() { DEBUGIN(" advanceSlash()");
    //# only gets called if extra.tokenize == true

    TokenRecord prevToken, checkToken;
    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    if (extra.tokenRecords.size() == 0) {
        // Nothing before that: it cannot be a division.
      return DEBUGRET("advSlash", collectRegex()); 
    }    
    prevToken = extra.tokenRecords[extra.tokenRecords.size() - 1];
    
    if (prevToken.typestring == u"Punctuator") { 
        if (prevToken.valuestring == u"]") { 
          return DEBUGRET("advSlash", scanPunctuator());
        }
        if (prevToken.valuestring == u")" && extra.openParenToken > 0
            && extra.tokenRecords.size() > (extra.openParenToken - 1)) { 
            checkToken = extra.tokenRecords[extra.openParenToken - 1];
            if (//checkToken && 
                //# instead of checking for existence, we add
                //# the openParenToken value check to the condition above.
                //# remember exta.tokens() is already size > 0 bcos 
                //# check at top of func.
                checkToken.typestring == u"Keyword" && 
                has<u16string>(checkToken.valuestring, 
                    {u"if", u"while", u"for", u"with"})) {
              return DEBUGRET("advSlash", collectRegex()); 
            }
          return DEBUGRET("advSlash", scanPunctuator());
        }
        if (prevToken.valuestring == u"}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokenRecords.size() > (extra.openCurlyToken -3) &&
                extra.tokenRecords[extra.openCurlyToken - 3].typestring 
                == u"Keyword") { 
                // Anonymous function.

                if (extra.openCurlyToken > 3
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 4)) {
                    checkToken = extra.tokenRecords[extra.openCurlyToken -4];
                } else { 
                  return DEBUGRET("advSlash", scanPunctuator());
                }
            } else if (extra.openCurlyToken >= 4 &&
                extra.tokenRecords.size() > (extra.openCurlyToken -4) &&
                extra.tokenRecords[extra.openCurlyToken - 4].typestring
                       == u"Keyword") {
                // again previously had checked type against string in this cond.
                // Named function.
                if (extra.openCurlyToken > 4
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 5)) {
                    checkToken = extra.tokenRecords[extra.openCurlyToken -5];
                } else { 
                  return DEBUGRET("advSlash", collectRegex());
                }
            } else {
              return DEBUGRET("advSlash", scanPunctuator());
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (has<u16string>(checkToken.valuestring, FnExprTokens)) {
                // It is an expression.
              return DEBUGRET("advSlash", scanPunctuator());
            }
            // It is a declaration.
            return DEBUGRET("advSlash", collectRegex()); 
        }
        return DEBUGRET("advSlash", collectRegex());
    }
    if (prevToken.typestring == u"Keyword") { 
      return DEBUGRET("advSlash", collectRegex()); 
    }
  return DEBUGRET("advSlash", scanPunctuator());
}

//#CLEAR+
TokenStruct advance() { DEBUGIN(" advance()");
    char16_t ch;
    TokenStruct t;

    skipComment();

    if (idx >= length) {
        t.type = Token["EOF"];
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = idx;
        t.end = idx;
      DEBUGOUT(); return t;
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

//#CLEAR+
TokenStruct collectToken() { DEBUGIN(" collectToken()");
    Loc loc;
    TokenStruct token;
    TokenRecord tr;
    u16string value;

    skipComment();
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart;

    token = advance();
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (token.type != Token["EOF"]) { //this didn't check against string. is fine.
        tr.valuestring = slice(sourceraw, token.start, token.end);
        tr.typestring = TokenName[token.type];
        tr.range[0] = token.start;
        tr.range[1] = token.end;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

  DEBUGOUT("collectToken"); return token;
}

//#CLEAR
TokenStruct lex() { DEBUGIN(" lex()");
    TokenStruct token;

    token = lookahead;
    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    lookahead = (extra.tokenTracking) ? collectToken() : advance(); 

    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

  DEBUGOUT("lex"); return token;
}

//#CLEAR
void peek() { DEBUGIN(" peek()");
    int pos, line, start;
    pos = idx;
    line = lineNumber;
    start = lineStart;
    lookahead = (extra.tokenTracking) ? collectToken() : advance(); 
    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("peek");
}

//# Position and SourceLocation are defined as structs near the top.



//#CLEAR
//Loc 


    
//#CLEAR

//#this is the ONLY constructor in this code capable of 
//#modifying state, it ALWAYS and ONLY changes state if lookaheadAvail
//#is true. Important to keep in mind when making 
//#1:1 updates.
Node::Node(bool lookaheadAvail, bool storeStats) { DEBUGIN("Node::Node(bool, bool)", true);
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
    DEBUGOUT("", true);
}
Node::Node() : Node(false, true) {} 

json_object* Node::toJson() { 
  return this->jv;
}


string Node::s(u16string in) { return toU8string(in); }

void Node::lookavailInit() {
    hasJv = true;
    jv = json_newmap();

    idx = lookahead.start;
    if (lookahead.type == Token["StringLiteral"]) {
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
    trailingComments.clear();
    leadingComments.clear();
    expressions.clear();
    
    hasLoc = false;
    hasRange = false;
}

void Node::unused() {
    if (hasJv) {
        json_object_put(this->jv);
    }
}

void Node::jvput(string path, string b) {json_put(jv, path.data(), b); }
void Node::jvput(string path, int b) {json_put(jv, path.data(), b); }
void Node::jvput(string path, bool b) {json_put(jv, path.data(), b); }
void Node::jvput_dbl(string path, string b) {json_put_dbl(jv, path.data(), b); }
void Node::jvput_null(string path) { json_put_null(jv, path.data()); }

//# different name to prevent easy bug of forgetting the string.
void Node::regNoadd(vector<string> paths, Node &child) { DEBUGIN(" Node::regNoadd(vector<string> paths, Node &child)");
    if (child.isNull) { 
        child.jv = NULL;
      DEBUGOUT(); return;
    }
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
                for (int j=0; j<paths.size(); j++) {
                    regexPaths.back().push_back(paths[j]);
                }
            }
        }
    }
    child.clear();
    DEBUGOUT("Node::regNoAdd");
}

void Node::reg(string path, Node &child) { DEBUGIN(" Node::reg(string path, Node &child)");
    regNoadd({path}, child);
    json_put(jv, path.data(), child.jv);
    DEBUGOUT("node::reg");
}

void Node::nodeVec(string path, vector< Node > & nodes) { DEBUGIN(" Node::nodeVec(string path, vector< Node > & nodes)");
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
 DEBUGOUT("node::nodeVec");
}
void Node::addType(string in) { DEBUGIN(" Node::addType(string in)");
    type = Syntax[in];
    json_put(jv, "type", s(type));
 DEBUGOUT("node::addType");
}
json_object* Node::regexPaths2json() { DEBUGIN("Node::regexPaths2json()");
    json_object *tmp, *root = json_newarr();
    for (int i=0; i<regexPaths.size(); i++) {
        tmp = json_newarr();
        for (int j=0; j<regexPaths[i].size(); j++) {
            json_push(tmp, regexPaths[i].back());
            regexPaths[i].pop_back();
        }
        json_push(root, tmp);
    }
  DEBUGOUT(); return root;
}

void Node::commentsIntoJson(const bool leading) { DEBUGIN(" Node::commentsIntoJson(const bool leading)");
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
        jvput(key, vec2jsonCallback<Comment>(*commentVec,
                                          &Comment::toJson));
    } else {
        json_del(jv, key.data());
    }
    DEBUGOUT();
}

//#CLEAR
void Node::processComment() { DEBUGIN(" Node::processComment()");
    //# assumes attachComments 
    //# so that means range is already true.

    vector<Comment> trailingComments;

    vector< Node * > * bottomRight = &(extra.bottomRightStack);
    Node * lastChild = &NULLNODE,
        * last = bottomRight->at(bottomRight->size() - 1);
    bool LEADING = true;


    if (type == Syntax["Program"]) {  
        if (json_object_array_length(json_require(jv, "body")) > 0) {
          DEBUGOUT(); return;
        }
    }

    if (extra.trailingComments.size() > 0) {
        if (extra.trailingComments[0].range[0] >= this->range[1]) {
            trailingComments = extra.trailingComments;
            extra.trailingComments.clear();
        } else {
            extra.trailingComments.clear(); 
            //# originally first clause had =[] and this has .length = 0
            //# don't think there's an effective difference thoug
        }
    } else {
        if (!(last->isNull) && 
            last->trailingComments.size() > 0 && 
            last->trailingComments[0].range[0] >= this->range[1]) {
            trailingComments = last->trailingComments;
            last->trailingComments.clear();
            last->commentsIntoJson(! LEADING);
            //delete last.trailingComments; 
            //? maybe have a boolean to say no trailing comments? length will prob. be workable.
        }
    }

    // Eating the stack.
    if (!(last->isNull)) {
        while ((!(last->isNull)) && last->range[0] >= this->range[0]) {
            lastChild = last;
            last = bottomRight->back();
            bottomRight->pop_back();
        }
    }

    if (!(lastChild->isNull)) { 
        if (lastChild->leadingComments.size() > 0 &&
            lastChild->leadingComments[lastChild->leadingComments.size() - 1].range[1] <= this->range[0]) {
            this->leadingComments = lastChild->leadingComments;
            lastChild->leadingComments.clear();
            lastChild->commentsIntoJson(LEADING);
            this->commentsIntoJson(LEADING);
        }
    } else if (extra.leadingComments.size() > 0 && 
               extra.leadingComments[extra.leadingComments.size() - 1].range[1] <= this->range[0]) {
        this->leadingComments = extra.leadingComments;
        extra.leadingComments.clear();
        this->commentsIntoJson(LEADING);
    }

    if (trailingComments.size() > 0) {
        this->trailingComments = trailingComments;
        this->commentsIntoJson(! LEADING);
    }

    bottomRight->push_back(this);
 DEBUGOUT();
}

//#CLEAR
void Node::finish() { DEBUGIN(" Node::finish()");
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

//#CLEAR
void Node::finishArrayExpression(vector< Node >& elements) { DEBUGIN(" Node::finishArrayExpression(vector< Node >& elements)");
    addType("ArrayExpression");
    nodeVec("elements", elements);
    this->finish();
 DEBUGOUT();
}

void Node::finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression) { DEBUGIN(" Node::finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression)");
    addType("ArrowFunctionExpression");

    jvput_null("id");
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", expression);
    this->finish();
 DEBUGOUT();
}

//#CLEAR
void Node::finishAssignmentExpression(u16string oper, Node& left, Node& right) { DEBUGIN(" Node::finishAssignmentExpression(u16string oper, Node& left, Node& right)");

    addType("AssignmentExpression");
    jvput("operator", s(oper));

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
 DEBUGOUT();
}

//#CLEAR
void Node::finishBinaryExpression(u16string oper, Node& left, Node& right) { DEBUGIN(" Node::finishBinaryExpression(u16string oper, Node& left, Node& right)");
    addType((oper == u"||" || oper == u"&&") ? "LogicalExpression" : "BinaryExpression");
    jvput("operator", s(oper));


    reg("left", left); 
    reg("right", right);
    this->finish();
 DEBUGOUT();
}

//#CLEAR
void Node::finishBlockStatement(vector< Node >& body) { DEBUGIN(" Node::finishBlockStatement(vector< Node >& body)");
    addType("BlockStatement");
    nodeVec("body", body);
    this->finish();
 DEBUGOUT();
}

//#CLEAR
void Node::finishBreakStatement(Node& label) { DEBUGIN(" Node::finishBreakStatement(Node& label)");
    addType("BreakStatement");
    reg("label", label);
    this->finish();  DEBUGOUT();
}

//#CLEAR
void Node::finishCallExpression(Node& callee, vector< Node >& args) { DEBUGIN(" Node::finishCallExpression(Node& callee, vector< Node >& args)");
    addType("CallExpression");
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishCatchClause(Node& param, Node& body) { DEBUGIN(" Node::finishCatchClause(Node& param, Node& body)");
    addType("CatchClause");
    reg("param", param);
    reg("body", body);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishConditionalExpression(Node& test, Node& consequent, Node& alternate) { DEBUGIN(" Node::finishConditionalExpression(Node& test, Node& consequent, Node& alternate)");
    addType("ConditionalExpression");
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishContinueStatement(Node& label) { DEBUGIN(" Node::finishContinueStatement(Node& label)");
    addType("ContinueStatement");
    reg("label", label);
    this->finish();  DEBUGOUT();
}

//#CLEAR
void Node::finishDebuggerStatement() { DEBUGIN(" Node::finishDebuggerStatement()");
    addType("DebuggerStatement");
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishDoWhileStatement(Node& body, Node& test) { DEBUGIN(" Node::finishDoWhileStatement(Node& body, Node& test)");
    addType("DoWhileStatement");
    reg("body", body);
    reg("test", test);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishEmptyStatement() { DEBUGIN(" Node::finishEmptyStatement()");
    addType("EmptyStatement");
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishExpressionStatement(Node expression) { DEBUGIN(" Node::finishExpressionStatement(Node expression)");
    addType("ExpressionStatement");
    reg("expression", expression);
    this->finish();  DEBUGOUT();
}

//#CLEAR
void Node::finishForStatement(Node& init, Node& test, Node& update, Node& body) { DEBUGIN(" Node::finishForStatement(Node& init, Node& test, Node& update, Node& body)");
    addType("ForStatement");
    reg("init", init);
    reg("test", test);
    reg("update", update);
    reg("body", body);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishForInStatement(Node& left, Node& right, Node& body) { DEBUGIN(" Node::finishForInStatement(Node& left, Node& right, Node& body)");
    addType("ForInStatement");
    reg("left", left);
    reg("right", right);
    reg("body", body);
    jvput("each", false);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishFunctionDeclaration(Node& id, vector< Node >& params, 
                               vector< Node >& defaults, Node& body) { DEBUGIN("Node::finishFunctionDeclaration(Node, vector<Node>, vector<Node>, Node");
    addType("FunctionDeclaration");
    reg("id", id);
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", false);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishFunctionExpression(Node& id, vector< Node >& params, 
                                    vector< Node >& defaults, Node& body) { DEBUGIN("Node::finishFunctionExpression(Node, vector<Node>, vector<Node>, Node");
    addType("FunctionExpression");
    reg("id", id);
    nodeVec("params", params);
    nodeVec("defaults", defaults);
    reg("body", body);
    jvput_null("rest");
    jvput("generator", false);
    jvput("expression", false);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishIdentifier(u16string name) { DEBUGIN(" Node::finishIdentifier(u16string name)");
    addType("Identifier");
    this->name = name;
    jvput("name", s(name));
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishIfStatement(Node& test, Node& consequent, Node& alternate) { DEBUGIN(" Node::finishIfStatement(Node& test, Node& consequent, Node& alternate)");
    addType("IfStatement");
    reg("test", test);
    reg("consequent", consequent);
    reg("alternate", alternate);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishLabeledStatement(Node label, Node body) { DEBUGIN(" Node::finishLabeledStatement(Node label, Node body)");
    addType("LabeledStatement");
    reg("label", label);
    reg("body", body);
    this->finish(); DEBUGOUT();
}

//#CLEAR ?maybe check against js to make sure we're not missing anything.
void Node::finishLiteral(TokenStruct token) { DEBUGIN(" Node::finishLiteral(TokenStruct token)");
    addType("Literal");
    if (token.literaltype == LiteralType["String"]) {
        jvput("value", s(token.strvalue));
    } else if (token.literaltype == LiteralType["Int"]) {
        jvput_dbl("value", s(token.strvalue));
    } else if (token.literaltype == LiteralType["Double"]) {
        jvput_dbl("value", s(token.strvalue));
    } else if (token.literaltype == LiteralType["Bool"]) {
        jvput("value", token.bvalue);
    } else if (token.literaltype == LiteralType["Null"]) {
        jvput_null("value");
    } else if (token.literaltype == LiteralType["Regexp"]) {
        json_put(jv, "value", 
                 vec2json<string>({s(token.strvalue), s(token.flags)}));
        regexPaths.push_back({"."});
    }
    jvput("raw", s(slice(sourceraw, token.start, token.end)));
    this->finish();
 DEBUGOUT();
}

//#CLEAR
void Node::finishMemberExpression(char16_t accessor, Node& object, Node& property) { DEBUGIN(" Node::finishMemberExpression(char16_t accessor, Node& object, Node& property)");
    addType("MemberExpression");
    jvput("computed", (accessor == u'['));
    reg("object", object);
    reg("property", property);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishNewExpression(Node& callee, vector<Node>& args) { DEBUGIN(" Node::finishNewExpression(Node& callee, vector<Node>& args)");
    addType("NewExpression");
    reg("callee", callee);
    nodeVec("arguments", args);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishObjectExpression(vector<Node>& properties) { DEBUGIN(" Node::finishObjectExpression(vector<Node>& properties)");
    addType("ObjectExpression");
    nodeVec("properties", properties);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishPostfixExpression(u16string oper, Node& argument) { DEBUGIN(" Node::finishPostfixExpression(u16string oper, Node& argument)");
    addType("UpdateExpression");
    jvput("oper", s(oper));
    reg("argument", argument);
    jvput("prefix", false);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishProgram(vector< Node >& body) { DEBUGIN(" Node::finishProgram(vector< Node >& body)");
    addType("Program");
    nodeVec("body", body);
    this->finish(); 
    //no parent node to call reg so add these atts. here.
    if (extra.range) {
        json_put(jv, "range",vec2json<int>({range[0], range[1]}));
    }
    if (extra.loc) {
        json_put(jv, "loc", locToJson(loc));
    }
    DEBUGOUT();    
}

//#CLEAR
void Node::finishProperty(u16string kind, Node& key, Node& value) { DEBUGIN(" Node::finishProperty(u16string kind, Node& key, Node& value)");
    addType("Property");
    reg("key", key);
    reg("value", value);
    jvput("kind", s(kind));
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishReturnStatement(Node& argument) { DEBUGIN(" Node::finishReturnStatement(Node& argument)");
    addType("ReturnStatement");
    reg("argument", argument);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishSequenceExpression(vector< Node >& expressions) { DEBUGIN(" Node::finishSequenceExpression(vector< Node >& expressions)");
    addType("SequenceExpression");
    this->expressions = expressions;
    nodeVec("expressions", expressions);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishSwitchCase(Node& test, vector< Node >& consequent) { DEBUGIN(" Node::finishSwitchCase(Node& test, vector< Node >& consequent)");
    addType("SwitchCase");
    reg("test", test);
    nodeVec("consequent", consequent);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishSwitchStatement(Node& discriminant, vector < Node >& cases) { DEBUGIN(" Node::finishSwitchStatement(Node& discriminant, vector < Node >& cases)");
    addType("SwitchStatement");
    reg("discriminant", discriminant);
    nodeVec("cases", cases);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishThisExpression() { DEBUGIN(" Node::finishThisExpression()");
    addType("ThisExpression");
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishThrowStatement(Node& argument) { DEBUGIN(" Node::finishThrowStatement(Node& argument)");
    addType("ThrowStatement");
    reg("argument", argument);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishTryStatement(Node& block, vector<Node>& guardedHandlers, 
                        vector<Node>& handlers, Node& finalizer) {
    addType("TryStatement");
    reg("block", block);
    nodeVec("guardedHandlers", guardedHandlers);
    nodeVec("handlers", handlers);
    reg("finalizer", finalizer);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishUnaryExpression(u16string oper, Node& argument) { DEBUGIN(" Node::finishUnaryExpression(u16string oper, Node& argument)");
    addType((oper == u"++" || oper == u"--") ? 
            "UpdateExpression" : "UnaryExpression");
    jvput("operator", s(oper));
    reg("argument", argument);
    jvput("prefix", true);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishVariableDeclaration(vector< Node >& declarations, 
                                     u16string kind) {

    addType("VariableDeclaration");
    nodeVec("declarations", declarations);
    jvput("kind", s(kind));
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishVariableDeclarator(Node& id, Node& init) { DEBUGIN(" Node::finishVariableDeclarator(Node& id, Node& init)");
    addType("VariableDeclarator");
    reg("id", id);
    reg("init", init);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishWhileStatement(Node& test, Node& body) { DEBUGIN(" Node::finishWhileStatement(Node& test, Node& body)");
    addType("WhileStatement");
    reg("test", test);
    reg("body", body);
    this->finish(); DEBUGOUT();
}

//#CLEAR
void Node::finishWithStatement(Node& object, Node& body) { DEBUGIN(" Node::finishWithStatement(Node& object, Node& body)");
    addType("WithStatement");
    reg("object", object);
    reg("body", body);
    this->finish(); DEBUGOUT();
}


class WrappingNode : public Node {
public:
    WrappingNode(TokenStruct startToken) : Node(true, true) {
        DEBUGIN("WrappingNode(Token)");
 
        if (extra.range) {
            hasRange = true;
            range[0] = startToken.start;
            range[1] = 0;
        }
 
        if (extra.loc) {
            loc = this->WrappingSourceLocation(startToken);
        }
 
        DEBUGOUT("Wr");
    }
    Loc WrappingSourceLocation(TokenStruct startToken) {
        DEBUGIN("WrappingSourceLocation (Token)");
        Loc result;
        if (startToken.type == Token["StringLiteral"]) {
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
      DEBUGOUT("WraSrcLoc"); return result;
    }
};
    
// Return true if there is a line terminator before the next token.

//#CLEAR+
bool peekLineTerminator() { DEBUGIN(" peekLineTerminator()");
    int pos = idx,
        line = lineNumber,
        start = lineStart;
    bool found;
    skipComment();
    found = (lineNumber != line);
    idx = pos;
    lineNumber = line;
    lineStart = start;
  DEBUGOUT(); return found;
}


// Throw an exception
void throwToJS(ExError err) { DEBUGIN(" throwToJS(ExError err)");
    throw runtime_error(toU8string(err.description));
}


ExError genExError(TokenStruct token, u16string messageFormat, 
                   vector<u16string> args) { DEBUGIN(" genExError");
    ExError error;
    int searchresult;
    u16string searchkey, msg = messageFormat;
    for (int i=0; i<args.size(); i++) {
        searchkey=u"%";
        //#all this conversion is a bit ridiculous. it may
        //#be simpler here to work with just strings.
        searchkey.append(toU16string(to_string(i)));
        searchresult = msg.find(searchkey);
        assert(searchresult != u16string::npos, 
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
  DEBUGOUT("genExErr"); return error;
}

void throwError(TokenStruct token, u16string messageFormat, vector<u16string> args) { DEBUGIN(" throwError(TokenStruct token, u16string messageFormat, vector<u16string> args)");
    throwToJS(genExError(token, messageFormat, args));
}

void throwErrorTolerant(TokenStruct token, u16string messageFormat, vector<u16string> args) { DEBUGIN(" throwErrorTolerant(TokenStruct token, u16string messageFormat, vector<u16string> args)");
    ExError result = genExError(token, messageFormat, args);
    if (extra.errorTolerant) {
        extra.errors.push_back(result);
    } else {
        throwToJS(result);
    }
 DEBUGOUT("throwErrTol");
}

// Throw an exception because of the token.

void throwUnexpected(TokenStruct token) { DEBUGIN(" throwUnexpected(TokenStruct token)");
    if (token.type == Token["EOF"]) {
        throwError(token, Messages["UnexpectedEOS"], {});
    }

    if (token.type == Token["NumericLiteral"]) {
        throwError(token, Messages["UnexpectedNumber"], {});
    }

    if (token.type == Token["StringLiteral"]) {
        throwError(token, Messages["UnexpectedString"], {});
    }

    if (token.type == Token["Identifier"]) {
        throwError(token, Messages["UnexpectedIdentifier"], {});
    }

    if (token.type == Token["Keyword"]) {
        if (isFutureReservedWord(token.strvalue)) {
            throwError(token, Messages["UnexpectedReserved"],{});
        } else if (strict && isStrictModeReservedWord(token.strvalue)) {
            throwErrorTolerant(token, Messages["StrictReservedWord"], {});
          DEBUGOUT(); return;
        }
        throwError(token, Messages["UnexpectedToken"], {token.strvalue});
    }

    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages["UnexpectedToken"], {token.strvalue});
}
    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.
   


//#CLEAR
void expect(u16string value) { 
    //DEBUGIN(" expect(u16string value)");
    
    TokenStruct token = lex();
    
    
    if (token.type != Token["Punctuator"] || 
        /*!(has<int>(token.type, {NULLTOKEN.type, 
                        Token["Keyword"],  //# don't include punctuator.
                        Token["StringLiteral"],
                        Token["Identifier"]})) ||*/
            token.strvalue != value) {
        throwUnexpected(token); 
    }
    // DEBUGOUT("expect");
}


 
//#CLEAR
void expectTolerant(u16string value) {
    // DEBUGIN(" expectTolerant(u16string value)");
    if (extra.errorTolerant) {
        TokenStruct token = lookahead;


        if (token.type != Token["Punctuator"] || 

            /*!(has<int>(token.type, {
                        NULLTOKEN.type, 
                            Token["Keyword"],  //# don't include punctuator.
                            Token["StringLiteral"],
                            Token["Identifier"]})) || */ 
                token.strvalue != value) {
            throwErrorTolerant(token, Messages["UnexpectedToken"], 
                               {token.strvalue});
        } else {
            lex();
        }
    } else {
        expect(value);
    }
    // DEBUGOUT("expectTol");

}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
//#CLEAR
void expectKeyword(const u16string keyword) { 
    TokenStruct token = lex();
    if (token.type != Token["Keyword"] || 
        token.strvalue != keyword) {
        throwUnexpected(token);
    }

}


// Return true if the next token matches the specified punctuator.
//#CLEAR
bool match(const u16string value) { 
  return lookahead.type == Token["Punctuator"] && lookahead.strvalue == value;
}

// Return true if the next token matches the specified keyword

//#CLEAR
bool matchKeyword(const u16string keyword) {
    // DEBUGIN(" matchKeyword(const u16string keyword)");
    //  DEBUGOUT("matchKey"); 
return lookahead.type == Token["Keyword"] && lookahead.strvalue == keyword;
}

    // Return true if the next token is an assignment operator

//#CLEAR
bool matchAssign() { 
    if (lookahead.type != Token["Punctuator"]) {
     return false;
    }
    u16string op = lookahead.strvalue;
  return has<u16string>(op, {//?I'm assuming lookahead.value will be a from-input val?
                u"=", 
                u"*=",
                u"/=",
                u"%=",
                u"+=",
                u"-=",
                u"<<=",
                u">>=",
                u">>>=",
                u"&=",
                u"^=",
                u"|="});
}

//#forward declarations of out-of-dependency order
//#or cross-dependent functions, in rough order of req.
Node parseAssignmentExpression();
Node parseFunctionSourceElements();
Node parseVariableIdentifier();
Node parseExpression();
Node parseFunctionExpression();
Node parseLeftHandSideExpression();
Node parseSourceElement();
Node parseStatement();
vector<Node> parseVariableDeclarationList(u16string in);
Node parseFunctionDeclaration();


//#can't dynamically initialize empty vectors
//#if func is passed by reference.
vector<Node> EMPTY_NODE_LIST;

//#CLEAR+
void consumeSemicolon() { DEBUGIN(" consumeSemicolon()");
    int line;

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B || match(u";")) { 
        lex();
      DEBUGOUT(); return;
    }

    line = lineNumber;
    skipComment();
    if (lineNumber != line) {
      DEBUGOUT(); return;
    }

    if (lookahead.type != Token["EOF"] && !match(u"}")) { 
        throwUnexpected(lookahead); 
    }
 DEBUGOUT("consumeSemi");
}

    // Return true if provided expression is LeftHandSideExpression

//#CLEAR
bool isLeftHandSide(Node expr) { DEBUGIN("   isLeftHandSide(Node expr)");
  DEBUGOUT("isLeft"); return expr.type == Syntax["Identifier"] || expr.type == Syntax["MemberExpression"];
};


// 11.1.4 Array Initialiser


//#CLEAR+
Node parseArrayInitialiser() { DEBUGIN(" parseArrayInitialiser()");
    vector< Node > elements;
    Node node(true, true);    
    
    expect(u"[");
    
    while (!match(u"]")) {

        if (match(u",")) {
            lex();
            elements.push_back(NULLNODE);
        } else {
            elements.push_back(parseAssignmentExpression()); 
            if (!match(u"]")) {
                expect(u",");
            }
        }
    }

    lex();

    node.finishArrayExpression(elements);
  DEBUGOUT("parseArrInit"); return node;
}

// 11.1.5 Object Initialiser

//#CLEAR+
Node parsePropertyFunction(vector<Node>& param, TokenStruct first) { DEBUGIN(" parsePropertyFunction(vector<Node>& param, TokenStruct first)");
    bool previousStrict;
    Node body(false, true), node(true, true);

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (!(first.isNull) && strict && isRestrictedWord(param[0].name)) {
        throwErrorTolerant(first, Messages["StrictParamName"],{});
    }
    strict = previousStrict;
    node.finishFunctionExpression(NULLNODE, param, 
                                  EMPTY_NODE_LIST, body); 
  DEBUGOUT("parsePropFunction"); return node;
}

//#CLEAR+
Node parseObjectPropertyKey() { DEBUGIN(" parseObjectPropertyKey()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token.type == Token["StringLiteral"] || 
        token.type == Token["NumericLiteral"]) {
        if (strict && token.octal) {
            throwErrorTolerant(token, Messages["StrictOctalLiteral"], {});
        }
        node.finishLiteral(token);
      DEBUGOUT(); return node;
    }

    node.finishIdentifier(token.strvalue);
  DEBUGOUT("ParseObjPropKey"); return node;
}
//@parseInt


//@Empty_node_list is passed as a vector_of_arrays,
//@ where normally what would be passed into .finishFunctionExpression is
//@ an undefined variable. this might make the value null expectedly,
//@ while making this version render empty list.
//#CLEAR+
Node parseObjectProperty() { DEBUGIN(" parseObjectProperty()");
    TokenStruct token;
    vector<Node> param;
    Node id(false, true), key(false, true), value(false, true), 
        node(true,true);

    token = lookahead;

    if (token.type == Token["Identifier"]) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token.strvalue == u"get" && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            expect(u")");
            value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                          NULLTOKEN);
            node.finishProperty(u"get", key, value);
          DEBUGOUT("parseObjProp"); return node;
        }
        if (token.strvalue == u"set" && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            token = lookahead;
            if (token.type != Token["Identifier"]) {
                expect(u")");
                throwErrorTolerant(token, 
                                   Messages["UnexpectedToken"], 
                                   {token.strvalue});
                value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                              NULLTOKEN);
            } else {
                param = vector< Node >({ parseVariableIdentifier() });
                expect(u")");
                value = parsePropertyFunction(param, token);
            }
            node.finishProperty(u"set", key, value);
          DEBUGOUT("parseObjProp"); return node;
        }
        expect(u":");
        value = parseAssignmentExpression();
        node.finishProperty(u"init", id, value);
      DEBUGOUT("parseObjProp"); return node;
    }
    if (token.type == Token["EOF"] || token.type == Token["Punctuator"]) {
        throwUnexpected(token);
      DEBUGOUT("parseObjProp"); return NULLNODE; //#just to satisfy warnings.
    } else {
        key = parseObjectPropertyKey();
        expect(u":");
        value = parseAssignmentExpression();
        node.finishProperty(u"init", key, value);
      DEBUGOUT("parseObjProp"); return node;
    }
}

string json_tostring(json_object * in) { DEBUGIN("json_tostring");
    json_type objtype = json_object_get_type(in);
    if (objtype == json_type_string) {
      return DEBUGRET("json_tostring", json_object_get_string(in));
    } else if (objtype == json_type_double) {
      return DEBUGRET("json_tostring", to_string(json_object_get_double(in)));
    } else if (objtype == json_type_boolean) {
      return DEBUGRET("json_tostring", to_string(json_object_get_boolean(in)));
    }
  DEBUGOUT("json_tostring"); return "";
}

//#CLEAR
Node parseObjectInitialiser() { DEBUGIN(" parseObjectInitialiser()");
    vector<Node> properties;
    TokenStruct token;
    Node node(true, true), property(false, true);
    json_object * keyobj;
    string keytype, key, name, kindname;
    int kind;
    map<string, int> kmap;

    expect(u"{");

    while (!match(u"}")) {
        property = parseObjectProperty();
        keyobj = json_require(property.jv, "key");
        keytype = json_object_get_string(
                                         json_require(keyobj, "type"));

        if (keytype == toU8string(Syntax["Identifier"])) {
            name = json_object_get_string(
                                        json_require(keyobj, "name"));
        } else {
            name = json_tostring(json_require(keyobj, "value"));
        }
        kindname = json_object_get_string(json_require(property.jv, "kind"));
        kind = (kindname == "init") ? PropertyKind["Data"] : (kindname == "get") ? PropertyKind["Get"] : PropertyKind["Set"];

        key = "$";
        key.append(name);
        if (hasStringKey<int>(key,kmap)) {
            if (kmap[key] == PropertyKind["Data"]) {
                if (strict && kind == PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, Messages["StrictDuplicateProperty"],{});
                } else if (kind != PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorDataProperty"],{});
                }
            } else {
                if (kind == PropertyKind["Data"]) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorDataProperty"],{});
                } else if (kmap[key] & kind) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorGetSet"],{});
                }
            }
            kmap[key] |= kind;
        } else {
            kmap[key] = kind;
        }

        properties.push_back(property);

        if (!match(u"}")) {
            expectTolerant(u",");
        }
    }

    expect(u"}");

    node.finishObjectExpression(properties);
  DEBUGOUT("parseObjectInit"); return node;
}

// 11.1.6 The Grouping Operator
//#CLEAR+
Node parseGroupExpression() { DEBUGIN(" parseGroupExpression()");
    Node expr(false, true);
    expect(u"(");
    if (match(u")")) {
        lex();
      DEBUGOUT(); return PlaceHolders["ArrowParameterPlaceHolder"];
    }
    ++(state.parenthesisCount);
    expr = parseExpression();
    expect(u")");
  DEBUGOUT("parseGroupExpr"); return expr;
}


// 11.1 Primary Expressions
//#CLEAR+
Node parsePrimaryExpression() { DEBUGIN(" parsePrimaryExpression()");
    int type; 
    TokenStruct token;
    Node expr(false, true), node(false, true);

    if (match(u"(")) {
      return DEBUGRET("", parseGroupExpression());
    }

    if (match(u"[")) {
      return DEBUGRET("", parseArrayInitialiser());
    }

    if (match(u"{")) {
      return DEBUGRET("", parseObjectInitialiser());
    }

    type = lookahead.type;
    node.lookavailInit();

    expr = node;

    if (type == Token["Identifier"]) {
        expr.finishIdentifier(lex().strvalue);
    } else if (type == Token["StringLiteral"] || 
               type == Token["NumericLiteral"]) {

        if (strict && lookahead.octal) {
            throwErrorTolerant(lookahead, 
                               Messages["StrictOctalLiteral"], {});
        }
        expr.finishLiteral(lex());

    } else if (type == Token["Keyword"]) {
        if (matchKeyword(u"function")) {
            expr.unused();
          return DEBUGRET("", parseFunctionExpression());
        }
        if (matchKeyword(u"this")) {
            lex();
            expr.finishThisExpression();
        } else {
            throwUnexpected(lex());
        }
    } else if (type == Token["BooleanLiteral"]) {
        token = lex();
        token.bvalue = (token.strvalue == u"true");
        token.literaltype = LiteralType["Bool"];
        expr.finishLiteral(token);
    } else if (type == Token["NullLiteral"]) {
        token = lex();
        token.isNull = true;
        token.literaltype = LiteralType["Null"];
        expr.finishLiteral(token);
    } else if (match(u"/") || match(u"/=")) {
        if (extra.tokenTracking) {
            expr.finishLiteral(collectRegex());
        } else {
            expr.finishLiteral(scanRegExp());
        }
        peek();
    } else {
        throwUnexpected(lex());
    }

  DEBUGOUT(); return expr;
}

// 11.2 Left-Hand-Side Expressions
//# CLEAR+
vector< Node > parseArguments() { DEBUGIN(" parseArguments()");
    vector< Node > args; 
    expect(u"(");
    if (!match(u")")) {
        while (idx < length) {
            args.push_back(parseAssignmentExpression());
            if (match(u")")) {
                break;
            }
            expectTolerant(u",");
        }
    }
    expect(u")");
  DEBUGOUT("parseArgu"); return args;
}

//#CLEAR+
Node parseNonComputedProperty() { DEBUGIN(" parseNonComputedProperty()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    if (!isIdentifierName(token)) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.strvalue);
  DEBUGOUT(); return node;
}

//#CLEAR+
Node parseNonComputedMember() { DEBUGIN(" parseNonComputedMember()");
    expect(u".");
  return DEBUGRET("parseNonComp", parseNonComputedProperty());
}

//#CLEAR+
Node parseComputedMember() { DEBUGIN(" parseComputedMember()");
    Node expr(false, true);
    expect(u"[");
    expr = parseExpression();
    expect(u"]");
  DEBUGOUT("parseComp"); return expr;
}

//#CLEAR+
Node parseNewExpression() { DEBUGIN(" parseNewExpression()");
    vector< Node > args;
    Node callee(false, true), node(true, true);

    expectKeyword(u"new");
    callee = parseLeftHandSideExpression();
    if (match(u"(")) { args = parseArguments(); }

    node.finishNewExpression(callee, args);
  DEBUGOUT("parseNewExpr"); return node;
}

//#CLEAR
Node parseLeftHandSideExpressionAllowCall() { DEBUGIN(" parseLeftHandSideExpressionAllowCall()");
    vector< Node > args;
    Node expr(false, true), property(false, true), tmpnode(false,true);
    TokenStruct startToken;
    bool previousAllowIn = state.allowIn;
    bool sub = false;

    startToken = lookahead;
    state.allowIn = true;
    expr = matchKeyword(u"new") ? 
        parseNewExpression() : parsePrimaryExpression();



    for (;;) {
        if (match(u".")) {
            sub = true;
            property = parseNonComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
        } else if (match(u"(")) {
            sub = true;
            args = parseArguments();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishCallExpression(expr, args);
        } else if (match(u"[")) {
            sub = true;
            property = parseComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
        } else {
            break;
        }
    }

    state.allowIn = previousAllowIn;

  DEBUGOUT("parseLeftHandSideExprAllow"); 
  if (sub) { return tmpnode; }
  else { return expr; }
}

//#CLEAR
Node parseLeftHandSideExpression() { DEBUGIN(" parseLeftHandSideExpression()");
    Node tmpnode(false, true), expr(false, true), property(false, true);
    TokenStruct startToken;
    bool sub = false;

    assert(state.allowIn, "callee of new expression always allow in keyword.");
    startToken = lookahead;
    expr = matchKeyword(u"new") ? 
        parseNewExpression() : parsePrimaryExpression();
    
    for (;;) {
        if (match(u"[")) {
            sub = true;
            property = parseComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
        } else if (match(u".")) {
            sub = true;
            property = parseNonComputedMember();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
        } else {            
            break;
        }
    }
  DEBUGOUT("parseLeftHandSideExpr");
  if (sub) { return tmpnode; }
  else { return expr; }
}

// 11.3 Postfix Expressions
//#CLEAR
Node parsePostfixExpression() { DEBUGIN(" parsePostfixExpression()");
    Node expr(false, true),tmpnode(false, true);
    TokenStruct token, startToken = lookahead;

    expr = parseLeftHandSideExpressionAllowCall();

    if (lookahead.type == Token["Punctuator"]) {
        if ((match(u"++") || match(u"--")) && !peekLineTerminator()) {
            // 11.3.1, 11.3.2
            if (strict && expr.type == Syntax["Identifier"] && 
                isRestrictedWord(expr.name)) {
                throwErrorTolerant(NULLTOKEN,
                                   Messages["StrictLHSPostfix"],{});
            }

            if (!isLeftHandSide(expr)) {
                throwErrorTolerant(NULLTOKEN,
                                   Messages["InvalidLHSInAssignment"],{});
            }

            token = lex();
            tmpnode = WrappingNode(startToken);
            tmpnode.finishPostfixExpression(token.strvalue, expr);
          DEBUGOUT("parsePostfix"); return tmpnode;
        }
    }

  DEBUGOUT("parsePostfix"); return expr;
}

// 11.4 Unary Operators
//#CLEAR+
Node parseUnaryExpression() { DEBUGIN(" parseUnaryExpression()");
    TokenStruct token, startToken;
    Node expr(false, true), tmpnode(false, true);
    u16string exprname;

    if (lookahead.type != Token["Punctuator"] 
        && lookahead.type != Token["Keyword"]) {
        expr = parsePostfixExpression();
    } else if (match(u"++") || match(u"--")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        // 11.4.4, 11.4.5
        if (strict && expr.type == Syntax["Identifier"] 
            && isRestrictedWord(expr.name)) {
            throwErrorTolerant(NULLTOKEN, Messages["StrictLHSPrefix"],{});
        }

        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, 
                               Messages["InvalidLHSInAssignment"], {});
        }

        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
      DEBUGOUT("parseUnary"); return tmpnode;
    } else if (match(u"+") || match(u"-") || match(u"~") || match(u"!")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
      DEBUGOUT("parseUnary"); return tmpnode;
    } else if (matchKeyword(u"delete") 
               || matchKeyword(u"void") 
               || matchKeyword(u"typeof")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        if (strict && token.strvalue == u"delete" 
            && expr.type == Syntax["Identifier"]) {
            throwErrorTolerant(NULLTOKEN, Messages["StrictDelete"], {});
        }
      DEBUGOUT("parseUnary"); return tmpnode;
    } else {
        expr = parsePostfixExpression();
    }

  DEBUGOUT("parseUnary"); return expr;
}

//#CLEAR+
int binaryPrecedence(TokenStruct token, bool allowIn) {
    DEBUGIN(" binaryPrecedence(Tokenstruct token, bool allowIn)");
    int prec = 0;
    u16string tokval;

    if (token.type != Token["Punctuator"] 
        && token.type != Token["Keyword"]) {
      DEBUGOUT("binaryPrec"); return 0;
    }
    tokval = token.strvalue;
    
    if (tokval == u"||") {
        prec = 1;
    } else if (tokval == u"&&") {
        prec = 2;
    } else if (tokval == u"|") {
        prec = 3;
    } else if (tokval == u"^") {
        prec = 4;
    } else if (tokval == u"&") {
        prec = 5;
    } else if (has(tokval, {u"==", u"!=", u"===", u"!=="})) {
        prec = 6;
    } else if (has(tokval, {u"<", u">", u"<=", u">=", u"instanceof"})) {
        prec = 7;
    } else if (tokval == u"in") {
        prec = allowIn ? 7 : 0;
    } else if (has(tokval,
                   {u"<<", u">>", u">>>"})) {
        prec = 8;
    } else if (tokval == u"+" || tokval == u"-") {
        prec = 9;
    } else if (has(tokval, {u"*", u"/", u"%"})) {
        prec = 11;
    }

  DEBUGOUT("binaryPrec"); return prec;
}

// 11.5 Multiplicative Operators
// 11.6 Additive Operators
// 11.7 Bitwise Shift Operators
// 11.8 Relational Operators
// 11.9 Equality Operators
// 11.10 Binary Bitwise Operators
// 11.11 Binary Logical Operators
//CLEAR?
Node parseBinaryExpression() { DEBUGIN(" parseBinaryExpression()");

    Node tmpnode(false, true), expr(false, true), left(false, true), 
        right(false, true);
    vector < Node > nodestack;
    TokenStruct marker, token;
    vector< TokenStruct > markers, tokstack;
    u16string oper;
    int i, prec;
    
    marker = lookahead;

    left = parseUnaryExpression();

    if (left.type == PlaceHolders["ArrowParameterPlaceHolder"].type) {
        //? placeholder
      DEBUGOUT("parseBinary"); return left;
    }

    token = lookahead;
    prec = binaryPrecedence(token, state.allowIn);
    if (prec == 0) {
      DEBUGOUT("parseBinary"); return left;
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

    while ((prec = binaryPrecedence(lookahead, state.allowIn)) > 0) { //? will this work the same in c++ as in js

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


  DEBUGOUT("parseBinary"); return expr;
}


// 11.12 Conditional Operator

//#CLEAR+
Node parseConditionalExpression() { DEBUGIN(" parseConditionalExpression()");
    Node expr(false, true), tmpnode(false, true), 
        consequent(false, true), alternate(false, true);
    bool previousAllowIn;
    TokenStruct startToken;

    startToken = lookahead;

    expr = parseBinaryExpression();
    if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type) { 
        //? ever supposed to eval. to true? cause it might in some cases
        //? even tho it seems in javascript it never ever will.
        
      DEBUGOUT("parseCondExpr"); return expr;
    }
    if (match(u"?")) {
        lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        state.allowIn = previousAllowIn;
        expect(u":");
        alternate = parseAssignmentExpression();

        tmpnode = WrappingNode(startToken);
        tmpnode.finishConditionalExpression(expr, consequent, alternate);
      DEBUGOUT("parseCondExpr"); return tmpnode;
    }

  DEBUGOUT("parseCondExpr"); return expr;
}

// [ES6] 14.2 Arrow Function
//#CLEAR+
Node parseConciseBody() { DEBUGIN(" parseConciseBody()");
    if (match(u"{")) {
      return DEBUGRET("parseConciseBody", parseFunctionSourceElements());
    }
  return DEBUGRET("parseConciseBody", parseAssignmentExpression());
}

//#CLEAR
void validateParamNode(ReinterpretOptions options,
                       Node param, u16string name) {
    DEBUGIN("validateParamNode(Reinterp, Node, u16str)");
    u16string key = u"$";
    key.append(name);
    if (strict) {
        if (isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages["StrictParamName"];
        }
        if (has(key, options.paramSet)) {
            options.stricted = param;
            options.message = Messages["StrictParamDupe"];
        }
    } else if (options.firstRestricted.isNull) {
        if (isRestrictedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages["StrictParamName"];
        } else if (isStrictModeReservedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages["StrictReservedWord"];
        } else if (has(key, options.paramSet)) {
            options.firstRestricted = param;
            options.message = Messages["StrictParamDupe"];
        }
    }
    options.paramSet.insert(key);
 DEBUGOUT("validateParamNode");
}


ReinterpretOut reinterpretAsCoverFormalsList(vector< Node >& expressions) { 
    DEBUGIN("Reinterpret");
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
        if (param.type == Syntax["Identifier"]) {
            params.push_back(param);
            defaults.push_back(NULLNODE);
            validateParamNode(opts, param, param.name);
        } else if (param.type == Syntax["AssignmentExpression"]) {
            params.push_back(*(param.left));
            defaults.push_back(*(param.right));
            ++defaultCount;
            validateParamNode(opts, *(param.left), (*(param.left)).name);
        } else {
            reOut.isNull = true;
          DEBUGOUT(); return reOut; 
        }
    }

    if (opts.message == Messages["StrictParamDupe"]) {
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
    
  DEBUGOUT("reinterpret"); return reOut;
}
    
//#CLEAR+
Node parseArrowFunctionExpression(ReinterpretOut options, Node node) { DEBUGIN(" parseArrowFunctionExpression(ReinterpretOut options, Node node)");
     bool previousStrict;
     Node body(false, true);

    expect(u"=>");
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
                                       options.defaults, body, 
                                       body.type != Syntax["BlockStatement"]);
  DEBUGOUT("parseArrowFuncExpr"); return node;
}

    // 11.13 Assignment Operators


//#CLEAR
Node parseAssignmentExpression() { DEBUGIN(" parseAssignmentExpression()");
    int oldParenthesisCount;
    TokenStruct token, startToken;
    Node expr(false, true), right(false, true), tmpnode(false, true);
    vector<Node> reIn;
    ReinterpretOut list;


    oldParenthesisCount = state.parenthesisCount;

    startToken = lookahead;
    token = lookahead;

    expr = parseConditionalExpression();

    if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type //? will work?
        || match(u"=>")) {

        if (state.parenthesisCount == oldParenthesisCount ||
                state.parenthesisCount == (oldParenthesisCount + 1)) {      
            if (expr.type == Syntax["Identifier"]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn); 
            } else if (expr.type == Syntax["Assignmentxpression"]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn);
            } else if (expr.type == Syntax["SequenceExpression"]) {
                list = reinterpretAsCoverFormalsList(expr.expressions);
            } else if (expr.type == PlaceHolders["ArrowParameterPlaceHolder"].type) {
                list = reinterpretAsCoverFormalsList(reIn); 
            }
            if (!(list.isNull)) {
              return DEBUGRET("", parseArrowFunctionExpression(list, WrappingNode(startToken)));
            }
        }
    }

    if (matchAssign()) {
        // LeftHandSideExpression


        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, Messages["InvalidLHSInAssignment"], {});
        }

        // 11.13.1
        if (strict && expr.type == Syntax["Identifier"] && isRestrictedWord(expr.name)) {
            throwErrorTolerant(token, Messages["StrictLHSAssignment"], {});
        }

        token = lex();
        right = parseAssignmentExpression();
        tmpnode = WrappingNode(startToken);

        tmpnode.finishAssignmentExpression(token.strvalue, expr, right); 
        return tmpnode;
    }

  DEBUGOUT("parseAssignExpr"); return expr;
}

// 11.14 Comma Operator
//#CLEAR+
Node parseExpression() { DEBUGIN(" parseExpression()");
    Node expr(false, true); 
    TokenStruct startToken = lookahead;
    vector< Node > expressions;

    expr = parseAssignmentExpression();

    if (match(u",")) {
        expressions.push_back(expr);

        while (idx < length) {
            if (!match(u",")) {
                break;
            }
            lex();
            expressions.push_back(parseAssignmentExpression());
        }

        expr = WrappingNode(startToken);
        expr.finishSequenceExpression(expressions);
    }

  DEBUGOUT("parseExpr"); return expr;
}

// 12.1 Block
//#CLEAR+
vector< Node > parseStatementList() { DEBUGIN(" parseStatementList()");
    vector< Node > list;
    Node statement;

    while (idx < length) {
        if (match(u"}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement.isNull) { 
            break;
        }
        list.push_back(statement);
    }

  DEBUGOUT("parseStatementList"); return list;
}

//#CLEAR+
Node parseBlock() { DEBUGIN(" parseBlock()");
    vector< Node > block;
    Node node(true, true);

    expect(u"{");
    block = parseStatementList();
    expect(u"}");
    node.finishBlockStatement(block);
  DEBUGOUT("parseBlock"); return node;
}


// 12.2 Variable Statement

//#CLEAR+
Node parseVariableIdentifier() { DEBUGIN(" parseVariableIdentifier()");
    TokenStruct token;
    Node node(true, true);

    token = lex();

    if (token.type != Token["Identifier"]) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.strvalue);
  DEBUGOUT("parseVariableIdent"); return node;
}

//#CLEAR+
Node parseVariableDeclaration(u16string kind) { DEBUGIN(" parseVariableDeclaration(u16string kind)");
    Node id(false, true), node(true, true), init(false, true);
    init = NULLNODE;

    id = parseVariableIdentifier();

    // 12.2.1
    if (strict && isRestrictedWord(id.name)) {
        throwErrorTolerant(NULLTOKEN, Messages["StrictVarName"], {});
    }

    if (kind == u"const") {
        expect(u"=");
        init = parseAssignmentExpression();
    } else if (match(u"=")) {
        lex();
        init = parseAssignmentExpression();
    }

    node.finishVariableDeclarator(id, init);
  DEBUGOUT("parseVariableDecl"); return node;
}

//#CLEAR+
vector< Node > parseVariableDeclarationList(u16string kind) {
    vector< Node > list; 

    do {
        list.push_back(parseVariableDeclaration(kind));
        if (!match(u",")) {
            break;
        }
        lex();
    } while (idx < length);

  DEBUGOUT("parseVariableDeclarationList"); return list;
}

//#CLEAR
Node parseVariableStatement(Node node) { DEBUGIN(" parseVariableStatement(Node node)");
    vector< Node > declarations;

    expectKeyword(u"var");
    declarations = parseVariableDeclarationList(u""); 
    consumeSemicolon();

    node.finishVariableDeclaration(declarations, u"var");
  DEBUGOUT("parseVariableStatement"); return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//#CLEAR
Node parseConstLetDeclaration(u16string kind) { DEBUGIN(" parseConstLetDeclaration(u16string kind)");
    vector< Node > declarations;
    Node node(true, true);

    expectKeyword(kind);
    declarations = parseVariableDeclarationList(kind);
    consumeSemicolon();
    node.finishVariableDeclaration(declarations, kind);
  DEBUGOUT("parseConstLetDeclaration"); return node;
}

// 12.3 Empty Statement

//#CLEAR
Node parseEmptyStatement() { DEBUGIN(" parseEmptyStatement()");
    Node node(true, true);
    expect(u";");
    node.finishEmptyStatement();
  DEBUGOUT("parseEmptyStatement"); return node;
}

// 12.4 Expression Statement
//#CLEAR
Node parseExpressionStatement(Node node) { DEBUGIN(" parseExpressionStatement(Node node)");
    Node expr(false, true);
    expr = parseExpression();
    consumeSemicolon();
    node.finishExpressionStatement(expr);
  DEBUGOUT(); return node;
}

// 12.5 If statement
//#CLEAR
Node parseIfStatement(Node node) { DEBUGIN(" parseIfStatement(Node node)");
    Node test(false, true), consequent(false, true), alternate(false, true);
    expectKeyword(u"if");
    expect(u"(");
    test = parseExpression();
    expect(u")");
    consequent = parseStatement();
    if (matchKeyword(u"else")) {
        lex();
        alternate = parseStatement();
    } else {
        alternate = NULLNODE;
    }
    node.finishIfStatement(test, consequent, alternate);
  DEBUGOUT("parseIfStatement"); return node;
}

// 12.6 Iteration Statements

//#CLEAR
Node parseDoWhileStatement(Node node) { DEBUGIN(" parseDoWhileStatement(Node node)");
    Node body(false, true), test(false, true);
    bool oldInIteration;

    expectKeyword(u"do");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    expectKeyword(u"while");
    expect(u"(");
    test = parseExpression();
    expect(u")");
    if (match(u";")) {
        lex();
    }
    node.finishDoWhileStatement(body, test);
  DEBUGOUT("parseDoWhileStatement"); return node;
}

//#CLEAR
Node parseWhileStatement(Node node) { DEBUGIN(" parseWhileStatement(Node node)");
    Node test(false, true), body(false, true);
    bool oldInIteration;
    expectKeyword(u"while");
    expect(u"(");
    test = parseExpression();
    expect(u")");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    node.finishWhileStatement(test, body);
  DEBUGOUT("parseWhileStatement"); return node;
}

//#CLEAR
Node parseForVariableDeclaration() { DEBUGIN(" parseForVariableDeclaration()");
    TokenStruct token;
    vector< Node > declarations;
    Node node(true, true);

    token = lex();
    declarations = parseVariableDeclarationList(u"");
    node.finishVariableDeclaration(declarations, token.strvalue);
  DEBUGOUT("parseForVariableDeclaration"); return node;
}

//#CLEAR
Node parseForStatement(Node node) { DEBUGIN(" parseForStatement(Node node)");

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node body(false, true), left(false, true), right(false, true), 
        update(false, true), test(false, true), init(false, true);
    update=NULLNODE; test=NULLNODE; init=NULLNODE;

    expectKeyword(u"for");
    expect(u"(");

    if (match(u";")) {
        lex();
    } else {
        if (matchKeyword(u"var") || matchKeyword(u"let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (json_object_array_length(
                         json_require(init.jv, "declarations")) == 1 
                && matchKeyword(u"in")) { 
                
                lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        } else {
            state.allowIn = false;
            init = parseExpression();
            state.allowIn = previousAllowIn;

            if (matchKeyword(u"in")) {
                // LeftHandSideExpression
                if (!isLeftHandSide(init)) {
                    throwErrorTolerant(NULLTOKEN, 
                                       Messages["InvalidLHSInForIn"],{});
                }

                lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        }

        if (left.isNull) {
            expect(u";");
        }
    }

    if (left.isNull) {

        if (!match(u";")) {
            test = parseExpression();
        }
        expect(u";");

        if (!match(u")")) {
            update = parseExpression();
        }
    }

    expect(u")");

    oldInIteration = state.inIteration;
    state.inIteration = true;

    body = parseStatement();

    state.inIteration = oldInIteration;

    if (left.isNull) {
        node.finishForStatement(init, test, update, body);
    } else {
        node.finishForInStatement(left, right, body);
    }
  DEBUGOUT("parseForStatement"); return node;
}

// 12.7 The continue statement
//#CLEAR
Node parseContinueStatement(Node node) { DEBUGIN(" parseContinueStatement(Node node)");
    Node label(false, true);
    label = NULLNODE;
    u16string key;

    expectKeyword(u"continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration)) {
            throwError(NULLTOKEN, Messages["IllegalContinue"],{});
        }

        node.finishContinueStatement(NULLNODE);
      DEBUGOUT("parseContinueStatement"); return node;
    }

    if (peekLineTerminator()) {
        if (!state.inIteration) {
            throwError(NULLTOKEN, Messages["IllegalContinue"],{});
        }

        node.finishContinueStatement(NULLNODE);
      DEBUGOUT("parseContinueStatement"); return node;
    }

    if (lookahead.type == Token["Identifier"]) {
        label = parseVariableIdentifier();

        key = u"$";
        key.append(label.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["UnknownLabel"], {label.name});
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration)) {
        throwError(NULLTOKEN, Messages["IllegalContinue"],{});
    }

    node.finishContinueStatement(label);
  DEBUGOUT("parseContinueStatement"); return node;
}

// 12.8 The break statement
//#CLEAR
Node parseBreakStatement(Node node) { DEBUGIN(" parseBreakStatement(Node node)");
    Node label(false, true);
    u16string key;

    expectKeyword(u"break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages["IllegalBreak"],{});
        }

        node.finishBreakStatement(NULLNODE);
      DEBUGOUT("parseBreakStatement"); return node;
    }

    if (peekLineTerminator()) {
        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages["IllegalBreak"],{});
        }

        node.finishBreakStatement(NULLNODE);
      DEBUGOUT("parseBreakStatement"); return node;
    }

    if (lookahead.type == Token["Identifier"]) {
        label = parseVariableIdentifier();

        key = u"$";
        key.append(label.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["UnknownLabel"], {label.name});
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration || state.inSwitch)) {
        throwError(NULLTOKEN, Messages["IllegalBreak"], {});
    }

    node.finishBreakStatement(label);
  DEBUGOUT("parseBreakStatement"); return node;
}

// 12.9 The return statement
//#CLEAR
Node parseReturnStatement(Node node) { DEBUGIN(" parseReturnStatement(Node node)");
    Node argument(false, true);
    argument = NULLNODE;

    expectKeyword(u"return");

    if (!(state.inFunctionBody)) {
        throwErrorTolerant(NULLTOKEN, Messages["IllegalReturn"], {});
    }

    // 'return' followed by a space and an identifier is very common.
    if (source(idx) == 0x20) {
        if (isIdentifierStart(source(idx + 1))) {
            argument = parseExpression();
            consumeSemicolon();
            node.finishReturnStatement(argument);
          DEBUGOUT("parseReturnStatement"); return node;
        }
    }

    if (peekLineTerminator()) {
        node.finishReturnStatement(NULLNODE);
      DEBUGOUT("parseReturnStatement"); return node;
    }

    if (!match(u";")) {
        if (!match(u"}") && lookahead.type != Token["EOF"]) {
            argument = parseExpression();
        }
    }

    consumeSemicolon();

    node.finishReturnStatement(argument);
  DEBUGOUT("parseReturnStatement"); return node;
}

// 12.10 The with statement
//#CLEAR
Node parseWithStatement(Node node) { DEBUGIN(" parseWithStatement(Node node)");
    Node object(false, true), body(false, true);

    if (strict) {
        // TODO(ikarienator): Should we update the test cases instead?
        skipComment();
        throwErrorTolerant(NULLTOKEN, Messages["StrictModeWith"], {});
    }

    expectKeyword(u"with");
    expect(u"(");
    object = parseExpression();
    expect(u")");
    body = parseStatement();
    node.finishWithStatement(object, body);
  DEBUGOUT("parseWithStatement"); return node;
}

// 12.10 The swith statement
//#CLEAR
Node parseSwitchCase() { DEBUGIN(" parseSwitchCase()");
    Node test(false, true), statement(false, true), node(true, true);
    vector< Node > consequent;

    if (matchKeyword(u"default")) {
        lex();
        test = NULLNODE;
    } else {
        expectKeyword(u"case");
        test = parseExpression();
    }
    expect(u":");

    while (idx < length) {
        if (match(u"}") || matchKeyword(u"default") || matchKeyword(u"case")) {
            break;
        }
        statement = parseStatement();
        consequent.push_back(statement);
    }

    node.finishSwitchCase(test, consequent);
  DEBUGOUT("parseSwitchCase"); return node;
}

//#CLEAR
Node parseSwitchStatement(Node node) { DEBUGIN(" parseSwitchStatement(Node node)");
    Node discriminant(false, true), clause(false, true); 
    vector< Node > cases; 
    bool oldInSwitch, defaultFound;

    expectKeyword(u"switch");
    expect(u"(");
    discriminant = parseExpression();
    expect(u")");
    expect(u"{");
    if (match(u"}")) {
        lex();
        node.finishSwitchStatement(discriminant, cases);
      DEBUGOUT("parseSwitchStatement"); return node;
    }
    oldInSwitch = state.inSwitch;
    state.inSwitch = true;
    defaultFound = false;

    while (idx < length) {
        if (match(u"}")) {
            break;
        }
        clause = parseSwitchCase();
        if (json_object_is_type(json_require(clause.jv, "test"), 
                                json_type_null)) {
            if (defaultFound) {
                throwError(NULLTOKEN, 
                           Messages["MultipleDefaultsInSwitch"],{});
            }
            defaultFound = true;
        }
        cases.push_back(clause);
    }

    state.inSwitch = oldInSwitch;
    expect(u"}");
    node.finishSwitchStatement(discriminant, cases);
  DEBUGOUT("parseSwitchStatement"); return node;
}

// 12.13 The throw statement
//#CLEAR
Node parseThrowStatement(Node node) { DEBUGIN(" parseThrowStatement(Node node)");
    Node argument(false, true);

    expectKeyword(u"throw");
    if (peekLineTerminator()) {
        throwError(NULLTOKEN, Messages["NewlineAfterThrow"],{});
    }
    argument = parseExpression();
    consumeSemicolon();
    node.finishThrowStatement(argument);
  DEBUGOUT("parseThrowStatement"); return node;
}

// 12.14 The try statement

//#CLEAR
Node parseCatchClause() { DEBUGIN(" parseCatchClause()");
    Node body(false, true), param(false, true), node(true, true);

    expectKeyword(u"catch");

    expect(u"(");
    if (match(u")")) {
        throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (strict && isRestrictedWord(param.name)) { 
        throwErrorTolerant(NULLTOKEN, Messages["StrictCatchVariable"],{});
    }

    expect(u")");
    body = parseBlock();
    node.finishCatchClause(param, body);
  DEBUGOUT("parseCatchClause"); return node;
}

//#CLEAR
Node parseTryStatement(Node node) { DEBUGIN(" parseTryStatement(Node node)");
    Node block(false, true), finalizer(false, true); 
    vector< Node > handlers;

    finalizer = NULLNODE;

    expectKeyword(u"try");

    block = parseBlock();

    if (matchKeyword(u"catch")) {
        handlers.push_back(parseCatchClause());
    }

    if (matchKeyword(u"finally")) {
        lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && finalizer.isNull) {
        throwError(NULLTOKEN, Messages["NoCatchOrFinally"], {});
    }

    node.finishTryStatement(block, EMPTY_NODE_LIST, 
                            handlers, finalizer); 
  DEBUGOUT("parseTryStatement"); return node;
}

// 12.15 The debugger statement
//#CLEAR
Node parseDebuggerStatement(Node node) { DEBUGIN(" parseDebuggerStatement(Node node)");
    expectKeyword(u"debugger");
    consumeSemicolon();
    node.finishDebuggerStatement();
  DEBUGOUT("parseDebuggerStatement"); return node;
}

// 12 Statements

//#partial
Node parseStatement() { DEBUGIN(" parseStatement()");
    int type = lookahead.type;
    u16string key, tokval;
    Node expr(false, true), node(false, true), labeledBody(false, true);

    if (type == Token["EOF"]) {
        throwUnexpected(lookahead);
    }

    if (type == Token["Punctuator"] && lookahead.strvalue == u"{") {
      return DEBUGRET("parseStatement", parseBlock());
    }

    node.lookavailInit();

    if (type == Token["Punctuator"]) {
        tokval = lookahead.strvalue;
        if (tokval == u";") {
          return DEBUGRET("parseStatement", parseEmptyStatement());
        } else if (tokval == u"(") {
          return DEBUGRET("parseStatement", parseExpressionStatement(node));
        }
    } else if (type == Token["Keyword"]) {
        tokval = lookahead.strvalue;
        if (tokval == u"break") {
          return DEBUGRET("parseStatement", parseBreakStatement(node));
        } else if (tokval == u"continue") {
          return DEBUGRET("parseStatement", parseContinueStatement(node));
        } else if (tokval == u"debugger") {
          return DEBUGRET("parseStatement", parseDebuggerStatement(node));
        } else if (tokval == u"do") {
          return DEBUGRET("parseStatement", parseDoWhileStatement(node));
        } else if (tokval == u"for") {
          return DEBUGRET("parseStatement", parseForStatement(node));
        } else if (tokval == u"function") {
            //#oddly enough in js passes node here.
            //#even though has no param even in js.
          return DEBUGRET("parseStatement", parseFunctionDeclaration());
        } else if (tokval == u"if") {
          return DEBUGRET("parseStatement", parseIfStatement(node));
        } else if (tokval == u"return") {
          return DEBUGRET("parseStatement", parseReturnStatement(node));
        } else if (tokval == u"switch") {
          return DEBUGRET("parseStatement", parseSwitchStatement(node));
        } else if (tokval == u"throw") {
          return DEBUGRET("parseStatement", parseThrowStatement(node));
        } else if (tokval == u"try") {
          return DEBUGRET("parseStatement", parseTryStatement(node));
        } else if (tokval == u"var") {
          return DEBUGRET("parseStatement", parseVariableStatement(node));
        } else if (tokval == u"while") {
          return DEBUGRET("parseStatement", parseWhileStatement(node));
        } else if (tokval == u"with") {
          return DEBUGRET("parseStatement", parseWithStatement(node));
        }
    }

    expr = parseExpression(); 

    // 12.12 Labelled Statements
    if ((expr.type == Syntax["Identifier"]) && match(u":")) {
        lex();

        key = u"$";
        key.append(expr.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["Redeclaration"], 
                       {u"Label", expr.name}); 
        }

        state.labelSet.insert(key);
        labeledBody = parseStatement();
        state.labelSet.erase(key);
        node.finishLabeledStatement(expr, labeledBody);
      DEBUGOUT("parseStatement"); return node;
    }

    consumeSemicolon();

    node.finishExpressionStatement(expr);
  DEBUGOUT("parseStatement"); return node;
}

// 13 Function Definition
//#CLEAR
Node parseFunctionSourceElements() { DEBUGIN(" parseFunctionSourceElements()");
    Node sourceElement(false, true), node(true, true);
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive,
        oldLabelSet, oldInIteration, oldInSwitch, oldInFunctionBody, oldParenthesisCount;
    StateStruct oldstate;

    expect(u"{");

    firstRestricted.isNull = true;
    while (idx < length) {
        if (lookahead.type != Token["StringLiteral"]) {
            break;
        }
        token = lookahead;

        sourceElement = parseSourceElement(); 
        //# returns in turn the value of parseStatement for stringLiteral 
        //# so returns a string literal expression node wrapped in an expressionStatement node.
        sourceElements.push_back(sourceElement); 
        if (strcmp(
                   json_object_get_string(json_require(
                                                       json_require(
                                                                    sourceElement.jv, 
                                                                    "expression"),
                                                       "type")
                                          ), 
                   toU8string(Syntax["Literal"]).data()) != 0) {
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
                throwErrorTolerant(firstRestricted, Messages["StrictOctalLiteral"], {});
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
        if (match(u"}")) {
            break;
        }
        sourceElement = parseSourceElement();
        if (sourceElement.isNull) {
            break;
        }
        sourceElements.push_back(sourceElement);
    }

    expect(u"}");

    state.labelSet = oldstate.labelSet;
    state.inIteration = oldstate.inIteration;
    state.inSwitch = oldstate.inSwitch;
    state.inFunctionBody = oldstate.inFunctionBody;
    state.parenthesisCount = oldstate.parenthesisCount;

    node.finishBlockStatement(sourceElements);
  DEBUGOUT("parseFunctionSourceElements"); return node;
}

//#CLEAR
void validateParam(ParseParamsOptions options, 
                   TokenStruct param, u16string name) {
    u16string key = u"$";
    key.append(name);
    if (strict) {
        if (isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages["StrictParamName"];
        }
        if (has(key, options.paramSet)) {
            options.stricted = param;
            options.message = Messages["StrictParamDupe"];
        }
    } else if (options.firstRestricted.isNull) {
        if (isRestrictedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages["StrictParamName"];
        } else if (isStrictModeReservedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages["StrictReservedWord"];
        } else if (has(key, options.paramSet)) {
            options.firstRestricted = param;
            options.message = Messages["StrictParamDupe"];
        }
    }
    options.paramSet.insert(key);
 DEBUGOUT("validateParam");
}


//#CLEAR
bool parseParam(ParseParamsOptions options) { DEBUGIN(" parseParam(ParseParamsOptions options)");
    TokenStruct token; 
    Node param(false, true), def(false, true);

    token = lookahead;
    param = parseVariableIdentifier();
    //! MAJOR CONCERN: if a number is put where an argument is here,
    //! will that cause an ungraceful crash because validateParam is expecting
    //! a string so we cast it here, but the token grabbed might be
    //! a numeric literal?? if so perhaps the thing to do is check
    //! for token.type and if it is of numeric types cast it?
    validateParam(options, token, token.strvalue);
    if (match(u"=")) {
        lex();
        def = parseAssignmentExpression();
        ++(options.defaultCount);
    }

    options.params.push_back(param);
    options.defaults.push_back(def);

  return DEBUGRET("parseParam", !match(u")"));
}

//#CLEAR
ParseParamsOut parseParams(TokenStruct firstRestricted) { DEBUGIN(" parseParamS(TokenStruct firstRestricted)");
    ParseParamsOptions options;
    ParseParamsOut out;
    options.defaultCount = 0;
    options.firstRestricted = firstRestricted;
    options.stricted.isNull = true;

    expect(u"(");

    if (!match(u")")) {
        while (idx < length) {
            if (!parseParam(options)) {
                break;
            }
            expect(u",");
        }
    }

    
    expect(u")");
    DEBUGIN(" aquel ");
    if (options.defaultCount == 0) {
        options.defaults.clear();
    }

    out.params = options.params;
    out.defaults = options.defaults;
    out.stricted = options.stricted;
    out.firstRestricted = options.firstRestricted;
    out.message = options.message;
  DEBUGOUT("parseParamS"); return out;
}

//#CLEAR
Node parseFunctionDeclaration() { DEBUGIN(" parseFunctionDeclaration()");
    TokenStruct token, firstRestricted, stricted;
    u16string message, tokval;
    Node body(false, true), id(false, true), node(true, true);    
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    bool previousStrict;
    id = NULLNODE;
    
    expectKeyword(u"function");
 
    token = lookahead;
    id = parseVariableIdentifier(); 

    firstRestricted.isNull = true;
    if (strict) {
        if (isRestrictedWord(token.strvalue)) {
            throwErrorTolerant(token, Messages["StrictFunctionName"], {});
        }
    } else {
        if (isRestrictedWord(token.strvalue)) {
            firstRestricted = token;
            message = Messages["StrictFunctionName"];
        } else if (isStrictModeReservedWord(tokval)) {
            firstRestricted = token;
            message = Messages["StrictReservedWord"];
        }
    }

    tmp = parseParams(firstRestricted);
    params = tmp.params;
    defaults = tmp.defaults;
    stricted = tmp.stricted;
    firstRestricted = tmp.firstRestricted;

    if (tmp.message != u"") { //#TODO switch to hasMessage, 
        //# hasMessage being assigned to true on message assignment.
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
  DEBUGOUT("parseFunctionDecl"); return node;
}

//#partial
Node parseFunctionExpression() { DEBUGIN(" parseFunctionExpression()");
    TokenStruct token, firstRestricted, stricted;
    u16string message, tokval;
    Node body(false, true), id(false, true), node(true, true);    
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    bool previousStrict;
    
    
    id = NULLNODE;
    expectKeyword(u"function");
    
    if (!match(u"(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 
        tokval = token.strvalue;
        if (strict) {
            if (isRestrictedWord(tokval)) {
                throwErrorTolerant(token, Messages["StrictFunctionName"],{});
            }
        } else {
            if (isRestrictedWord(tokval)) {
                firstRestricted = token;
                message = Messages["StrictFunctionName"];
            } else if (isStrictModeReservedWord(tokval)) {
                firstRestricted = token;
                message = Messages["StrictReservedWord"];
            }
        }
    }
    
    tmp = parseParams(firstRestricted); 
    params = tmp.params;
    defaults = tmp.defaults; 
    stricted = tmp.stricted;
    firstRestricted = tmp.firstRestricted;

    if (tmp.message != u"") {
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
    
  DEBUGOUT("parseFuncExpr"); return node;
}

    // 14 Program

//#CLEAR
Node parseSourceElement() { DEBUGIN(" parseSourceElement()");
    u16string val;
    if (lookahead.type == Token["Keyword"]) {
        val = lookahead.strvalue;
        if (val == u"const" || val == u"let") {
          return DEBUGRET("", parseConstLetDeclaration(val));
        } else if (val == u"function") {
          return DEBUGRET("", parseFunctionDeclaration()); 
        } else {
            
          return DEBUGRET("", parseStatement());
        }
    }

    if (lookahead.type != Token["EOF"]) {
        
      return DEBUGRET("", parseStatement());
    }
    
  DEBUGOUT("parseSourceElement"); return NULLNODE;
}

//#partial
vector< Node > parseSourceElements() { DEBUGIN(" parseSourceElementS() ");
    Node sourceElement(false, true);
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive;

    firstRestricted.isNull = true;
    while (idx < length) {
        token = lookahead;
        if (token.type != Token["StringLiteral"]) {
            break;
        }

        sourceElement = parseSourceElement();
        sourceElements.push_back(sourceElement);
        //#todo make a function that accepts vector of nested finds
        //#so we can make tests like this more legible.
        if (strcmp(
                   json_object_get_string(json_require(
                                                       json_require(
                                                                    sourceElement.jv, 
                                                                    "expression"),
                                                       "type")
                                          ), 
                   toU8string(Syntax["Literal"]).data()) != 0) {         
            // this is not directive
            break;
        }
        directive = slice(sourceraw, token.start + 1, token.end - 1);
        if (directive == u"use strict") {
            strict = true;
            if (!(firstRestricted.isNull)) { 
                throwErrorTolerant(firstRestricted, Messages["StrictOctalLiteral"],{});
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
    
  DEBUGOUT("parseSourceElementS"); return sourceElements;
}

//#CLEAR
Node parseProgram() { DEBUGIN(" parseProgram()");
    Node node(false, true);
    vector< Node > body;

    skipComment();
    peek();
    node.lookavailInit();
    strict = false;
    body = parseSourceElements();
    node.finishProgram(body);
    
  DEBUGOUT("parseProgram"); return node;
}

//#CLEAR
void filterTokenLocation() { DEBUGIN(" filterTokenLocation()");
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

json_object* tokenize(u16string code, OptionsStruct options) { 
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

 
    peek();
 
    if (lookahead.type == Token["EOF"]) {
        json_put(outJson, "tokenlist", vec2jsonCallback<TokenRecord>(extra.tokenRecords, 
                                                         &TokenRecord::toJson));
        delete[] sourceraw;
      return outJson;
    }

    lex();
    while (lookahead.type != Token["EOF"]) {
        try {
            lex();
        } catch (...) { //! catch(LexError
            ExError e;
            e.description = u"LexError";
            if (extra.errorTolerant) {
                extra.errors.push_back(e); //!
                // We have to break on the first error
                // to avoid infinite loops.
                break;
            } else {
                throw e;
            }
        }
    }

    filterTokenLocation();
    json_put(outJson, "tokenlist", vec2jsonCallback<TokenRecord>(extra.tokenRecords, 
                                                    &TokenRecord::toJson)); 
    if (extra.commentTracking) {
        json_put(outJson, "comments", vec2jsonCallback<Comment>(extra.comments,
                                               &Comment::toJson));
    }
    if (extra.errorTolerant) {
        json_put(outJson, "errors",  vec2jsonCallback<ExError>(extra.errors,
                                             &ExError::toJson));
    }

  return outJson;
}


json_object*  tokenize(string code, OptionsStruct options) { 
  return tokenize(toU16string(code), options);
}
json_object*  tokenize(string code) { 
    OptionsStruct o;
  return tokenize(code, o);
}
json_object*  tokenize(u16string code) { 
    OptionsStruct o;
  return tokenize(code, o);
}

string tokenizeRetString(string code, OptionsStruct options) {
  return string(json_object_to_json_string_ext(tokenize(code, options), JSON_C_TO_STRING_PRETTY)); 
}
string tokenizeRetString(u16string code, OptionsStruct options) {
  return string(json_object_to_json_string_ext(tokenize(code, options), JSON_C_TO_STRING_PRETTY)); 
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

json_object*  parse(const u16string code, const OptionsStruct options) { 
        
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


    programNode = parseProgram();
    json_put(programJson, "program", programNode.jv);
    json_put(programJson, "regexp", programNode.regexPaths2json());


    if (extra.commentTracking) {
        json_put(programJson, "comments", vec2jsonCallback<Comment>(extra.comments,
                                                           &Comment::toJson)); //! do these .toJson funcs all exist?
    }

    if (extra.tokenTracking) {
        filterTokenLocation();
        json_put(programJson, "tokens", vec2jsonCallback<TokenRecord>(extra.tokenRecords,
                                                         &TokenRecord::toJson));
    }

    if (extra.errorTolerant) {
        json_put(programJson, "errors", vec2jsonCallback<ExError>(extra.errors,
                                                         &ExError::toJson));
    }


    extra = ExtraStruct();

  return programJson;
}

json_object*  parse(const string code, const OptionsStruct options) {
    
  return parse(toU16string(code), options);
}
json_object*  parse(const string code) { 
    OptionsStruct o;
  return parse(code, o);
}
json_object*  parse(const u16string code) { 
    OptionsStruct o;
  return parse(code, o);
}

//# return json as string.
string parseRetString(const u16string code, const OptionsStruct options) { 
  return json_object_to_json_string_ext(parse(code, options), JSON_C_TO_STRING_PLAIN); 
}
string parseRetString(const string code, const OptionsStruct options) { 
    json_object * m = parse(code, options);
    string result = json_object_to_json_string_ext(m, JSON_C_TO_STRING_PLAIN); 
    cout << "put: " << json_object_put(m) << endl;
    return result;
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
    char* parseExtern(const char *code, const char* options) {

        //printf("code:%s\n", code);
      return strToChar(parseRetString(string(code), 
                                       OptionsStruct(
                                         options)));
    }
}


/*int main() {
    string somecode = "var f = function() { echo('hello world'); }";

    string someopt = "{ 'loc': false }";
    string result = string(parseExtern(somecode.data(), someopt.data()));
    result.append("\n");
    printf("%s", result.data());
}*/




