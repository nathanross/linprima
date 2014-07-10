#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <regex>
#include <functional>!!
#include <unordered_set>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>
//algorithm is for find(vector.begin()
using namespace std;

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
template<typename T> bool has(const T needle, const unordered_set<T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}
template<typename T> bool hasStringKey(const string needle, const map<string,T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}

template <typename T>
Json::Value vec2jsonCallback(vector<T> in, 
                             function<Json::Value(T&)> const& f) {
    Json::Value root = Json::arrayValue;
    for (int i=0; i<in.size(); i++) {
        root[i] = f(in[i]);
    }
    return root;
} 

template <typename T>
Json::Value vec2json(vector<T> in) {
    Json::Value root = Json::arrayValue;
    for (int i=0; i<in.size(); i++) {
        root[i] = in[i];
    }
    return root;
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

u16string res_u16(void* pos) {
    return *((u16string *) pos);
}

u16string slice(char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
    char16_t * startptr = arr + start;
    int size = sizeof(startptr[0]); //in case need to generalize this func.
    return u16string(startptr, (end-start)*size);
}

void append(u16string &base, char16_t tail) {
    base.append(u16string({tail})); //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.
}

template<typename T> 
Json::Value vec2json(vector<T> in) {
    Json::Value arr = Json::arrayValue;
    for (int i=0; i<in.size(); i++) {
        arr[i] = in[i];
    }
    return arr;
}

//void throwError(vector<string> someMsg, string errType, string otherMsg) { //!!!
//    return;
//}

int parseInt(u16string in, int radix) {  // !!!
    return 5; 
}

const char16_t NULL_CHAR16 = u'X';

//---- ----------  -----------------------------
// signatures (temporary until we set up a header file):

void throwError(TokenStruct token, u16string messageFormat, string otherMsg);
void throwErrorTolerant(TokenStruct token, u16string messageFormat, string otherMsg);
void throwUnexpected(TokenStruct token);

// -----------------------



struct RegexHalf {
    u16string value;
    u16string literal;
    int start;
    int end;
    RegexHalf() { start = -1; end = -1; }
}

struct Position {
    int line;
    int column;
    Position() {
        line = lineNumber;
        column = idx - lineStart;
    }
}

struct Loc { 
    //aka SourceLocation
    Position start;
    Position end;
    
    Loc() {
        this->end->line = -1;
        this->end->column = -1;
    }
};

struct Comment {
    u16string type;
    u16string value;
    int range[2];
    int rangeEnd;
    Loc loc;
    Comment() {
        this->type = -1;
        this->value = u"";
        this->rangeStart = -1;
        this->rangeEnd = -1;
    }
};

//# called ExError to prevent forseeable 
//# exception-handling namespace conflict.
struct ExError {
};

struct RegexHalf {
    u16string value;
    u16string literal;
}

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
        isNull = false;
        type = -1;
        value = nullptr;
        lineNumber = -1;
        lineStart = -1;
        startLineNumber = -1;
        startLineStart = -1;
        start = -1;
        end = -1;
        octal=false; 
        range[0] = -1;
        range[1] = -1;

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
        range[0] = -1;
        range[1] = -1;
    }
}


struct Argument {
    u16string type;
}

struct Expr {
    u16string oper;
    u16string name;
    Argument argument;
}

TokenStruct NULLTOKEN;
NULLTOKEN.isNull = true;

struct ExtraStruct {
    bool tokenTracking; //port-specific member to replace "if (extra.tokens)"
    vector<TokenRecord> tokenRecords; //called extra.tokens in esprima
    // name changed here to distinguish specific type and different domains
    // of these types.

    bool hasSource;
    String source; 

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
    vector<> bottomRightStack;

    Extra() {
        tokenize = false;
        errorTolerant = false;
        attachComment = false;
        tokenRecords.clear();
        comments.clear();
        errors.clear();
        leadingComments.clear();
        trailingComments.clear();
        
    }

    //auto source; //! see wrappingNode.finish()
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
        range = false;
        loc = false;
        comment = false;
        tolerant = false;
        attachComment = false;
        tokens = false;
        hasSource = false;
    }
    OptionsStruct(Json::Value in) {
        range = in.isMember("range") && in["range"].asBool();
        loc = in.isMember("loc") && in["loc"].asBool();
        attachComment = in.isMember("attachComment") && in["attachComment"].asBool();
        comment = in.isMember("comment") && in["comment"].asBool();
        tolerant = in.isMember("tolerant") && in["tolerant"].asBool();
        tokens = in.isMember("tokens") && in["tokens"].asBool();
        hasSource = in.isMember("source");
        if (hasSource) {
            source = in["source!!"].asString();
        }
    }
};

//! expr type (see isLeftHandSide)


OptionStruct options;
ExtraStruct extra;
StateStruct state;
TokenStruct lookahead; //! todo somewhere lookahead.value is treated a straight u16string value instead of token.
int idx;
int lineNumber;
int lineStart;
int length;
bool strict = false; //! remove initialization

char16_t * sourceraw;
char16_t source(int idx) { return *(sourceraw + idx); }

map<string, int> LiteralType = {
    {"String", 1},
    {"Int", 2},
    {"Double", 3},
    {"Regexp", 4},
    {"Bool", 5},
    {"null", 6}
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


vector< u16string > FnExprTokens = {
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

// PlaceHolders { Arrow { type } } //!!
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

 // Ensure the condition is true, otherwise throw an error.
 // This is only to have a better contract semantic, i.e. another safety net
 // to catch a logic error. The condition shall be fulfilled in normal case.
 // Do NOT use this to enforce a certain condition on any user input.

void assert(bool condition, string message) { 
     /* istanbul ignore if */
    if (!condition) { //! does this function sometimes get 
        //called with first arg undefined? if so this is gonna be problem.
        throw std::runtime_error(message);
    }
}

bool isDecimalDigit(const char16_t ch) {
    return (ch >= 0x30 && ch <= 0x39); //0..9
}

bool isHexDigit(const char16_t ch) {
    return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) != std::string::npos);    
}

bool isOctalDigit(const char16_t ch) {
    return (u16string({u"01234567"}).find_first_of(ch) != std::string::npos);    
}

char16_t toLowercaseHex(const char16_t ch) { //used in scanHexEscape
    //assumes isHexDigit(ch) evals to true
    u16string hexletters = u"abcdefABCDEF";
    int pos = hexletters.find_first_of(ch);
    if (pos != std::string::npos && pos >= 6) {
        pos = pos - 6;
        ch = hexletters[pos];
    }
    return ch;
}

//7.2 White Space
bool isWhiteSpace(const char16_t ch) {
    return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || (ch >= 0x1680 && has<int>(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators

bool isLineTerminator(const char16_t ch) {
    return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}

// 7.6 Identifier Names and Identifiers

bool isIdentifierStart(const char16_t ch) {
    smatch m;
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierStart"]));
} 

bool isIdentifierPart(const char16_t ch) {
    smatch m;
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch >= 0x30 && ch <= 0x39) ||         // 0..9
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierPart"]));
}

// 7.6.1.2 Future Reserved Words

bool isFutureReservedWord(const u16string id) {
    return has<u16string>(id, { //
            u"class",
            u"enum",
            u"export",
            u"extends",
            u"import",
            u"super"
            });
}

bool isStrictModeReservedWord(const u16string id) {
    return has<u16string>(id, { 
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

bool isRestrictedWord(const u16string id) {
    return (id == u"eval" || id == u"arguments");
}

// 7.6.1.1 Keywords
bool isKeyword(const u16string id) {
    if (strict && isStrictModeReservedWord(id)) { //! ? where is strict stored??
        return true;
    }

    // 'const' is specialized as Keyword in V8.
    // 'yield' and 'let' are for compatiblity with SpiderMonkey and ES.next.
    // Some others are from future reserved words.

    switch (id.length()) {
        case 2:
            return (id == u"if") || (id == u"in") || (id == u"do");
        case 3:
            return has<u16string>(id, { u"var", u"for", u"new", u"try", u"let"});
        case 4:
            return has<u16string>(id, {u"this", u"else", u"case", u"void",
                        u"with", u"enum"});
        case 5:
            return has<u16string>(id, {u"while", u"break", u"catch",
                        u"throw", u"const", u"yield", u"class", u"super"});
        case 6:
            return has<u16string>(id, {u"return", u"typeof", u"delete",
                        u"switch", u"export", u"import"});
        case 7:
            return (id == u"default") || (id == u"finally") || (id == u"extends");
        case 8:
            return (id == u"function") || (id == u"continue") || (id == u"debugger");
        case 10:
            return (id == u"instanceof");
        default:
            return false;
    }
}

// 7.4 Comments

//#CLEAR+
//# only called if extra.commentTracking
void addComment(u16string type, u16string value, int start, int end, Loc loc) {
    Comment comment;

    //assert(typeof start === 'number', 'Comment must have valid position');

    // Because the way the actual token is scanned, often the comments
    // (if any) are skipped twice during the lexical analysis.
    // Thus, we need to skip adding a comment if the comment array already
    // handled it.
    if (state.lastCommentStart >= start) {
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
}

u16string dummy(char16_t in){ 
    if (in > 0x90) {
        return u"hi";
    } else {
        return u"bai";
    }
}


int main() {
    u16string banana = dummy(u'g');
    std::cout << toU8string(banana) << std::endl;
    return 0;
}

//#CLEAR+
void skipSingleLineComment(const int offset) {
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
            return;
        }
    }

    if (extra.commentTracking) {
        comment = slice(sourceraw, start + offset, idx); 
        loc.end.line = lineNumber;
        loc.end.column = idx - lineStart;
        addComment(u"Line", comment, start, idx, loc);
    }
}

void skipSingleLineComment() { //? are we sure that in javascript the calls to this with no arg will default to 0?
    skipSingleLineComment(0);
}

//#CLEAR+
void skipMultiLineComment() {
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
                throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
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
                return;
            }
            ++idx;
        } else {
            ++idx;
        }
    }

    throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
}

//#CLEAR+
void skipComment() {
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
}

//#CLEAR+
char16_t scanHexEscape(const char16_t prefix) {
    int i, len;
    char16_t ch;
    int code = 0;

    len = (prefix == u'u') ? 4 : 2;
    for (i = 0; i < len; ++i) {
        if (idx < length && isHexDigit(source(idx))) {
            ch = source(idx++);
            code = code * 16 + u16string({u"0123456789abcdef"}).find_first_of(toLowercaseHex(ch));
        } else {
            return NULL_CHAR16; 
        }
    }
    return code;
}

u16string scanUnicodeCodePointEscape() {
    char16_t ch;
    int code;
    char16_t cu[2];

    ch = source(idx);
    code = 0;

    // At least, one hex digit is required.
    if (ch == u'}') {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }

    while (idx < length) {
        ch = source(idx++);
        if (!isHexDigit(ch)) {
            break;
        }
        code = code * 16 + u16string({u"0123456789abcdef"}).find_first_of(toLowercaseHex(ch));
    }

    if (code > 0x10FFFF || ch != u'}') {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }

    // UTF-16 Encoding
    if (code <= 0xFFFF) {
        return u16string({(char16_t) code});
    }

    cu[0] = ((code - 0x10000) >> 10) + 0xD800; 
    cu[1] = ((code - 0x10000) & 1023) + 0xDC00;
    return u16string(cu);
}

u16string getEscapedIdentifier() {
    char16_t ch;
    u16string id;

    ch = source(idx++);
    id = u16string({ch});

    // '\u' (U+005C, U+0075) denotes an escaped character.
    if (ch == 0x5C) {
        if (source(idx) != 0x75) {
            throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (!ch || ch == u'\\' || !isIdentifierStart(ch)) { //!! what does !ch mean in this context?
            throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
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
                throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (!ch || ch == u'\\' || !isIdentifierPart(ch)) {
                throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
            }
            append(id, ch);
        }
    }

    return id;
}

u16string getIdentifier() {
    int start;
    char16_t ch;

    start = idx++;
    while (idx < length) {
        ch = source(idx);
        if (ch == 0x5C) {
            // Blackslash (U+005C) marks Unicode escape sequence.
            idx = start;
            return getEscapedIdentifier();
        }
        if (isIdentifierPart(ch)) {
            ++idx;
        } else {
            break;
        }
    }

    return slice(sourceraw, start, idx); 
}

TokenStruct scanIdentifier() {
    TokenStruct t;
    int start, type;
    u16string id;

    start = idx;

    // Backslash (U+005C) starts an escaped character.
    id = (source(idx) == 0x5C) ? getEscapedIdentifier() : getIdentifier();

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
    t.value = new u16string(id);
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    return t;
}

// 7.7 Punctuators

TokenStruct scanPunctuator() {
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
        t.value = new u16string({ code[0] });
        t.end = idx;
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
                t.value = new u16string(code);
                t.end = idx;
                return t;
            case 0x21: // !
            case 0x3D: // =
                idx += 2;

                // !== and ===
                if (source(idx) == 0x3D) {
                    ++idx;
                }
                t.value = new u16string(slice(sourceraw, start, idx));
                t.end = idx;
                return t;
            }
        }
    }

    // 4-character punctuator: >>>=

    ch4 = slice(sourceraw, idx, idx+4); 

    if (ch4 == u">>>=") {
        idx += 4;
        t.value = new u16string(ch4);
        t.end = idx;
        return t;
    }

    // 3-character punctuators: === !== >>> <<= >>=

    ch3 = ch4.substr(0, 3); 

    if (ch3 == u">>>" || ch3 == u"<<=" || ch3 == u">>=") {
        idx += 3;
        t.value = new u16string(ch3);
        t.end = idx;
        return t;
    }

    // Other 2-character punctuators: ++ -- << >> && ||
    ch2 = ch3.substr(0, 2); 

    if ((ch1 == ch2[1] && (u16string({u"+-<>&|"}).find_first_of(ch1) != std::string::npos)) || ch2 == u"=>") {
        idx += 2;
        t.value = new u16string(ch2);
        t.end = idx;
        return t;
    }

    // 1-character punctuators: < > = ! + - * % & | ^ /

    if (u16string({u"<>=!+-*%&|^/"}).find_first_of(ch1) != std::string::npos) {
        ++idx;
        t.value = new u16string({ch1});
        t.end = idx;
        return t;
    }

    throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    return t; //?! returning this empty on error is not in source behavior. need to make sure it's handled gracefully.
}
    // 7.8.3 Numeric Literals

TokenStruct scanHexLiteral(const int start) {
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }
   
    t.type = Token["NumericLiteral"];
    val = u"0x";
    val.append(number);
    t.value = new int(parseInt(val, 16));
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    return t;
}

TokenStruct scanOctalLiteral(const int start) {
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }

    t.type = Token["NumericLiteral"];
    t.value = new int(parseInt(number, 8));
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    return t;
}


TokenStruct scanNumericLiteral() {
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
                return scanHexLiteral(start);
            }
            if (isOctalDigit(ch)) {
                return scanOctalLiteral(start);
            }
            // decimal number starts with '0' such as '09' is illegal.
            if (ch && isDecimalDigit(ch)) {
                throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL"); 
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
            throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
        }
    }

    if (isIdentifierStart(source(idx))) {
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }
    
    t.type = Token["NumericLiteral"];
    t.value = new double(stod(toU8string(number)));
    t.lineNumber = lineNumber;
    t.lineStart = lineStart;
    t.start = start;
    t.end = idx;
    
    return t; 
}

// 7.8.4 String Literals

TokenStruct scanStringLiteral() {
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
                            code = code * 8 + u16string({u"01234567"}).find_first_of(source(idx++));

                            // 3 digits are only allowed when string starts
                            // with 0, 1, 2, 3
                            if (u16string({u"0123"}).find_first_of(ch) != std::string::npos &&
                                    idx < length &&
                                    isOctalDigit(source(idx))) {
                                code = code * 8 + u16string({u"01234567"}).find_first_of(source(idx++));
                            }
                        }
                        append(str, code);
                    } else {
                        str += ch;
                    }
                    break;
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
        throwError(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
    }
    
    t.type = Token["StringLiteral"];
    t.value = new u16string(str);
    t.octal = octal;
    t.startLineNumber = startLineNumber;
    t.startLineStart = startLineStart;
    t.lineNumber = lineNumber;
    t.lineStart = lineStart,
    t.start = start;
    t.end = idx;
    return t;
}

RegexHalf scanRegExpBody() {
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexHalf rh;

    ch = source(idx);
    assert(ch == u'/', 'Regular expression literal must start with a slash');
    append(str, source(idx++);

    classMarker = false;
    terminated = false;
    while (idx < length) {
        ch = source(idx++);
        append(str, ch);
        if (ch == u'\\') {
            ch = source(idx++);
            // ECMA-262 7.8.5
            if (isLineTerminator(ch)) {
                throwError(NULLTOKEN, Messages["UnterminatedRegExp"], "");
            }
            append(str, ch);
        } else if (isLineTerminator(ch)) {
            throwError(NULLTOKEN, Messages["UnterminatedRegExp"], "");
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
        throwError(NULLTOKEN, Messages["UnterminatedRegExp"], "");
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length - 2);
    rh.value = body;
    rh.literal = str;
    return rh;
}

RegexHalf scanRegExpFlags() {
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
                    for (str.append(u"\\u")); restore < idx; ++restore) { 
                    //? note that we replaced u'\\u' with u"\\u" here and right below.
                        append(str, source(restore));
                    }
                } else {
                    idx = restore;
                    append(flags, u'u');
                    str.append(u"\\u");
                }
                throwErrorTolerant(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
            } else {
                append(str, u'\\');
                throwErrorTolerant(NULLTOKEN, Messages["UnexpectedToken"], "ILLEGAL");
            }
        } else {
            append(flags, ch);
            append(str, ch);
        }
    }
    
    rh.value = flags;
    rh.literal = str;
    return rh;
}

TokenStruct scanRegExp() {
    int start;
    RegexHalf body; 
    RegexHalf flags; 
    int value; 
    TokenStruct t;
    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = null;
    skipComment();
    start = idx;

    body = scanRegExpBody();
    flags = scanRegExpFlags();
    //value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t.type = Token["RegularExpression"];
        t.value = value;
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = start;
        t.end = idx;
        return t; //not polymorphic right now. not going to work... :!
    }

    t.literal = body.literal; 
    t.literal.append(flags.literal);
    t.value = body.value;
    t.flags = flags.value;
    t.start = start;
    t.end = idx;
    return t;
}

//#PARTIAL
TokenStruct collectRegex() {
    int pos;
    Loc loc;
    RegexHalf regex;
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
                    extra.tokenRecords.pop(); //! is it pop or pop back? also search/change other instances of pop() accordingly.
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

    return regex;
}

//#CLEAR
bool isIdentifierName(token) {
    return has<int>(token.type, { Token["Identifier"], Token["Keyword"],
                Token["BooleanLiteral"], Token["NullLiteral"]});
}

//#PARTIAL
TokenStruct advanceSlash() {
    //# only gets called if extra.tokenize == true

    TokenRecord prevToken, checkToken;
    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    if (extra.tokenRecords.size() == 0) {
        // Nothing before that: it cannot be a division.
        return collectRegex(); //...
    }    
    prevToken = extra.tokenRecords[extra.tokenRecords.length - 1];

    if (prevToken.typestring == u"Punctuator") { 
        if (prevToken.strvalue == u"]") { //!
            return scanPunctuator();
        }
        if (prevToken.valuestring == u")" && extra.openParenToken > 0) { 
            checkToken = extra.tokenRecords[extra.openParenToken - 1];
            if (//checkToken && //# instead of checking for existence, we 
                //# add the openParenToken value check to the condition above.
                //# remember exta.tokens() is already size > 0 bcos check at top of func.
                checkToken.typestring == u"Keyword" && 
                has<u16string>(checkToken.valuestring, {u"if", u"while", u"for", u"with"})) {
                return collectRegex(); //? returns tokenstruct?
            }
            return scanPunctuator();
        }
        if (prevToken.valuestring == u"}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokenRecords.size() >= ((extra.openCurlyToken -3) +1) &&
                extra.tokenRecords[extra.openCurlyToken - 3].typestring == u"Keyword") { 
                // Anonymous function.
                //- checkToken = extra.tokenRecords[extra.openCurlyToken - 4];
                //- if (!checkToken) {
                if (if extra.openCurlyToken > 3) {
                    checkToken = extra.tokenRecords[extra.openCurlyToken -4];
                } else { 
                    return scanPunctuator();
                }
            } else if (extra.openCurlyToken >= 4 &&
                extra.tokenRecords.size() >= ((extra.openCurlyToken -4) +1) &&
                    extra.tokenRecords[extra.openCurlyToken - 4].typestring == u"Keyword") {
                // again previously had checked type against string in this cond.
                // Named function.
                if (if extra.openCurlyToken > 4) {
                    checkToken = extra.tokenRecords[extra.openCurlyToken -5];
                } else { 
                    return collectRegex();
                }
            } else {
                return scanPunctuator();
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (FnExprTokens.idxOf(checkToken.value) >= 0) { //!
                // It is an expression.
                return scanPunctuator();
            }
            // It is a declaration.
            return collectRegex(); 
        }
        return collectRegex();
    }
    if (prevToken.typestring == u"Keyword") { //! not checking type against string whaat.
        return collectRegex(); 
    }
    return scanPunctuator();
}

//#CLEAR
TokenStruct advance() {
    char16_t ch;
    TokenStruct t;

    skipComment();

    if (idx >= length) {
        t.type = Token["EOF"];
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = idx;
        t.end = idx;
        return t;
    }

    ch = source(idx);

    if (isIdentifierStart(ch)) {
        return scanIdentifier();
    }

    // Very common: ( and ) and ;
    if (ch == 0x28 || ch == 0x29 || ch == 0x3B) {
        return scanPunctuator();
    }

    // String literal starts with single quote (U+0027) or double quote (U+0022).
    if (ch == 0x27 || ch == 0x22) {
        return scanStringLiteral();
    }


    // Dot (.) U+002E can also start a floating-point number, hence the need
    // to check the next character.
    if (ch == 0x2E) {
        if (isDecimalDigit(source(idx + 1))) {
            return scanNumericLiteral();
        }
        return scanPunctuator();
    }

    if (isDecimalDigit(ch)) {
        return scanNumericLiteral();
    }

    // Slash (/) U+002F can also start a regex.
    if (extra.tokenize && ch === 0x2F) {
        return advanceSlash();
    }

    return scanPunctuator();
}

//#CLEAR
TokenStruct collectToken() {
    Loc loc;
    TokenStruct token;
    TokenRecord tr;
    u16string value;

    skipComment();
    loc.start.line = lineNumber;
    loc.start.column = idx - ineStart;

    token = advance();
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (token.type != Token["EOF"]) { //this didn't check against string. is fine.
        tr.valuestring = slice(sourceraw, token.start, token.end);
        tr.typestring = TokenName[token.type];
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

    return token;
}

//#CLEAR
function lex() {
    TokenStruct token;

    token = lookahead;
    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    lookahead = (tokenTracking) ? collectToken() : advance(); 

    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    return token;
}

//#CLEAR
void peek() {
    int pos, line, start;
    pos = idx;
    line = lineNumber;
    start = lineStart;
    lookahead = (tokenTracking) ? collectToken() : advance(); 
    idx = pos;
    lineNumber = line;
    lineStart = start;
}

//# Position and SourceLocation are defined as structs near the top.

//#CLEAR 
Json::Value posToJson(Position p) {
    Json::Value root;
    root["line"] = p.line;
    root["column"] = p.column;
}

Json::Value locToJson(Loc l) {
    Json::Value root;
    root["start"] = posToJson(l.start); 
    if (l.end.line != -1) {
        root["end"] = posToJson(l.end); 
    }
    return root;
}

//#CLEAR
//Loc 


class Node {
public:
    Json::Value jv;
    Loc loc;
    bool isNull;
    NodeFinish nf;
    bool hasRange;
    vector<Comment> trailingComments;
    vector<Comment> leadingComments;
    vector< vector<string> > regexPaths; //lin only. obv.

    u16string type;
    u16string name;
    
    //#CLEAR
    Node() { 
        hasRange = false;

        jv["loc"] = locToJson(loc);
        isNull = false;

        idx = lookahead.start;
        if (lookahead.type == Token["StringLiteral"]) {
            lineNumber = lookahead.startLineNumber;
            lineStart = lookahead.startLineStart;
        } else {
            lineNumber = lookahead.lineNumber;
            lineStart = lookahead.lineStart;
        }
        if (extra.range) {
            if (!hasRange) jv["range"] = Json::arrayValue;
            jv["range"][0] = idx;
            jv["range"][1] = 0;
        }
    }
    
    Json::Value toJson() {
        return this->jv;
    }
    
    void reg(String path, Node &child) {
        if (child.isNull) { jv[path] = Json::Value.null; }

        if (child.regexPaths.size() == 0) { return; }
        if (child.regexPaths[0][0] == ".") {
            regexPaths.push_back({index});
        }
        for (int i=0; i<child.regexPaths.size(); i++) {
            regexPaths.push_back(child.regexPaths[i]);
            regexPaths.back().push_back(index);
        }
        child.regexPaths.clear();

        jv[path] = child.jv;
    }

    Json::Value nodeVec(String path, vector< Node > & nodes) {
        jv[path] = Json::arrayValue;
        Json::Value * rootptr = &(jv[path]);

        for (int i=0; i<nodes.size(); i++) {
            if (nodes[i].isNull) {
                rootptr->[i] = Json::Value.null; //! probably not correct way to access this value
            } else {
                reg(nodes[i]);
                rootptr->[i] = nodes[i].jv;
            }
        } 
    }
    void addType(String in) {
        type = Syntax[in];
        jv["type"] = s(type);
    }
    Json::value regexPaths2json() {
        Json::value root = Json::arrayValue;
        for (int i=0; i<regexPaths.size(); i++) {
            root[i] = Json::arrayValue;
            for (int j=0; j<regexPaths[i].size(); j++) {
                root[i][j] = regexPaths[i].back();
                regexPaths[i].pop_back();
            }
        }
        return root;
    }
    
    void trailingCommentsIntoJson(const bool leading) {
        string key;
        vector<Comment> * commentVec;
        if (leading) {
            key = "leadingComments";
            commentVec = &leadingComments;
        } else {
            key = "trailingComments";
            commentVec = &trailingComments;
        }
        if (commentVec.size() > 0) {
            jv[key] = Json::arrayValue;
            //! do this section. vec<Comment>

        } else {
            jv.removeKey(key);
        }
    }
    
    //#PARTIAL
    void processComment() {
        //# assumes attachComments 
        //# so that means range is already true.

        vector<Comment> trailingComments;
                
        vector< Node * > * bottomRight = &(extra.bottomRightStack);
        Node * lastChild,
            * last = bottomRight[bottomRight.length - 1];
        bool LEADING = true;


        if (jv["type"] == s(Syntax["Program"])) {  
            if (this.body.length > 0) { //!
                return;
            }
        }

        if (extra.trailingComments.size() > 0) {
            if (extra.trailingComments[0].range[0] >= jv["range"][1].asInt()) {
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
                last->trailingComments[0].range[0] >= jv["range"][1].asInt()) {
                trailingComments = last->trailingComments;
                last->trailingComments.clear();
                last->commentsIntoJson(! LEADING);
                //delete last.trailingComments; 
                //? maybe have a boolean to say no trailing comments? length will prob. be workable.
            }
        }

        // Eating the stack.
        if (!(last->isNull)) {
            while ((!(last->isNull)) && last->jv["range"][0] >= jv["range"][0].asInt()) {
                lastChild = last;
                last = bottomRight->pop();
            }
        }

        if (!(lastChild->isNull)) { 
            if (lastChild->leadingComments.size() > 0 &&
                lastChild->leadingComments[lastChild->leadingComments.size() - 1].range[1] <= jv["range"][0].asInt()) {
                this->leadingComments = lastChild->leadingComments;
                lastChild->leadingComments.clear();
                lastChild->commentsIntoJson(LEADING);
                this->commentsIntoJson(LEADING);
            }
        } else if (extra.leadingComments.size() > 0 && 
                   extra.leadingComments[extra.leadingComments.size() - 1].range[1] <= jv["range"][0].asInt()) {
            this->leadingComments = extra.leadingComments;
            extra.leadingComments.clear();
            this->commentsIntoJson(LEADING);
        }

        if (trailingComments.size() > 0) {
            this.trailingComments = trailingComments;
            this->commentsIntoJson(! LEADING);
        }

        bottomRight->push_back(this);
    }

    //#CLEAR
    void finish() {
        if (extra.range) {
            jv["range"][1] = idx; 
        }
        if (extra.loc) {
            Position newpos;
            jv["loc"]["end"] = posToJson(newpos);
            if (extra.hasSource) { 
                jv["loc"]["source"] = extra.source; 
            }
        }

        if (extra.attachComment) {
            this->processComment();
        }
    }

    //#partial
    void finishArrayExpression(vector< Node >& elements) {
        addType("ArrayExpression");
        nodeVec("elements", elements);
        this->finish();
    }

    void s(u16string in) {
        return toU8string(in);
    }

    void finishArrowFunctionExpression(params, defaults, body, expression) {
        addType("ArrowFunctionExpression");
        jv["id"] = null;
        jv["params"] = params;
        jv["defaults"] = defaults;
        jv["body"] = body;
        jv["rest"] = null;
        jv["generator"] = false;
        jv["expression"] = expression;
        this->finish();
    }

    void finishAssignmentExpression(u16string oper, TokenStruct left, Node& right) {
        addType("AssignmentExpression");
        jv["operator"] = s(oper);
        jv["left"] = s(left.strvalue);
        reg("right", right);
        this->finish();
    }

    void finishBinaryExpression(u16string oper, left, right) {
        addType((oper == u"||" || oper == u"&&") ? "LogicalExpression" : "BinaryExpression");
        jv["operator"] = s(oper);
        jv["left"] = left; //!
        jv["right"] = right; //!
        this->finish();
    }

    //#CLEAR
    void finishBlockStatement(vector< Node >& body) {
        addType("BlockStatement");
        nodeVec("body", body);
        this->finish();
    }

    //#CLEAR
    void finishBreakStatement(Node& label) {
        addType("BreakStatement");
        reg("label", label);
        this->finish();
    }

    //#CLEAR
    void finishCallExpression(Node& callee, Vector< Node >& args) {
        addType("CallExpression");
        reg("callee", callee);
        nodeVec("arguments", args);
        this->finish();
    }

    //#CLEAR
    void finishCatchClause(Node& param, Node& body) {
        addType("CatchClause");
        reg("param", param);
        reg("body", body);
        this->finish();
    }

    //#CLEAR
    void finishConditionalExpression(Node& test, Node& consequent, Node& alternate) {
        addType("ConditionalExpression");
        reg("test", test);
        reg("consequent", consequent);
        reg("alternate", alternate);
        this->finish();
    }

    //#CLEAR
    void finishContinueStatement(Node& label) {
        addType("ContinueStatement");
        reg("label", label);
        this->finish();
    }

    //#CLEAR
    void finishDebuggerStatement() {
        addType("DebuggerStatement");
        this->finish();
    }

    //#CLEAR
    void finishDoWhileStatement(Node& body, Node& test) {
        addType("DoWhileStatement");
        reg("body", body);
        reg("test", test);
        this->finish();
    }

    //#CLEAR
    void finishEmptyStatement() {
        addType("EmptyStatement");
        this->finish();
    }

    //#CLEAR
    void finishExpressionStatement(Node expression) {
        addType("ExpressionStatement");
        reg("expression", expression);
        this->finish();
    }

    //#CLEAR
    void finishForStatement(Node& init, Node& test, Node& update, Node& body) {
        addType("ForStatement");
        reg("init", init);
        reg("test", test);
        reg("update", update);
        reg("body", body);
        this->finish();
    }

    //#CLEAR
    void finishForInStatement(left, right, body) {
        addType("ForInStatement");
        reg("left", left);
        reg("right", right);
        reg("body", body);
        jv["each"] = false;
        this->finish();
    }

    void finishFunctionDeclaration(id, params, defaults, body) {
        addType("FunctionDeclaration");
        jv["id"] = id;
        jv["params"] = params;
        jv["defaults"] = defaults;
        jv["body"] = body;
        jv["rest"] = null;
        jv["generator"] = false;
        jv["expression"] = false;
        this->finish();
    }

    void finishFunctionExpression(id, params, defaults, Node body) {
        reg(body);
        addType("FunctionExpression");
        jv["id"] = id;
        jv["params"] = params;
        jv["defaults"] = defaults;
        jv["body"] = body;
        jv["rest"] = null;
        jv["generator"] = false;
        jv["expression"] = false;
        this->finish();
    }

    //#CLEAR
    void finishIdentifier(u16string name) {
        addType("Identifier");
        this->name = name;
        jv["name"] = s(name);
        this->finish();
    }

    //#CLEAR
    void finishIfStatement(Node& test, Node& consequent, Node& alternate) { 
        addType("IfStatement");
        reg("test", test);
        reg("consequent", consequent);
        reg("alternate", alternate);
        this->finish();
    }

    //#CLEAR
    void finishLabeledStatement(Node label, Node body) {
        addType("LabeledStatement");
        reg("label", label);
        reg("body", body);
        this->finish();
    }

    void finishLiteral(TokenStruct token) {
        addType("Literal"); //#c
        //#!todo pass kind of literal through in extra node member.
        if (token.literaltype == LiteralType["String"]) {
            jv["value"] = token.strvalue;
        } else if (token.literaltype == LiteralType["Int"]) {
            jv["value"] = token.intvalue;
        } else if (token.literaltype == LiteralType["Double"]) {
            jv["value"] = token.dblvalue;
        } else if (token.literaltype == LiteralType["Regexp"]) {
            jv["value"] = *((int*) token.value); //!!! 
            regexPaths.push(".");
        }
        this->jv["raw"] = slice(sourceraw, token.start, token.end); //#c
        this->finish();
    }

    //#CLEAR
    void finishMemberExpression(char16_t accessor, Node& object, Node& property) {
        addType("MemberExpression");
        jv["computed"] = (accessor == u'[');
        reg("object", object);
        reg("property", property);
        this->finish();
    }

    void finishNewExpression(Node& callee, vector<Node>& args) {
        addType("NewExpression");
        reg("callee", callee);
        nodeVec("arguments", args);
        this->finish();
    }

    void finishObjectExpression(properties) {
        addType("ObjectExpression");
        jv["properties"] = properties;
        this->finish();
    }

    //#CLEARto
    void finishPostfixExpression(u16string oper, Node& argument) {
        addType("UpdateExpression");
        jv["oper"] = s(oper);
        reg("argument", argument);
        jv["prefix"] = false;
        this->finish();
    }

    void finishProgram(body) {
        addType("Program");
        jv["body"] = body;
        this->finish();
    }

    //#CLEAR
    void finishProperty(u16string kind, Node& key, Node& value) {
        addType("Property");
        reg("key", key);
        reg("value", value);
        jv["kind"] = s(kind);
        this->finish();
    }

    //#CLEAR
    void finishReturnStatement(Node& argument) {
        addType("ReturnStatement");
        reg("argument", argument);
        this->finish();
    }

    void finishSequenceExpression(expressions) {
        addType("SequenceExpression");
        jv["expressions"] = expressions;
        this->finish();
    }

    //#CLEAR
    void finishSwitchCase(Node& test, vector< Node >& consequent) {
        addType("SwitchCase");
        reg("test", test);
        nodeVec("consequent", consequent); //#todo switch nodeVec to be like reg, nodeVec("consequent", consequent)
        this->finish();
    }

    //#CLEAR
    void finishSwitchStatement(Node& discriminant, vector < Node >& cases) {
        addType("SwitchStatement");
        reg("discriminant", discriminant);
        nodeVec("cases", cases);
        this->finish();
    }

    void finishThisExpression() {
        addType("ThisExpression");
        this->finish();
    }

    //#CLEAR
    void finishThrowStatement(Node& argument) {
        addType("ThrowStatement");
        reg("argument", argument);
        this->finish();
    }

    //#CLEAR
    void finishTryStatement(Node& block, vector<Node>& guardedHandlers, 
                            vector<Node>& handlers, Node& finalizer) {
        addType("TryStatement");
        reg("block", block);
        nodeVec("guardedHandlers", guradedHandlers);
        nodeVec("handlers", handlers);
        reg("finalizer", finalizer);
        this->finish();
    }

    //#CLEAR
    void finishUnaryExpression(u16string oper, Node& argument) {
        addType((oper == u"++" || oper == u"--") ? "UpdateExpression" : "UnaryExpression");
        jv["operator"] = s(oper);
        reg("argument", argument);
        jv["prefix"] = true;
        this->finish();
    }

    //#CLEAR
    void finishVariableDeclaration(List< Node >& declarations, u16string kind) {
        addType("VariableDeclaration");
        nodeVec("declarations", declarations);
        jv["kind"] = s(kind);
        this->finish();
    }

    //#CLEAR
    void finishVariableDeclarator(Node& id, Node& init) {
        addType("VariableDeclarator");
        reg("id", id);
        reg("init", init);
        this->finish();
    }

    //#CLEAR
    void finishWhileStatement(Node& test, Node& body) {
        addType("WhileStatement");
        reg("test", test);
        reg("body", body);
        this->finish();
    }

    //#CLEAR
    void finishWithStatement(Node& object, Node& body) {
        addType("WithStatement");
        reg("object", object);
        reg("body", body);
        this->finish();
    }
};

    
Node NULLNODE;
NULLNODE.isNull = true;

class WrappingNode : public Node {
public:
    WrappingNode(TokenStruct startToken) : Node() {
        if (extra.range) {
            jv["range"][0] = startToken.start;
            jv["range"][1] = 0;
        }
        if (extra.loc) {
            jv["loc"] = this->WrappingSourceLocation(startToken);
        }
    }
    Json::Value WrappingSourceLocation(startToken) {
        Loc result;
        if (startToken.type == Token["StringLiteral"]) {
            this.start.line = startToken.startLineNumber;
            this.start.column = startToken.start - startToken.startLineStart;
        } else {
            this.start.line = startToken.lineNumber;
            this.start.column = startToken.start - startToken.lineStart;
        }
        //return result;
        return locToJson(result);
    }
};
    
// Return true if there is a line terminator before the next token.

//#CLEAR+
bool peekLineTerminator() {
    int pos = idx,
        line = lineNumber,
        start = lineStart;
    bool found;
    skipComment();
    found = (lineNumber != line);
    idx = pos;
    lineNumber = line;
    lineStart = start;
    return found;
}


// Throw an exception

void throwError(TokenStruct token, u16string messageFormat, string otherMsg) {
    /* var error,
        args = Array.prototype.slice.call(arguments, 2),
        msg = messageFormat.replace(
            /%(\d)/g,
            function (whole, idx) {
                assert(idx < args.length, 'Message reference must be in range');
                return args[idx];
            }
        );

    if (typeof token.lineNumber === 'number') {
        error = new Error('Line ' + token.lineNumber + ': ' + msg);
        error.idx = token.start;
        error.lineNumber = token.lineNumber;
        error.column = token.start - lineStart + 1;
    } else {
        error = new Error('Line ' + lineNumber + ': ' + msg);
        error.idx = idx;
        error.lineNumber = lineNumber;
        error.column = idx - lineStart + 1;
    }

    error.description = msg;
    throw error; */
}

void throwErrorTolerant(TokenStruct token, u16string messageFormat, string otherMsg) {
    /* try {
        throwError(token, messageFormat, otherMessage);
    } catch (e) {
        if (extra.errorTolerant) {
            extra.errors.push_back(e);
        } else {
            throw e;
        }
    } */
}

// Throw an exception because of the token.

void throwUnexpected(TokenStruct token) {
    if (token.type == Token["EOF"]) {
        throwError(token, Messages["UnexpectedEOS"]);
    }

    if (token.type == Token["NumericLiteral"]) {
        throwError(token, Messages["UnexpectedNumber"], "");
    }

    if (token.type == Token["StringLiteral"]) {
        throwError(token, Messages["UnexpectedString"], "");
    }

    if (token.type == Token["Identifier"]) {
        throwError(token, Messages["UnexpectedIdentifier"], "");
    }

    if (token.type == Token["Keyword"]) {
        if (isFutureReservedWord(token.strvalue)) {
            throwError(token, Messages["UnexpectedReserved"],"");
        } else if (strict && isStrictModeReservedWord(token.strvalue)) {
            throwErrorTolerant(token, Messages["StrictReservedWord"], "");
            return;
        }
        throwError(token, Messages["UnexpectedToken"], toU8string(token.strvalue));
    }

    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages["UnexpectedToken"], toU8string(token.strvalue));
}
    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.
   


//! TODO Implement for double and int signature.
//! TODO provide rest of string token types.
void expect(u16string value) {
    var token = lex();
    if (!(has<int>(token.type, {Token["Keyword"],  //# don't include punctuator.
                            Token["StringLiteral"]})) ||
            token.strvalue != value) {
        throwUnexpected(token); //! throwUnexpected()
    }
}


   /* @name expectTolerant
    * @description Quietly expect the given token value when in tolerant mode, otherwise delegates
    * to <code>expect(value)</code>
    * @param {String} value The value we are expecting the lookahead token to have
    * @since 2.0
    */
//! to do include rest of string token types, and make separate override for double vals.
function expectTolerant(u16string value) {
    if (extra.errors) {
        var token = lookahead;
        if (!(has<int>(token.type, {Token["Keyword"],  //# don't include punctuator.
                            Token["StringLiteral"]})) || 
                       token.strvalue != value) {
            throwErrorTolerant(token, Messages["UnexpectedToken"], token.strvalue);
        } else {
            lex();
        }
    } else {
        expect(value);
    }
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
//#CLEAR
void expectKeyword(const u16string keyword) {
    var token = lex();
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

//#CLEAR
void consumeSemicolon() {
    int line;

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B ||3 match(u";")) { //! match(u"") what is this function?
        lex();
        return;
    }

    line = lineNumber;
    skipComment();
    if (lineNumber != line) {
        return;
    }

    if (lookahead.type != Token["EOF"] && !match(u"}")) { //! match()
        throwUnexpected(lookahead); //! throwUnexpected()
    }
}

    // Return true if provided expression is LeftHandSideExpression


bool isLeftHandSide(const expr) { //! expr.type
    return expr.type == Syntax["Identifier"] || expr.type == Syntax["MemberExpression"];
};


// 11.1.4 Array Initialiser


//#CLEAR
Node parseArrayInitialiser() {
    vector< Node > elements;
    Node node;

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
    return node;
}

// 11.1.5 Object Initialiser

//#partial
Node parsePropertyFunction(vector<> param, TokenStruct first) { //!typeof param
    bool previousStrict;
    Node body, node;

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (!(first.isNull) && strict && isRestrictedWord(param[0].name)) { //! type param
        throwErrorTolerant(first, Messages["StrictParamName"],"");
    }
    strict = previousStrict;
    node.finishFunctionExpression(null, param, [], body); //! args validity.
    return node;
}

//#CLEAR
Node parseObjectPropertyKey() {
    TokenStruct token;
    Node node;

    token = lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token.type == Token["StringLiteral"] || token.type == Token["NumericLiteral"]) {
        if (strict && token.octal) {
            throwErrorTolerant(token, Messages["StrictOctalLiteral"], "");
        }
        node.finishLiteral(token);
        return node;
    }

    node.finishIdentifier(token.strvalue);
    return node;
}

//#CLEAR
Node parseObjectProperty() {
    TokenStruct token;
    vector<Node> param;
    Node id, key, value, node;

    token = lookahead;

    if (token.type == Token["Identifier"]) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token.strvalue == u"get" && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            expect(u")");
            value = parsePropertyFunction({}, NULLTOKEN);
            node.finishProperty(u"get", key, value);
            return node;
        }
        if (token.strvalue == u"set" && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            token = lookahead;
            if (token.type != Token["Identifier"]) {
                expect(u")");
                throwErrorTolerant(token, 
                                   Messages["UnexpectedToken"], 
                                   toU8string(token.strvalue));
                value = parsePropertyFunction([{}, NULLTOKEN]);
            } else {
                param = vector< Node >({ parseVariableIdentifier() });
                expect(u")");
                value = parsePropertyFunction(param, token);
            }
            node.finishProperty(u"set", key, value);
            return node;
        }
        expect(u":");
        value = parseAssignmentExpression();
        node.finishProperty(u"init", id, value);
        return node;
    }
    if (token.type == Token["EOF"] || token.type == Token["Punctuator"]) {
        throwUnexpected(token);
    } else {
        key = parseObjectPropertyKey();
        expect(u":");
        value = parseAssignmentExpression();
        node.finishProperty(u"init", key, value);
        return node;
    }
}

Node parseObjectInitialiser() {
    var properties = [];
    TokenSruct token;
    Node node, property;
    u16string name, key, kind, map = {};

    expect(u"{");

    while (!match(u"}")) {
        property = parseObjectProperty();

        if (property.key.type == Syntax["Identifier"]) {
            name = property.key.name; //! property.key
        } else {
            name = toString(property.key.value); //! property.key
        }
        kind = (property.kind == u"init") ? PropertyKind.Data : (property.kind == u"get") ? PropertyKind.Get : PropertyKind.Set; //! variaous.

        key = '$' + name;
        if (Object.prototype.hasOwnProperty.call(map, key)) {
            if (map[key] == PropertyKind.Data) {
                if (strict && kind == PropertyKind.Data) {
                    throwErrorTolerant(NULLTOKEN, Messages["StrictDuplicateProperty"]);
                } else if (kind != PropertyKind.Data) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorDataProperty"]);
                }
            } else {
                if (kind == PropertyKind.Data) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorDataProperty"]);
                } else if (map[key] & kind) {
                    throwErrorTolerant(NULLTOKEN, Messages["AccessorGetSet"]);
                }
            }
            map[key] |= kind;
        } else {
            map[key] = kind;
        }

        properties.push(property);

        if (!match(u"}")) {
            expectTolerant(',');
        }
    }

    expect(u"}");

    node.finishObjectExpression(properties);
    return node;
}

// 11.1.6 The Grouping Operator

function parseGroupExpression() {
    var expr;
    expect(u"(");
    if (match(u")")) {
        lex();
        return PlaceHolders.ArrowParameterPlaceHolder; //! PlaceHolders
    }
    ++state.parenthesisCount;
    expr = parseExpression();
    expect(u")");
    return expr;
}


// 11.1 Primary Expressions

Node parsePrimaryExpression() {
    int type; 
    TokenStruct token;
    Node expr, node;

    if (match(u"(")) {
        return parseGroupExpression();
    }

    if (match(u"[")) {
        return parseArrayInitialiser();
    }

    if (match(u"{")) {
        return parseObjectInitialiser();
    }

    type = lookahead.type;


    expr = node;
    if (type == Token["Identifier"]) {
        expr.finishIdentifier(lex().value);
    } else if (type == Token["StringLiteral"] || type == Token["NumericLiteral"]) {
        if (strict && lookahead.octal) {
            throwErrorTolerant(lookahead, Messages["StrictOctalLiteral"]);
        }
        expr.finishLiteral(lex());
    } else if (type == Token["Keyword"]) {
        if (matchKeyword('function')) {
            return parseFunctionExpression();
        }
        if (matchKeyword('this')) {
            lex();
            expr.finishThisExpression();
        } else {
            throwUnexpected(lex());
        }
    } else if (type == Token["BooleanLiteral"]) {
        token = lex();
        token.bvalue = (token.strvalue == 'true'); //!Todo does boolean literal use string or actual bool value on scan?
        expr.finishLiteral(token);
    } else if (type == Token["NullLiteral"]) {
        token = lex();
        token.value = null;
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

    return expr;
}

// 11.2 Left-Hand-Side Expressions
//# CLEAR
vector< Node > parseArguments() {
    vector< Node > args; 
    expect(u"(");
    if (!match(u")")) {
        while (idx < length) {
            args.push(parseAssignmentExpression());
            if (match(u")")) {
                break;
            }
            expectTolerant(u",");
        }
    }
    expect(u")");
    return args;
}

//#CLEAR
Node parseNonComputedProperty() {
    TokenStruct token;
    Node node;

    token = lex();

    if (!isIdentifierName(token)) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.strvalue);
    return node;
}

//#CLEAR
Node parseNonComputedMember() {
    expect(u".");
    return parseNonComputedProperty();
}

//#CLEAR
Node parseComputedMember() {
    Node expr;
    expect(u"[");
    expr = parseExpression();
    expect(u"]");
    return expr;
}

//#CLEAR
Node parseNewExpression() {
    vector< Node > args;
    Node callee, node;

    expectKeyword(u"new");
    callee = parseLeftHandSideExpression();
    if (match(u"(")) { args = parseArguments(); }

    node.finishNewExpression(callee, args);
    return node;
}

//#CLEAR
Node parseLeftHandSideExpressionAllowCall() {
    vector< Node > args;
    Node expr, property, tmpnode;
    TokenStruct startToken;
    bool previousAllowIn = state.allowIn;

    startToken = lookahead;
    state.allowIn = true;
    expr = matchKeyword('new') ? parseNewExpression() : parsePrimaryExpression();

    for (;;) {
        if (match(u".")) {
            property = parseNonComputedMember();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else if (match(u"(")) {
            args = parseArguments();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishCallExpression(expr, args);
            expr = tmpnode;
        } else if (match(u"[")) {
            property = parseComputedMember();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else {
            break;
        }
    }
    state.allowIn = previousAllowIn;

    return expr;
}

//#CLEAR
Node parseLeftHandSideExpression() {
    Node tmpnode, expr, property;
    TokenStruct startToken;
    assert(state.allowIn, "callee of new expression always allow in keyword.");
    startToken = lookahead;
    expr = matchKeyword('new') ? parseNewExpression() : parsePrimaryExpression();
    for (;;) {
        if (match(u"[")) {
            property = parseComputedMember();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else if (match(u".")) {
            property = parseNonComputedMember();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else {
            break;
        }
    }
    return expr;
}

// 11.3 Postfix Expressions
//#CLEAR
Node parsePostfixExpression() {
    Node expr,tmpnode;
    TokenStruct token, startToken = lookahead;

    expr = parseLeftHandSideExpressionAllowCall();

    if (lookahead.type == Token["Punctuator"]) {
        if ((match(u"++") || match(u"--")) && !peekLineTerminator()) {
            // 11.3.1, 11.3.2
            if (strict && expr.type == Syntax["Identifier"] && isRestrictedWord(expr.name)) {
                throwErrorTolerant(NULLTOKEN, Messages["StrictLHSPostfix"],"");
            }

            if (!isLeftHandSide(expr)) {
                throwErrorTolerant(NULLTOKEN, Messages["InvalidLHSInAssignment"],"");
            }

            token = lex();
            tmpnode = new WrappingNode(startToken);
            tmpnode.finishPostfixExpression(token.strvalue, expr);
            return tmpnode;
        }
    }

    return expr;
}

// 11.4 Unary Operators
//#CLEAR
Node parseUnaryExpression() {
    TokenStruct token, startToken;
    Node expr, tmpnode;
    u16string exprname;

    if (lookahead.type != Token["Punctuator"] && lookahead.type != Token["Keyword"]) {
        expr = parsePostfixExpression();
    } else if (match(u"++") || match(u"--")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        // 11.4.4, 11.4.5
        if (strict && expr.type == Syntax["Identifier"] && isRestrictedWord(expr.name)) {
            throwErrorTolerant(NULLTOKEN, Messages["StrictLHSPrefix"],"");
        }

        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, Messages["InvalidLHSInAssignment"]);
        }

        tmpnode = new WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        return tmpnode;
    } else if (match(u"+") || match(u"-") || match(u"~") || match(u"!")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = new WrappingNode(startToken);
        tmpnode = finishUnaryExpression(token.strvalue, expr);
        return tmpnode;
    } else if (matchKeyword(u"delete") || matchKeyword(u"void") || matchKeyword(u"typeof")) {
        startToken = lookahead;
        token = lex();
        expr = parseUnaryExpression();
        tmpnode = new WrappingNode(startToken);
        tmpnode.finishUnaryExpression(token.strvalue, expr);
        if (strict && token.strvalue == u"delete" && expr.type == Syntax["Identifier"]) {
            throwErrorTolerant(NULLTOKEN, Messages["StrictDelete"], "");
        }
        return tmpnode;
    } else {
        expr = parsePostfixExpression();
    }

    return expr;
}

//#CLEAR
int binaryPrecedence(TokenStruct token, bool allowIn) {
    int prec = 0;
    u16string tokval;

    if (token.type != Token["Punctuator"] && token.type != Token["Keyword"]) {
        return 0;
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
        prec = allowIn ? 7 : 0
    } else if (has(tokval,
                   {u"<<", u">>", u">>>"})) {
        prec = 8;
    } else if (tokval == u"+" || tokval == u"-") {
        prec = 9;
    } else if (has(tokval, {u"*", u"/", u"%"})) {
        prec = 11;
    }

    return prec;
}

// 11.5 Multiplicative Operators
// 11.6 Additive Operators
// 11.7 Bitwise Shift Operators
// 11.8 Relational Operators
// 11.9 Equality Operators
// 11.10 Binary Bitwise Operators
// 11.11 Binary Logical Operators

Node parseBinaryExpression() {

    Node tmpnode, expr, left, right;
    vector < Node > stack;
    TokenStruct marker, token;
    vector< TokenStruct > markers;
    var oper;
    int i, prec;

    marker = lookahead;
    left = parseUnaryExpression();
    if (left == PlaceHolders.ArrowParameterPlaceHolder) { //! placeholder
        return left;
    }

    token = lookahead;
    prec = binaryPrecedence(token, state.allowIn);
    if (prec == 0) {
        return left;
    }
    token.prec = prec; //!
    lex();

    markers.push_back(marker);
    markers.push_back(lookahead);
    right = parseUnaryExpression();

    stack.push_back(left);
    stack.push_back(token);
    stack.push_back(right);

    while ((prec = binaryPrecedence(lookahead, state.allowIn)) > 0) { //? will this work the same in c++ as in js

        // Reduce: make a binary expression from the three topmost entries.
        while ((stack.size() > 2) && (prec <= stack[stack.size() - 2].prec)) { //? will this work the same in c++ as in js?
            right = stack.pop();
            oper = stack.pop().value;
            left = stack.pop();
            markers.pop();
            expr = new WrappingNode(markers[markers.length - 1]);
            expr.finishBinaryExpression(oper, left, right);
            stack.push(expr);
        }

        // Shift.
        token = lex();
        token.prec = prec;
        stack.push(token);
        markers.push(lookahead);
        expr = parseUnaryExpression();
        stack.push(expr);
    }

    // Final reduce to clean-up the stack.
    i = stack.length - 1;
    expr = stack[i];
    markers.pop();
    while (i > 1) {
        tmpnode = new WrappingNode(markers.pop());
        tmpnode.finishBinaryExpression(stack[i - 1].value, stack[i - 2], expr);
        expr = tmpnode;
        i -= 2;
    }

    return expr;
}


// 11.12 Conditional Operator

//#partial
Node parseConditionalExpression() {
    Node expr, tmpnode, consequent, alternate;
    bool previousAllowIn;
    TokenStruct startToken;

    startToken = lookahead;

    expr = parseBinaryExpression();
    if (expr == PlaceHolders.ArrowParameterPlaceHolder) { //! get arrowparameterplaceholder coord.
        return expr;
    }
    if (match(u"?")) {
        lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        state.allowIn = previousAllowIn;
        expect(u":");
        alternate = parseAssignmentExpression();

        tmpnode = new WrappingNode(startToken);
        tmpnode.finishConditionalExpression(expr, consequent, alternate);
        return tmpnode;
    }

    return expr;
}

// [ES6] 14.2 Arrow Function

function parseConciseBody() {
    if (match(u"{")) {
        return parseFunctionSourceElements();
    }
    return parseAssignmentExpression();
}

struct ReinterpretOut {
    TokenStruct firstRestricted;
    TokenStruct stricted;
    u16string message;
    vector< Node > params;
    vector< Node > defaults;
    int rest;
};
ReinterpretOut reinterpretAsCoverFormalsList(vector< Node > expressions) {
    int i, len, defaultCount;
    var param;
    vector<> params;
    vector<> defaults;
    ParseParamOpts options; //!todo move this struct to top of code.
    var rest;

    defaultCount = 0;
    rest = null; //!

    for (i = 0, len = expressions.size(); i < len; i += 1) {
        param = expressions[i];
        if (param.type == Syntax["Identifier"]) {
            params.push_back(param);
            defaults.push_back(null);
            validateParam(options, param, param.name);
        } else if (param.type == Syntax["AssignmentExpression"]) {
            params.push(param.left);
            defaults.push(param.right);
            ++defaultCount;
            validateParam(options, param.left, param.left.name);
        } else {
            return null;
        }
    }

    if (options.message == Messages["StrictParamDupe"]) {
        throwError(
            strict ? options.stricted : options.firstRestricted,
            options.message,
            ""
        );
    }

    if (defaultCount === 0) {
        defaults = [];
    }

    return {
        params: params,
        defaults: defaults,
        rest: rest,
        stricted: options.stricted,
        firstRestricted: options.firstRestricted,
        message: options.message
    };
}
    
Node parseArrowFunctionExpression(options, Node node) {
     bool previousStrict;
     var body;

    expect(u"=>");
    previousStrict = strict;

    body = parseConciseBody();

    if (strict && options.firstRestricted) {
        throwError(options.firstRestricted, options.message);
    }
    if (strict && options.stricted) {
        throwErrorTolerant(options.stricted, options.message);
    }

    strict = previousStrict;

    node.finishArrowFunctionExpression(options.params, options.defaults, body, body.type !== Syntax["BlockStatement"]);
    return node;
}

    // 11.13 Assignment Operators


//#partial
Node parseAssignmentExpression() {
    int oldParenthesisCount;
    TokenStruct token, startToken;
    Node expr, right;
    ReinterpretOut list; //! type list

    oldParenthesisCount = state.parenthesisCount;

    startToken = lookahead;
    token = lookahead;

    expr = parseConditionalExpression();

    if (expr == PlaceHolders.ArrowParameterPlaceHolder || match(u"=>")) {
        if (state.parenthesisCount == oldParenthesisCount ||
                state.parenthesisCount == (oldParenthesisCount + 1)) {            
            if (expr.type == Syntax["Identifier"]) {
                list = reinterpretAsCoverFormalsList({ expr }); //!type list
            } else if (expr.type == Syntax["Assignmentxpression"]) {
                list = reinterpretAsCoverFormalsList({ expr });
            } else if (expr.type == Syntax["SequenceExpression"]) {
                list = reinterpretAsCoverFormalsList(expr.expressions); //! expr.expressions
            } else if (expr == PlaceHolders.ArrowParameterPlaceHolder) {
                list = reinterpretAsCoverFormalsList({}); //! possible to initialize with empty brackets?
            }
            if (list) {
                return parseArrowFunctionExpression(list, new WrappingNode(startToken));
            }
        }
    }

    if (matchAssign()) {
        // LeftHandSideExpression
        if (!isLeftHandSide(expr)) {
            throwErrorTolerant(NULLTOKEN, Messages["InvalidLHSInAssignment"], "");
        }

        // 11.13.1
        if (strict && expr.type == Syntax["Identifier"] && isRestrictedWord(expr.name)) {
            throwErrorTolerant(token, Messages["StrictLHSAssignment"], "");
        }

        token = lex();
        right = parseAssignmentExpression();
        expr = new WrappingNode(startToken);
        expr.finishAssignmentExpression(token, expr, right); 
    }

    return expr;
}

// 11.14 Comma Operator
//#CLEAR
Node parseExpression() {
    Node expr; 
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
            expressions.push(parseAssignmentExpression());
        }

        expr = new WrappingNode(startToken);
        expr.finishSequenceExpression(expressions);
    }

    return expr;
}

// 12.1 Block
//#partial
vector< Node > parseStatementList() {
    vector< Node > list;
    Node statement;

    while (idx < length) {
        if (match(u"}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement.isNull) { //! under what conditions would this be null?
            break;
        }
        list.push(statement);
    }

    return list;
}

//#CLEAR
Node parseBlock() {
    vector< Node > block;
    Node node;

    expect(u"{");
    block = parseStatementList();
    expect(u"}");
    node.finishBlockStatement(block);
    return node;
}


// 12.2 Variable Statement

//#CLEAR
Node parseVariableIdentifier() {
    TokenStruct token;
    Node node;

    token = lex();

    if (token.type != Token["Identifier"]) {
        throwUnexpected(token);
    }

    node.finishIdentifier(token.value);
    return node;
}

//#CLEAR
Node parseVariableDeclaration(u16string kind) {
    Node id, node, init = NULLNODE;

    id = parseVariableIdentifier();

    // 12.2.1
    if (strict && isRestrictedWord(id.name)) {
        throwErrorTolerant(NULLTOKEN, Messages["StrictVarName"], "");
    }

    if (kind == u"const") {
        expect(u"=");
        init = parseAssignmentExpression();
    } else if (match(u"=")) {
        lex();
        init = parseAssignmentExpression();
    }

    node.finishVariableDeclarator(id, init);
    return node;
}

//#CLEAR
Vector< Node > parseVariableDeclarationList(u16string kind) {
    vector< Node > list; 

    do {
        list.push_back(parseVariableDeclaration(kind));
        if (!match(u",")) {
            break;
        }
        lex();
    } while (idx < length);

    return list;
}

//#CLEAR
Node parseVariableStatement(Node node) {
    vector< Node > declarations;

    expectKeyword(u"var");
    declarations = parseVariableDeclarationList(u""); 
    consumeSemicolon();

    node.finishVariableDeclaration(declarations, u"var");
    return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//#CLEAR
Node parseConstLetDeclaration(u16string kind) {
    List< Node > declarations;
    Node node;

    expectKeyword(kind);
    declarations = parseVariableDeclarationList(kind);
    consumeSemicolon();
    node.finishVariableDeclaration(declarations, kind);
    return node;
}

// 12.3 Empty Statement

//#CLEAR
Node parseEmptyStatement() {
    Node node;
    expect(u";");
    node.finishEmptyStatement();
    return node;
}

// 12.4 Expression Statement
//#CLEAR
Node parseExpressionStatement(Node node) {
    var expr = parseExpression();
    consumeSemicolon();
    node.finishExpressionStatement(expr);
    return node;
}

// 12.5 If statement
//#CLEAR
Node parseIfStatement(Node node) {
    var test;
    Node consequent, alternate;
    expectKeyword(u"if");
    expect(u"(");
    test = parseExpression();
    expect(u")");
    consequent = parseStatement();
    if (matchKeyword('else')) {
        lex();
        alternate = parseStatement();
    } else {
        alternate = NULLNODE;
    }
    node.finishIfStatement(test, consequent, alternate);
    return node;
}

// 12.6 Iteration Statements

//#CLEAR
Node parseDoWhileStatement(Node node) {
    Node body, test;
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
    return node;
}

//#CLEAR
Node parseWhileStatement(Node node) {
    Node test, body;
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
    return node;
}

//#CLEAR
Node parseForVariableDeclaration() {
    TokenStruct token;
    vector< Node > declarations;
    Node node;

    token = lex();
    declarations = parseVariableDeclarationList();
    node.finishVariableDeclaration(declarations, token.strvalue);
    return node;
}

//#CLEAR
Node parseForStatement(Node node) {

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node body, left, right, update=NULLNODE, test=NULLNODE, init=NULLNODE;

    expectKeyword(u"for");
    expect(u"(");

    if (match(u";")) {
        lex();
    } else {
        if (matchKeyword(u"var") || matchKeyword(u"let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (init.jv["declarations"].size() == 1 && matchKeyword(u"in")) { //? more efficient way than jsoncpp access?
                lex();
                left = init;
                right = parseExpression();
                init = null;
            }
        } else {
            state.allowIn = false;
            init = parseExpression();
            state.allowIn = previousAllowIn;

            if (matchKeyword(u"in")) {
                // LeftHandSideExpression
                if (!isLeftHandSide(init)) {
                    throwErrorTolerant(NULLTOKEN, Messages["InvalidLHSInForIn"],"");
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
        node.finishForStatement(init, test, update, body) 
    } else {
        node.finishForInStatement(left, right, body);
    }
    return node;
}

// 12.7 The continue statement
//#CLEAR
Node parseContinueStatement(Node node) {
    Node label = NULLNODE;
    u16string key;

    expectKeyword(u"continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration)) {
            throwError(NULLTOKEN, Messages["IllegalContinue"],"");
        }

        node.finishContinueStatement(NULLNODE);
        return node;
    }

    if (peekLineTerminator()) {
        if (!state.inIteration) {
            throwError(NULLTOKEN, Messages["IllegalContinue"],"");
        }

        node.finishContinueStatement(NULLNODE);
        return node;
    }

    if (lookahead.type == Token["Identifier"]) {
        label = parseVariableIdentifier();

        key = u"$";
        key.append(label.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["UnknownLabel"], toU8string(label.name));
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration)) {
        throwError(NULLTOKEN, Messages["IllegalContinue"],"");
    }

    node.finishContinueStatement(label);
    return node;
}

// 12.8 The break statement
//#CLEAR
Node parseBreakStatement(Node node) {
    Node label;
    u16string key;

    expectKeyword(u"break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        lex();

        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages["IllegalBreak"],"");
        }

        node.finishBreakStatement(NULLNODE);
        return node;
    }

    if (peekLineTerminator()) {
        if (!(state.inIteration || state.inSwitch)) {
            throwError(NULLTOKEN, Messages["IllegalBreak"],"");
        }

        node.finishBreakStatement(NULLNODE);
        return node;
    }

    if (lookahead.type == Token["Identifier"]) {
        label = parseVariableIdentifier();

        key = u"$";
        key.append(label.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["UnknownLabel"], toU8string(label.name));
        }
    }

    consumeSemicolon();

    if (label.isNull && !(state.inIteration || state.inSwitch)) {
        throwError(NULLTOKEN, Messages["IllegalBreak"], "");
    }

    node.finishBreakStatement(label);
    return node;
}

// 12.9 The return statement
//#CLEAR
Node parseReturnStatement(Node node) {
    Node argument = NULLNODE;

    expectKeyword(u"return");

    if (!(state.inFunctionBody)) {
        throwErrorTolerant(NULLTOKEN, Messages["IllegalReturn"], "");
    }

    // 'return' followed by a space and an identifier is very common.
    if (source(idx) == 0x20) {
        if (isIdentifierStart(source(idx + 1))) {
            argument = parseExpression();
            consumeSemicolon();
            node.finishReturnStatement(argument);
            return node;
        }
    }

    if (peekLineTerminator()) {
        node.finishReturnStatement(null);
        return node;
    }

    if (!match(u";")) {
        if (!match(u"}") && lookahead.type != Token["EOF"]) {
            argument = parseExpression();
        }
    }

    consumeSemicolon();

    node.finishReturnStatement(argument);
    return node;
}

// 12.10 The with statement
//#CLEAR
Node parseWithStatement(Node node) {
    Node object, body;

    if (strict) {
        // TODO(ikarienator): Should we update the test cases instead?
        skipComment();
        throwErrorTolerant(NULLTOKEN, Messages["StrictModeWith"], "");
    }

    expectKeyword(u"with");
    expect(u"(");
    object = parseExpression();
    expect(u")");
    body = parseStatement();
    node.finishWithStatement(object, body);
    return node;
}

// 12.10 The swith statement
//#CLEAR
Node parseSwitchCase() {
    Node test, statement, node;
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
    return node;
}

//#CLEAR
Node parseSwitchStatement(Node node) {
    Node discriminant, clause; 
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
        return node;
    }
    oldInSwitch = state.inSwitch;
    state.inSwitch = true;
    defaultFound = false;

    while (idx < length) {
        if (match(u"}")) {
            break;
        }
        clause = parseSwitchCase();
        if (clause.jv["test"].isNull()) { //? possible bottleneck jsoncpp use.
            if (defaultFound) {
                throwError(NULLTOKEN, Messages["MultipleDefaultsInSwitch"],"");
            }
            defaultFound = true;
        }
        cases.push(clause);
    }

    state.inSwitch = oldInSwitch;
    expect(u"}");
    node.finishSwitchStatement(discriminant, cases);
    return node;
}

// 12.13 The throw statement
//#CLEAR
Node parseThrowStatement(Node node) {
    Node argument;

    expectKeyword(u"throw");
    if (peekLineTerminator()) {
        throwError(NULLTOKEN, Messages["NewlineAfterThrow"],"");
    }
    argument = parseExpression();
    consumeSemicolon();
    node.finishThrowStatement(argument);
    return node;
}

// 12.14 The try statement

//#CLEAR
Node parseCatchClause() {
    Node body, param, node;

    expectKeyword(u"catch");

    expect(u"(");
    if (match(u")")) {
        throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (strict && isRestrictedWord(param.name)) { 
        throwErrorTolerant(NULLTOKEN, Messages["StrictCatchVariable"],"");
    }

    expect(u")");
    body = parseBlock();
    node.finishCatchClause(param, body);
    return;
}

//#CLEAR
Node parseTryStatement(Node node) {
    Node block, finalizer; 
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

    if (handlers.size() == 0 && !finalizer) {
        throwError(NULLTOKEN, Messages["NoCatchOrFinally"], "");
    }

    node.finishTryStatement(block, {}, handlers, finalizer); //? pass empty vec in arg ok? 
    return node;
}

// 12.15 The debugger statement
//#CLEAR
Node parseDebuggerStatement(Node node) {
    expectKeyword(u"debugger");
    consumeSemicolon();
    node.finishDebuggerStatement();
    return node;
}

// 12 Statements

//#partial
Node parseStatement() {
    int type = lookahead.type;
    u16string key, tokval;
    Node expr, node, labeledBody;

    if (type == Token["EOF"]) {
        throwUnexpected(lookahead);
    }

    if (type == Token["Punctuator"] && lookahead.strvalue == u"{") {
        return parseBlock();
    }


    if (type == Token["Punctuator"]) {
        tokval = lookahead.strvalue;
        if (tokval == u";") {
            return parseEmptyStatement(node);
        } else if (tokval == u"(") {
            return parseExpressionStatement(node);
        }
    } else if (type == Token["Keyword"]) {
        tokval = lookahead.strvalue;
        if (tokval == u"break") {
            return parseBreakStatement(node);
        } else if (tokval == u"continue") {
            return parseContinueStatement(node);
        } else if (tokval == u"debugger") {
            return parseDebuggerStatement(node);
        } else if (tokval == u"do") {
            return parseDoWhileStatement(node);
        } else if (tokval == u"for") {
            return parseForStatement(node);
        } else if (tokval == u"function") {
            return parseFunctionDeclaration(node);
        } else if (tokval == u"if") {
            return parseIfStatement(node);
        } else if (tokval == u"return") {
            return parseReturnStatement(node);
        } else if (tokval == u"switch") {
            return parseSwitchStatement(node);
        } else if (tokval == u"throw") {
            return parseThrowStatement(node);
        } else if (tokval == u"try") {
            return parseTryStatement(node);
        } else if (tokval == u"var") {
            return parseVariableStatement(node);
        } else if (tokval == u"while") {
            return parseWhileStatement(node);
        } else if (tokval == u"with") {
            return parseWithStatement(node);
        }
    }

    expr = parseExpression(); 

    // 12.12 Labelled Statements
    if ((expr.type == Syntax["Identifier"]) && match(u":")) {
        lex();

        key = u"$";
        key.append(expr.name);
        if (has<u16string>(key, state.labelSet)) {
            throwError(NULLTOKEN, Messages["Redeclaration"], 'Label', expr.name); //! pretty odd call here. 4 args?
        }

        state.labelSet.insert(key);
        labeledBody = parseStatement();
        state.labelSet.erase(key);
        node.finishLabeledStatement(expr, labeledBody);
        return node;
    }

    consumeSemicolon();

    node.finishExpressionStatement(expr);
    return node;
}

// 13 Function Definition
//#CLEAR
Node parseFunctionSourceElements() {
    Node sourceElement;
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive,
        oldLabelSet, oldInIteration, oldInSwitch, oldInFunctionBody, oldParenthesisCount;
    Node node;
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
        if (sourceElement.jv["expression"]["type"] != Syntax["Literal"]) { 
            //? this one I doubt there's more an efficient way to do this
            //? then jsoncpp accesses. Storing node hierarchies just to fix this call seems to 
            //? be likely less performant.
            // this is not directive
            break;
        }
        directive = slice(sourceraw, token.start + 1, token.end - 1);
        if (directive == u"use strict") {
            strict = true;
            if (!(firstRestricted.isNull)) {
                throwErrorTolerant(firstRestricted, Messages["StrictOctalLiteral"],"");
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

    state.labelSet = oldstate.LabelSet;
    state.inIteration = oldstate.inIteration;
    state.inSwitch = oldstate.inSwitch;
    state.inFunctionBody = oldstate.inFunctionBody;
    state.parenthesisCount = oldstate.parenthesisCount;

    node.finishBlockStatement(sourceElements);
    return node;
}

//#CLEAR
struct ParseParamsStruct {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    TokenStruct firstRestricted;
    TokenStruct stricted;
    unordered_set<u16string> paramSet;
    u16string message;
};

//#CLEAR
void validateParam(ParseParamOpts options, TokenStruct param, u16string name) {
    u16string key = '$' + name;
    if (strict) {
        if (isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages["StrictParamName"];
        }
        if (has(key, options.paramSet)) {
            options.stricted = param;
            options.message = ["Messages.StrictParamDupe"];
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
}

//#CLEAR
bool parseParam(ParseParamOpts options) { 
    TokenStruct token; 
    Node param, def;

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

    return !match(u")");
}

//#CLEAR
struct ParseParamsOut {
    TokenStruct firstRestricted;
    TokenStruct stricted;
    u16string message;
    vector< Node > params;
    vector< Node > defaults;
};

//#CLEAR
function parseParams(TokenStruct firstRestricted) {
    ParseParamOpts options;
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

    if (options.defaultCount == 0) {
        options.defaults.clear();
    }

    out.params = options.params;
    out.defaults = options.defaults;
    out.stricted = options.stricted;
    out.firstRestricted = options.firstRestricted;
    out.message = options.message;
    return out;
}

//#partial
Node parseFunctionDeclaration() {
    TokenStruct token, firstRestricted, stricted;
    u16string message, tokval;
    Node id;
    id.isNull = true;
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    body; //!type
    bool previousStrict;
    Node node;
    
    expectKeyword(u"function");
 
    token = lookahead;
    id = parseVariableIdentifier(); 
    tokval = token.strvalue;

    firstRestricted.isNull = true;
    if (strict) {
        if (isRestrictedWord(tokval)) {
            throwErrorTolerant(token, Messages["StrictFunctionName"], "");
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

    tmp = parseParams(firstRestricted);
    params = tmp.params;
    defaults = tmp.defaults;
    stricted = tmp.stricted;
    firstRestricted = tmp.firstRestricted;

    if (tmp.message) {
        message = tmp.message;
    }

    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (strict && !(firstRestricted.isNull)) {
        throwError(firstRestricted, message, "");
    }
    if (strict && !(stricted.isNull)) {
        throwErrorTolerant(stricted, message, "");
    }
    strict = previousStrict;

    node.finishFunctionDeclaration(id, params, defaults, body);
    return node;
}

//#partial
Node parseFunctionExpression() {
    TokenStruct token, firstRestricted, stricted;
    u16string message, tokval;
    Node id;
    id.isNull = true;
    ParseParamsOut tmp;
    vector< Node > params;
    vector< Node > defaults;
    body; //!type
    bool previousStrict;
    Node node;
    
    expectKeyword(u"function");
    
    if (!match(u"(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 
        tokval = token.strvalue;
        if (strict) {
            if (isRestrictedWord(tokval)) {
                throwErrorTolerant(token, Messages["StrictFunctionName"],"");
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

    if (tmp.message) {
        message = tmp.message;
    }
    
    previousStrict = strict;
    body = parseFunctionSourceElements();
    if (strict && !(firstRestricted.isNull)) {
        throwError(firstRestricted, message, "");
    }
    if (strict && !(stricted.isNull)) {
        throwErrorTolerant(stricted, message, "");
    }
    strict = previousStrict;

    node.finishFunctionExpression(id, params, defaults, body);
    return node;
}

    // 14 Program

//#partial
Node parseSourceElement() { //ret sig also rets of calls w/in
    u16string val;
    if (lookahead.type == Token["Keyword"]) {
        val = lookahead.strvalue;
        if (val == u"const" || val == u"let") {
            return parseConstLetDeclaration(lookahead.value);
        } else if (val == u"function") {
            return parseFunctionDeclaration(); //confirmed Node
        } else {
            return parseStatement();
        }
    }

    if (lookahead.type != Token["EOF"]) {
        return parseStatement();
    }
}

//#partial
vector< Node > parseSourceElements() {
    Node sourceElement;
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
        if (sourceElement.expression.type != Syntax["Literal"]) { 
              //! can we guarantee source element will have the xpression type?
              //! if so how do we polymorphically access it?
            // this is not directive
            break;
        }
        directive = slice(sourceraw, token.start + 1, token.end - 1);
        if (directive == u"use strict") {
            strict = true;
            if (!(firstRestricted.isNull)) { 
                throwErrorTolerant(firstRestricted, Messages["StrictOctalLiteral"],"");
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
    return sourceElements;
}

Node parseProgram() {
    Node node;
    vector< Node > body;

    skipComment();
    peek();
    strict = false;

    body = parseSourceElements();
    node.finishProgram(body);
    return node;
}

//#CLEAR
void filterTokenLocation() {
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
}

string tokenize(string code) {
    OptionsStruct o;
    return tokenize(code, o);
}
string tokenize(string code, OptionsStruct options) {
    return tokenize(toU16string(code), options);
}
string tokenize(u16string code) {
    OptionsStruct o;
    return tokenize(code, o);
}

//#partial
string tokenize(u16string code, OptionsStruct options) {
    vector<TokenRecord> tokens;
    Json::Value outJson;
    Json::FastWriter fw;

    //! do this in outer javascript before call. 
    //! as it's a js-environment specific problem.

    //if (typeof code !== 'string' && !(code instanceof String)) {
    //    code = toString(code);
    //}

    sourceraw = code.data();
    idx = 0;
    lineNumber = (source.length > 0) ? 1 : 0;
    lineStart = 0;
    length = source.length;
    lookahead = null;

    state.allowIn = true;
    state.inFunctionBody = false;
    state.inIteration = false;
    state.inSwitch = false;
    state.lastCommentStart = -1;
    //! parenthesisCount for state not provided here normally as in parse. 
    //! That going to be a problem for us later?

    // Of course we collect tokens here.
    options["tokens"] = true;

    extra.Extra();
    extra.tokenTracking = true; 
    extra.tokenize = true;
    // The following two fields are necessary to compute the Regex tokens.
    extra.openParenToken = -1;
    extra.openCurlyToken = -1;

    extra.range = options.range; 
    extra.loc = options.loc;
    extra.commentTracking = options.comment;
    extra.errorTolerant = options.tolerant;

    try {
        peek();
        if (lookahead.type == Token["EOF"]) {
            outJson["tokenlist"] = vec2jsonCallback(extra.tokenRecords, &TokenRecord::toJson);
            free sourceraw;
            return fw.write(outJson);
        }

        lex();
        while (lookahead.type != Token["EOF"]) {
            try {
                lex();
            } catch (lexError) { //!
                if (extra.errorTolerant) {
                    extra.errors.push_back(lexError); //!
                    // We have to break on the first error
                    // to avoid infinite loops.
                    break;
                } else {
                    throw lexError;
                }
            }
        }

        filterTokenLocation();
        outJson["tokenlist"] = vec2jsonCallback(extra.tokenRecords, &TokenRecord::toJson); 
        if (extra.commentTracking) {
            outJson["comments"] = vec2jsonCallback(extra.comments,
                                                   &Comment::toJson);
        }
        if (extra.errorTolerant) {
            outJson["errors"] = vec2jsonCallback(extra.errors,
                                                 &ExError::toJson);
        }
    } catch (e) {
        throw e;
    } finally {
    }

    free sourceraw;
    return fw.write(tokens);
}

string parse(const string code) {
    OptionsStruct o;
    return parse(code, o);
}
string parse(const string code, const OptionsStruct options) {
    return parse(toU16string(code), options);
}
string parse(const u16string code) {
    OptionsStruct o;
    return parse(code, o);
}

//#partial
string parse(const u16string code, const OptionsStruct options) {
    Node programNode;
    Json::Value programJson;
    //! do this in outer javascript before call. 
    //! as it's a js-environment specific problem.

    //if (typeof code !== 'string' && !(code instanceof String)) { 
    //    code = toString(code);
    //}

    sourceraw = code.data();
    idx = 0;
    lineNumber = (source.length > 0) ? 1 : 0;
    lineStart = 0;
    length = source.length;
    lookahead = null;
    state.allowIn = true;
    state.parenthesisCount = 0;
    state.inFunctionBody = false;
    state.inIteration = false;
    state.inSwitch = false;
    state.lastCommentStart = -1;

    extra.Extra();
 
    extra.range = options.range;
    extra.loc = options.loc;
    extra.attachComment = options.attachComment;

    if (extra.loc && options.hasSource) {
        exta.hasSource = true;
        extra.source = options.source;
    } else { extra.hasSource = false; }

    extra.tokenTracking = options.takens;
    extra.commentTracking = options.comment;
    extra.errorTolerant = options.tolerant;

    //values which aren't strictly dependent on attachComment being true
    //but attachComment is sufficient for them to be true.
    if (extra.attachComment) {
        extra.range = true;
        extra.commentTracking = true;
    }


    try {
        programNode = parseProgram();
        //programJson["noderoot"] = programNode.jv;
        programJson = programNode.jv;
        programJson["regexp"] = programNode.regexPaths2json();
        if (extra.commentTracking) {
            programJson["comments"] = vec2jsonCallback(extra.comments,
                                                       &Comment::toJson); //! do these .toJson funcs all exist?
        }
        if (extra.tokenTracking) {
            filterTokenLocation();
            programJson["tokens"] = vec2jsonCallback(extra.tokenRecords,
                                                     &TokenStruct::toJson);
        }
        if (extra.errorTolerant) {
            programJson["errors"] = vec2jsonCallback(extra.errors,
                                                     &ExError::toJson);
        }
    } catch (e) {
        throw e;
    } finally {
        extra = {};
    }

    free sourceraw;
    Json::FastWriter fw;
    return fw.write(program);
}

//WARNING! ALLOCATES MEM. It's your job to free it.
char* strToChar(string in) {
    char *out = new char[in.size()+1];
    strcpy(out, in.c_str());
    return out;
}

extern "C" {
    char* tokenizeExt(char *code, char* options) {
        return strToChar(tokenize(string(code_str), 
                                  Options.dejson(string(options))));
    }
    char* parseExt(char *code, char* options) {
        return strToChar(parse(string(code_str), 
                                  Options.dejson(string(options))));
    }
}


    // Sync with *.json manifests.
    exports.version = '2.0.0-dev';

    exports.tokenize = tokenize;

    exports.parse = parse;

    // Deep copy.

    exports.Syntax = (function () {
        var name, types = {};

        if (typeof Object.create === 'function') {
            types = Object.create(null);
        }

        for (name in Syntax) {
            if (Syntax["hasOwnProperty"](name)) {
                types[name] = Syntax[name];
            }
        }

        if (typeof Object.freeze === 'function') {
            Object.freeze(types);
        }

        return types;
    }());

}));

*/
