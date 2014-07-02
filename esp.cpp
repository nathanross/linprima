#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <regex>
#include <unordered_set>
#include <json/json.h>
//algorithm is for find(vector.begin()
using namespace std;

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
template<typename T> bool has(T needle, unordered_set<T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}
template<typename T> bool hasStringKey(string needle, map<string,T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}

bool contains() {
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



struct RegexResult {
    u16string value;
    u16string literal;
    int start;
    int end;
    RegexResult() { start = -1; end = -1; }
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
    int range;
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

struct TokenStruct {
    bool isNull; 
    int type;
    u16string typestring;
    void* value;
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
        free (this->value);
    }
};

TokenStruct NULLTOKEN;
NULLTOKEN.isNull = true;

struct ExtraStruct {
    bool tokenTracking; //port-specific member to replace "if (extra.tokens)"
    vector<TokenStruct> tokens;

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

//! expr type (see isLeftHandSide)


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

map<string, string> Syntax = {
    {"AssignmentExpression", "AssignmentExpression"},
    {"ArrayExpression", "ArrayExpression"},
    {"ArrowFunctionExpression", "ArrowFunctionExpression"},
    {"BlockStatement", "BlockStatement"},
    {"BinaryExpression", "BinaryExpression"},
    {"BreakStatement", "BreakStatement"},
    {"CallExpression", "CallExpression"},
    {"CatchClause", "CatchClause"},
    {"ConditionalExpression", "ConditionalExpression"},
    {"ContinueStatement", "ContinueStatement"},
    {"DoWhileStatement", "DoWhileStatement"},
    {"DebuggerStatement", "DebuggerStatement"},
    {"EmptyStatement", "EmptyStatement"},
    {"ExpressionStatement", "ExpressionStatement"},
    {"ForStatement", "ForStatement"},
    {"ForInStatement", "ForInStatement"},
    {"FunctionDeclaration", "FunctionDeclaration"},
    {"FunctionExpression", "FunctionExpression"},
    {"Identifier", "Identifier"},
    {"IfStatement", "IfStatement"},
    {"Literal", "Literal"},
    {"LabeledStatement", "LabeledStatement"},
    {"LogicalExpression", "LogicalExpression"},
    {"MemberExpression", "MemberExpression"},
    {"NewExpression", "NewExpression"},
    {"ObjectExpression", "ObjectExpression"},
    {"Program", "Program"},
    {"Property", "Property"},
    {"ReturnStatement", "ReturnStatement"},
    {"SequenceExpression", "SequenceExpression"},
    {"SwitchStatement", "SwitchStatement"},
    {"SwitchCase", "SwitchCase"},
    {"ThisExpression", "ThisExpression"},
    {"ThrowStatement", "ThrowStatement"},
    {"TryStatement", "TryStatement"},
    {"UnaryExpression", "UnaryExpression"},
    {"UpdateExpression", "UpdateExpression"},
    {"VariableDeclaration", "VariableDeclaration"},
    {"VariableDeclarator", "VariableDeclarator"},
    {"WhileStatement", "WhileStatement"},
    {"WithStatement", "WithStatement"}
};

// PlaceHolders { Arrow { type } } //!!

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

bool isDecimalDigit(char16_t ch) {
    return (ch >= 0x30 && ch <= 0x39); //0..9
}

bool isHexDigit(char16_t ch) {
    return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) != std::string::npos);    
}

bool isOctalDigit(char16_t ch) {
    return (u16string({u"01234567"}).find_first_of(ch) != std::string::npos);    
}

char16_t toLowercaseHex(char16_t ch) { //used in scanHexEscape
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
bool isWhiteSpace(char16_t ch) {
    return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || (ch >= 0x1680 && has<int>(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators

bool isLineTerminator(char16_t ch) {
    return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}

// 7.6 Identifier Names and Identifiers

bool isIdentifierStart(char16_t ch) {
    smatch m;
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierStart"]));
} 

bool isIdentifierPart(char16_t ch) {
    smatch m;
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch >= 0x30 && ch <= 0x39) ||         // 0..9
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && regex_search(toU8string(u16string({ch})), m, Regex["NonAsciiIdentifierPart"]));
}

// 7.6.1.2 Future Reserved Words

bool isFutureReservedWord(u16string id) {
    return has<u16string>(id, { //
            u"class",
            u"enum",
            u"export",
            u"extends",
            u"import",
            u"super"
            });
}

bool isStrictModeReservedWord(u16string id) {
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

bool isRestrictedWord(u16string id) {
    return (id == u"eval" || id == u"arguments");
}

// 7.6.1.1 Keywords
bool isKeyword(u16string id) {
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
        comment.rangeStart = start;
        comment.rangeEnd = end;
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

void skipSingleLineComment(int offset) {
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

void skipSingleLineComment() {
    skipSingleLineComment(0);
}

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
                throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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

    throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
}

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

char16_t scanHexEscape(char16_t prefix) {
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
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
    }

    while (idx < length) {
        ch = source(idx++);
        if (!isHexDigit(ch)) {
            break;
        }
        code = code * 16 + u16string({u"0123456789abcdef"}).find_first_of(toLowercaseHex(ch));
    }

    if (code > 0x10FFFF || ch != u'}') {
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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
            throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (!ch || ch == u'\\' || !isIdentifierStart(ch)) { //!! what does !ch mean in this context?
            throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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
                throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (!ch || ch == u'\\' || !isIdentifierPart(ch)) {
                throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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
                extra.openParenToken = extra.tokens.size();
            } else if (code[0] == 0x7B) {
                extra.openCurlyToken = extra.tokens.size();
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

    throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
    return t; //?! returning this empty on error is not in source behavior. need to make sure it's handled gracefully.
}
    // 7.8.3 Numeric Literals

TokenStruct scanHexLiteral(int start) {
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
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
    }

    if (isIdentifierStart(source(idx))) {
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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

TokenStruct scanOctalLiteral(int start) {
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
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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
                throwError({}, Messages["UnexpectedToken"], "ILLEGAL"); 
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
            throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
        }
    }

    if (isIdentifierStart(source(idx))) {
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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
            if (!ch || !isLineTerminator(ch)) { //!! what does !ch mean in this context?
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
        throwError({}, Messages["UnexpectedToken"], "ILLEGAL");
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

void testRegExp(pattern, flags) {
    int value;
    try {
        value 5;
        //new RegExp(pattern, flags); //!!!! lol.
    } catch (e) {
        throwError({}, Messages["InvalidRegExp"], u""); //? before had no third arg, using "" here.
    }
    return value;
}

RegexResult scanRegExpBody() {
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexResult rr;

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
                throwError({}, Messages["UnterminatedRegExp"], "");
            }
            append(str, ch);
        } else if (isLineTerminator(ch)) {
            throwError({}, Messages["UnterminatedRegExp"], "");
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
        throwError({}, Messages["UnterminatedRegExp"], "");
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length - 2);
    rr.value = body;
    rr.literal = str;
    return rr;
}

RegexResult scanRegExpFlags() {
    char16_t ch;
    u16string str, flags;
    int restore;
    RegexResult rr;

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
                throwErrorTolerant({}, Messages["UnexpectedToken"], "ILLEGAL");
            } else {
                append(str, u'\\');
                throwErrorTolerant({}, Messages["UnexpectedToken"], "ILLEGAL");
            }
        } else {
            append(flags, ch);
            append(str, ch);
        }
    }
    
    rr.value = flags;
    rr.literal = str;
    return rr;
}

RegexResult scanRegExp() {
    int start;
    RegexResult body; 
    RegexResult flags; 
    int value; 
    TokenStruct t;
    RegexResult rr;
    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = null;
    skipComment();
    start = idx;

    body = scanRegExpBody();
    flags = scanRegExpFlags();
    value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t.type = Token["RegularExpression"];
        t.value = value;
        t.lineNumber = lineNumber;
        t.lineStart = lineStart;
        t.start = start;
        t.end = idx;
        return t; //not polymorphic right now. not going to work... :!
    }

    rr.literal = body.literal + flags.literal;
    rr.value = value;
    rr.start = start;
    rr.end = idx;
    return rr;
}

function collectRegex() {
    int pos;
    Loc loc;
    RegexResult regex;
    TokenStruct token;
    u16string tokval;

    skipComment();

    pos = idx;
    loc.start.line = lineNumber;
    loc.start.column = idx - lineStart;

    regex = scanRegExp();
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (!extra.tokenize) {
        // Pop the previous token, which is likely '/' or '/='
        if (extra.tokens.size() > 0) {
            token = extra.tokens[extra.tokens.size() - 1];
            if (token.range[0] == pos && token.typestring == "Punctuator") {
                //? also compares typestring not type. see
                // the function below for more discusion.
                tokval = res_u16(token.value);
                if (tokval == u"/" || tokval == u"/=") { //! polymorphism problems.
                    extra.tokens.pop_back();
                }
            }
        }

        TokenStruct t;
        t.type = u"RegularExpression";
        t.value = regex.literal;
        t.range[0] = pos;
        t.range[1] = idx;
        t.loc = loc;
        extra.tokens.push_back(t);
    }

    return regex; //...
}

bool isIdentifierName(token) {
    return has<int>(token.type, { Token["Identifier"], Token["Keyword"],
                Token["BooleanLiteral"], Token["NullLiteral"]});
}

function advanceSlash() {
    TokenStruct prevToken, checkToken;
    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    prevToken = extra.tokens[extra.tokens.length - 1];
    if (!prevToken) {
        // Nothing before that: it cannot be a division.
        return collectRegex(); //...
    }
    if (prevToken.typestring == u"Punctuator") { //? was compared to a string before? what? hm...
        // ? seems to be called after resolved to name? but seems so inefficient to compare strings...
        //  overloading the same member name with different types even in a dynamic language.
        //  generally represents that the member serves at least two separate roles and has 
        //  outgrown a single name.
        if (res_u16(prevToken.value) == u"]") { //!
            return scanPunctuator();
        }
        if (res_u16(prevToken.value) == u")" && extra.openParenToken >= 1) { //!
            checkToken = extra.tokens[extra.openParenToken - 1];
            if (//checkToken && //# instead of checking for existence, we 
                //# add the openParenToken value check to the condition above.
                checkToken.typestring == u"Keyword" && 
                //? once again a comparison that wouldn't have worked before.
                has<u16string>(res_u16(checkToken.value), {u"if", u"while", u"for", u"with"})) {
                return collectRegex(); //...
            }
            return scanPunctuator();
        }
        if (res_u16(prevToken.value) == u"}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokens[extra.openCurlyToken - 3].typestring == "Keyword") { 
                //? again previously checked type against string here.
                // Anonymous function.
                //- checkToken = extra.tokens[extra.openCurlyToken - 4];
                //- if (!checkToken) {
                if (extra.openCurlyToken == 3)
                    return scanPunctuator();
                }
            } else if (extra.openCurlyToken >= 4,
                       //-extra.tokens[extra.openCurlyToken - 4] &&
                    extra.tokens[extra.openCurlyToken - 4].typestring == "Keyword") {
                // again previously had checked type against string in this cond.
                // Named function.
                //- checkToken = extra.tokens[extra.openCurlyToken - 5];
                //- if (!checkToken) {
                    return collectRegex();
                }
            } else {
                return scanPunctuator();
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (FnExprTokens.idxOf(checkToken.value) >= 0) {
                // It is an expression.
                return scanPunctuator();
            }
            // It is a declaration.
            return collectRegex(); //...
        }
        return collectRegex(); //...
    }
    if (prevToken.type == Token["Keyword"]) { //checking type against string
        return collectRegex(); //...
    }
    return scanPunctuator();
}

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

function collectToken() {
    Loc loc;
    TokenStruct token, t;
    value;

    skipComment();
    loc.start.line = lineNumber;
    loc.start.column = idx - ineStart;

    token = advance();
    loc.end.line = lineNumber;
    loc.end.column = idx - lineStart;

    if (token.type != Token["EOF"]) { //this didn't check against string. is fine.
        value = slice(sourceraw, token.start, token.end);
        t.typestring = TokenName[token.type];
        t.value = new u16string(value);
        t.loc = loc;
        extra.tokens.push_back(t);
    }

    return token;
}

function lex() {
    var token;

    token = lookahead;
    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    lookahead = (typeof extra.tokens !== 'undefined') ? collectToken() : advance(); //!!

    idx = token.end;
    lineNumber = token.lineNumber;
    lineStart = token.lineStart;

    return token;
}

function peek() {
    int pos, line, start;

    pos = idx;
    line = lineNumber;
    start = lineStart;
    lookahead = (typeof extra.tokens !== 'undefined') ? collectToken() : advance(); //!!
    idx = pos;
    lineNumber = line;
    lineStart = start;

}

//# Position and SourceLocation are defined as structs near the top.

//CLEAR 
Loc WrappingSourceLocation(startToken) {
    Loc result;
    if (startToken.type == Token["StringLiteral"]) {
        this.start.line = startToken.startLineNumber;
        this.start.column = startToken.start - startToken.startLineStart;
    } else {
        this.start.line = startToken.lineNumber;
        this.start.column = startToken.start - startToken.lineStart;
    }
    return result;
}
class NodeFinish {
};

class Node {
public:
    int range[2];
    Loc loc;
    bool isNull;
    string type;
    NodeFinish nf;
    Node() { 
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
            this->range[0] = idx;
            this->range[1] = 0;
        }
    }

    void processComment() {
        vector<Comment> trailingComments;
        //! begin
        var lastChild,
            bottomRight = extra.bottomRightStack,
            last = bottomRight[bottomRight.length - 1];

        if (this.type === Syntax.Program) { 
            if (this.body.length > 0) {
                return;
            }
        }
        //!end

        if (extra.trailingComments.size() > 0) {
            if (extra.trailingComments[0].rangeStart >= this->range[1]) {
                trailingComments = extra.trailingComments;
                extra.trailingComments.clear();
            } else {
                extra.trailingComments.clear();
            }
        } else {
            if (last && //! if (last) need better way to check. 
                last.trailingComments && last.trailingComments[0].range[0] >= this.range[1]) { //! last?
                trailingComments = last.trailingComments;
                last.trailingComments.clear();
                //delete last.trailingComments; 
                //? maybe have a boolean to say no trailing comments? length will prob. be workable.
            }
        }

        // Eating the stack.
        if (last) { //!if (last)
            while (last && last.range[0] >= this->range[0]) { //if (last)
                lastChild = last;
                last = bottomRight.pop();
            }
        }

        if (lastChild) { //! if (lastChild) 
            if (lastChild.leadingComments && //!if leadingComments. not size because an empty array returns true in js.
                lastChild.leadingComments[lastChild.leadingComments.size() - 1].range[1] <= this->range[0]) {

                this.leadingComments = lastChild.leadingComments;
                lastChild.leadingComments.clear();
                //lastChild.leadingComments = undefined; //length enough?
            }
        } else if (extra.leadingComments.size() > 0 && 
                   extra.leadingComments[extra.leadingComments.size() - 1].range[1] <= this->range[0]) {
            this.leadingComments = extra.leadingComments;
            extra.leadingComments.clear()
            //extra.leadingComments = [];
        }


        if (trailingComments) {
            this.trailingComments = trailingComments;
        }

        bottomRight.push(this);
    }

    void finish() {
        if (extra.range) {
            this.range[1] = idx;
        }
        if (extra.loc) {
            this->loc.end = new Position();
            if (extra.source) { //! .source type?
                this->loc.source = extra.source; //! need to change structs to add .source.
            }
        }

        if (extra.attachComment) {
            this.processComment();
        }
    }
}


class NodeArrayExpression extends NodeFinish {
    public :
        vector<> elements;
        NodeArrayExpression(vector<> elements) {
            this->elements = elements;
        }
    //this.finish();
};
class NodeArrowFunctionExpression extends NodeFinish {
    public :
        bool generator;
        NodeArrowFunctionExpression(vector<> elements) {
            generator = false;
        }
    //this.finish();
};

    
    NodeArrayExpression(vector<> elements) {
        this->elements = elements;
    }
    //this.finish();
}
    finishArrowFunctionExpression: function (params, defaults, body, expression) {
        this.type = Syntax.ArrowFunctionExpression;
        this.id = null;
        this.params = params;
        this.defaults = defaults;
        this.body = body;
        this.rest = null;
        this.generator = false;
        this.expression = expression;
        this.finish();
        return this;
    },

    finishAssignmentExpression: function (operator, left, right) {
        this.type = Syntax.AssignmentExpression;
        this.operator = operator;
        this.left = left;
        this.right = right;
        this.finish();
        return this;
    },

    finishBinaryExpression: function (operator, left, right) {
        this.type = (operator === '||' || operator === '&&') ? Syntax.LogicalExpression : Syntax.BinaryExpression;
        this.operator = operator;
        this.left = left;
        this.right = right;
        this.finish();
        return this;
    },

    finishBlockStatement: function (body) {
        this.type = Syntax.BlockStatement;
        this.body = body;
        this.finish();
        return this;
    },

    finishBreakStatement: function (label) {
        this.type = Syntax.BreakStatement;
        this.label = label;
        this.finish();
        return this;
    },

    finishCallExpression: function (callee, args) {
        this.type = Syntax.CallExpression;
        this.callee = callee;
        this.arguments = args;
        this.finish();
        return this;
    },

    finishCatchClause: function (param, body) {
        this.type = Syntax.CatchClause;
        this.param = param;
        this.body = body;
        this.finish();
        return this;
    },

    finishConditionalExpression: function (test, consequent, alternate) {
        this.type = Syntax.ConditionalExpression;
        this.test = test;
        this.consequent = consequent;
        this.alternate = alternate;
        this.finish();
        return this;
    },

    finishContinueStatement: function (label) {
        this.type = Syntax.ContinueStatement;
        this.label = label;
        this.finish();
        return this;
    },

    finishDebuggerStatement: function () {
        this.type = Syntax.DebuggerStatement;
        this.finish();
        return this;
    },

    finishDoWhileStatement: function (body, test) {
        this.type = Syntax.DoWhileStatement;
        this.body = body;
        this.test = test;
        this.finish();
        return this;
    },

    finishEmptyStatement: function () {
        this.type = Syntax.EmptyStatement;
        this.finish();
        return this;
    },

    finishExpressionStatement: function (expression) {
        this.type = Syntax.ExpressionStatement;
        this.expression = expression;
        this.finish();
        return this;
    },

    finishForStatement: function (init, test, update, body) {
        this.type = Syntax.ForStatement;
        this.init = init;
        this.test = test;
        this.update = update;
        this.body = body;
        this.finish();
        return this;
    },

    finishForInStatement: function (left, right, body) {
        this.type = Syntax.ForInStatement;
        this.left = left;
        this.right = right;
        this.body = body;
        this.each = false;
        this.finish();
        return this;
    },

    finishFunctionDeclaration: function (id, params, defaults, body) {
        this.type = Syntax.FunctionDeclaration;
        this.id = id;
        this.params = params;
        this.defaults = defaults;
        this.body = body;
        this.rest = null;
        this.generator = false;
        this.expression = false;
        this.finish();
        return this;
    },

    finishFunctionExpression: function (id, params, defaults, body) {
        this.type = Syntax.FunctionExpression;
        this.id = id;
        this.params = params;
        this.defaults = defaults;
        this.body = body;
        this.rest = null;
        this.generator = false;
        this.expression = false;
        this.finish();
        return this;
    },

    finishIdentifier: function (name) {
        this.type = Syntax.Identifier;
        this.name = name;
        this.finish();
        return this;
    },

    finishIfStatement: function (test, consequent, alternate) {
        this.type = Syntax.IfStatement;
        this.test = test;
        this.consequent = consequent;
        this.alternate = alternate;
        this.finish();
        return this;
    },

    finishLabeledStatement: function (label, body) {
        this.type = Syntax.LabeledStatement;
        this.label = label;
        this.body = body;
        this.finish();
        return this;
    },

    finishLiteral: function (token) {
        this.type = Syntax.Literal;
        this.value = token.value;
        this.raw = source.slice(token.start, token.end);
        this.finish();
        return this;
    },

    finishMemberExpression: function (accessor, object, property) {
        this.type = Syntax.MemberExpression;
        this.computed = accessor === '[';
        this.object = object;
        this.property = property;
        this.finish();
        return this;
    },

    finishNewExpression: function (callee, args) {
        this.type = Syntax.NewExpression;
        this.callee = callee;
        this.arguments = args;
        this.finish();
        return this;
    },

    finishObjectExpression: function (properties) {
        this.type = Syntax.ObjectExpression;
        this.properties = properties;
        this.finish();
        return this;
    },

    finishPostfixExpression: function (operator, argument) {
        this.type = Syntax.UpdateExpression;
        this.operator = operator;
        this.argument = argument;
        this.prefix = false;
        this.finish();
        return this;
    },

    finishProgram: function (body) {
        this.type = Syntax.Program;
        this.body = body;
        this.finish();
        return this;
    },

    finishProperty: function (kind, key, value) {
        this.type = Syntax.Property;
        this.key = key;
        this.value = value;
        this.kind = kind;
        this.finish();
        return this;
    },

    finishReturnStatement: function (argument) {
        this.type = Syntax.ReturnStatement;
        this.argument = argument;
        this.finish();
        return this;
    },

    finishSequenceExpression: function (expressions) {
        this.type = Syntax.SequenceExpression;
        this.expressions = expressions;
        this.finish();
        return this;
    },

    finishSwitchCase: function (test, consequent) {
        this.type = Syntax.SwitchCase;
        this.test = test;
        this.consequent = consequent;
        this.finish();
        return this;
    },

    finishSwitchStatement: function (discriminant, cases) {
        this.type = Syntax.SwitchStatement;
        this.discriminant = discriminant;
        this.cases = cases;
        this.finish();
        return this;
    },

    finishThisExpression: function () {
        this.type = Syntax.ThisExpression;
        this.finish();
        return this;
    },

    finishThrowStatement: function (argument) {
        this.type = Syntax.ThrowStatement;
        this.argument = argument;
        this.finish();
        return this;
    },

    finishTryStatement: function (block, guardedHandlers, handlers, finalizer) {
        this.type = Syntax.TryStatement;
        this.block = block;
        this.guardedHandlers = guardedHandlers;
        this.handlers = handlers;
        this.finalizer = finalizer;
        this.finish();
        return this;
    },

    finishUnaryExpression: function (operator, argument) {
        this.type = (operator === '++' || operator === '--') ? Syntax.UpdateExpression : Syntax.UnaryExpression;
        this.operator = operator;
        this.argument = argument;
        this.prefix = true;
        this.finish();
        return this;
    },

    finishVariableDeclaration: function (declarations, kind) {
        this.type = Syntax.VariableDeclaration;
        this.declarations = declarations;
        this.kind = kind;
        this.finish();
        return this;
    },

    finishVariableDeclarator: function (id, init) {
        this.type = Syntax.VariableDeclarator;
        this.id = id;
        this.init = init;
        this.finish();
        return this;
    },

    finishWhileStatement: function (test, body) {
        this.type = Syntax.WhileStatement;
        this.test = test;
        this.body = body;
        this.finish();
        return this;
    },

    finishWithStatement: function (object, body) {
        this.type = Syntax.WithStatement;
        this.object = object;
        this.body = body;
        this.finish();
        return this;
    }


class WrappingNode : public Node {
 public:
    WrappingNode(TokenStruct startToken) : Node() {
        if (extra.range) {
            this->range[0] = startToken.start;
            this->range[1] = 0;
        }
        if (extra.loc) {
            loc = WrappingSourceLocation(startToken);
        }
    }
}

// Return true if there is a line terminator before the next token.

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
        if (isFutureReservedWord(res_u16(token.value))) {
            throwError(token, Messages["UnexpectedReserved"],"");
        } else if (strict && isStrictModeReservedWord(res_u16(token.value))) {
            throwErrorTolerant(token, Messages["StrictReservedWord"], "");
            return;
        }
        throwError(token, Messages["UnexpectedToken"], toU8string(res_u16(token.value)));
    }

    // BooleanLiteral, NullLiteral, or Punctuator.
    throwError(token, Messages["UnexpectedToken"], toU8string(res_u16(token.value)));
}
    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.
   


//! TODO Implement for double and int signature.
//! TODO provide rest of string token types.
void expect(u16string value) {
    var token = lex();
    if (!(has<int>(token.type, {Token["Keyword"],  //# don't include punctuator.
                            Token["StringLiteral"]})) ||
            res_u16(token.value) != value) {
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
                       res_u16(token.value) != value) {
            throwErrorTolerant(token, Messages.UnexpectedToken, token.value);
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
function expectKeyword(u16string keyword) {
    var token = lex();
    if (token.type != Token["Keyword"] || 
        res_u16(token.value) != keyword) {
        throwUnexpected(token);
    }
}
*/

// Return true if the next token matches the specified punctuator.
//#CLEAR
bool match(u16string value) {
    return lookahead.type == Token["Punctuator"] && res_u16(lookahead.value) == value;
}

// Return true if the next token matches the specified keyword

//#CLEAR
bool matchKeyword(u16string keyword) {
    return lookahead.type === Token["Keyword"] && res_u16(lookahead.value) == keyword;
}

    // Return true if the next token is an assignment operator

//#CLEAR
bool matchAssign() {
    if (lookahead.type != Token["Punctuator"]) {
       return false;
    }
    u16string op = res_u16(lookahead.value);
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
    if (source(idx) == 0x3B || match(u";")) { //! match(u"") what is this function?
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


bool isLeftHandSide(expr) { 
    return expr.type == Syntax["Identifier"] || expr.type == Syntax["MemberExpression"];
};


// 11.1.4 Array Initialiser

Node parseArrayInitialiser() {
    vector<Node> elements;
    Node node = new Node(), empty;
    empty.isNull = true;

    expect(u"[");

    while (!match(u"]")) {
        if (match(u",")) {
            lex();
            elements.push_back(empty); //! what type is elements? what should null val be?
        } else {
            elements.push_back(parseAssignmentExpression()); 
            if (!match(u"]")) {
                expect(u",");
            }
        }
    }

    lex();

    return node.finishArrayExpression(elements); //!
}

// 11.1.5 Object Initialiser

function parsePropertyFunction(param, TokenStruct first) { //!typeof param
    bool previousStrict;
    var body;
    node = new Node();

    previousStrict = strict;
    body = parseFunctionSourceElements(); //type body?
    if (!(first.isNull) && strict && isRestrictedWord(param[0].name)) {
        throwErrorTolerant(first, ["Messages.StrictParamName"],"");
    }
    strict = previousStrict;
    return node.finishFunctionExpression(null, param, [], body);
}

function parseObjectPropertyKey() {
    var token, node = new Node();

    token = lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token.type === Token.StringLiteral || token.type === Token.NumericLiteral) {
        if (strict && token.octal) {
            throwErrorTolerant(token, Messages.StrictOctalLiteral);
        }
        return node.finishLiteral(token);
    }

    return node.finishIdentifier(token.value);
}

function parseObjectProperty() {
    var token, key, id, value, param, node = new Node();

    token = lookahead;

    if (token.type === Token.Identifier) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token.value === 'get' && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            expect(u")");
            value = parsePropertyFunction([]);
            return node.finishProperty('get', key, value);
        }
        if (token.value === 'set' && !match(u":")) {
            key = parseObjectPropertyKey();
            expect(u"(");
            token = lookahead;
            if (token.type !== Token.Identifier) {
                expect(u")");
                throwErrorTolerant(token, Messages.UnexpectedToken, token.value);
                value = parsePropertyFunction([]);
            } else {
                param = [ parseVariableIdentifier() ];
                expect(u")");
                value = parsePropertyFunction(param, token);
            }
            return node.finishProperty('set', key, value);
        }
        expect(u":");
        value = parseAssignmentExpression();
        return node.finishProperty('init', id, value);
    }
    if (token.type === Token.EOF || token.type === Token.Punctuator) {
        throwUnexpected(token);
    } else {
        key = parseObjectPropertyKey();
        expect(u":");
        value = parseAssignmentExpression();
        return node.finishProperty('init', key, value);
    }
}

function parseObjectInitialiser() {
    var properties = [], token, property, name, key, kind, map = {}, toString = String, node = new Node();

    expect(u"{");

    while (!match(u"}")) {
        property = parseObjectProperty();

        if (property.key.type === Syntax.Identifier) {
            name = property.key.name;
        } else {
            name = toString(property.key.value);
        }
        kind = (property.kind === 'init') ? PropertyKind.Data : (property.kind === 'get') ? PropertyKind.Get : PropertyKind.Set;

        key = '$' + name;
        if (Object.prototype.hasOwnProperty.call(map, key)) {
            if (map[key] === PropertyKind.Data) {
                if (strict && kind === PropertyKind.Data) {
                    throwErrorTolerant({}, Messages.StrictDuplicateProperty);
                } else if (kind !== PropertyKind.Data) {
                    throwErrorTolerant({}, Messages.AccessorDataProperty);
                }
            } else {
                if (kind === PropertyKind.Data) {
                    throwErrorTolerant({}, Messages.AccessorDataProperty);
                } else if (map[key] & kind) {
                    throwErrorTolerant({}, Messages.AccessorGetSet);
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

    return node.finishObjectExpression(properties);
}

    // 11.1.6 The Grouping Operator

    function parseGroupExpression() {
        var expr;

        expect(u"(");

        if (match(u")")) {
            lex();
            return PlaceHolders.ArrowParameterPlaceHolder;
        }

        ++state.parenthesisCount;

        expr = parseExpression();

        expect(u")");

        return expr;
    }


    // 11.1 Primary Expressions

    function parsePrimaryExpression() {
        var type, token, expr, node;

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
        node = new Node();

        if (type === Token.Identifier) {
            expr =  node.finishIdentifier(lex().value);
        } else if (type === Token.StringLiteral || type === Token.NumericLiteral) {
            if (strict && lookahead.octal) {
                throwErrorTolerant(lookahead, Messages.StrictOctalLiteral);
            }
            expr = node.finishLiteral(lex());
        } else if (type === Token.Keyword) {
            if (matchKeyword('function')) {
                return parseFunctionExpression();
            }
            if (matchKeyword('this')) {
                lex();
                expr = node.finishThisExpression();
            } else {
                throwUnexpected(lex());
            }
        } else if (type === Token.BooleanLiteral) {
            token = lex();
            token.value = (token.value === 'true');
            expr = node.finishLiteral(token);
        } else if (type === Token.NullLiteral) {
            token = lex();
            token.value = null;
            expr = node.finishLiteral(token);
        } else if (match(u"/") || match(u"/=")) {
            if (typeof extra.tokens !== 'undefined') {
                expr = node.finishLiteral(collectRegex());
            } else {
                expr = node.finishLiteral(scanRegExp());
            }
            peek();
        } else {
            throwUnexpected(lex());
        }

        return expr;
    }

    // 11.2 Left-Hand-Side Expressions

    function parseArguments() {
        var args = [];

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

    function parseNonComputedProperty() {
        var token, node = new Node();

        token = lex();

        if (!isIdentifierName(token)) {
            throwUnexpected(token);
        }

        return node.finishIdentifier(token.value);
    }
!!
    function parseNonComputedMember() {
        expect(u".");

        return parseNonComputedProperty();
    }

    function parseComputedMember() {
        var expr;

        expect(u"[");

        expr = parseExpression();

        expect(u"]");

        return expr;
    }

    function parseNewExpression() {
        var callee, args, node = new Node();

        expectKeyword(u"new");
        callee = parseLeftHandSideExpression();
        args = match(u"(") ? parseArguments() : [];

        return node.finishNewExpression(callee, args);
    }

    function parseLeftHandSideExpressionAllowCall() {
        var expr, args, property, startToken, previousAllowIn = state.allowIn;

        startToken = lookahead;
        state.allowIn = true;
        expr = matchKeyword('new') ? parseNewExpression() : parsePrimaryExpression();

        for (;;) {
            if (match(u".")) {
                property = parseNonComputedMember();
                expr = new WrappingNode(startToken).finishMemberExpression('.', expr, property);
            } else if (match(u"(")) {
                args = parseArguments();
                expr = new WrappingNode(startToken).finishCallExpression(expr, args);
            } else if (match(u"[")) {
                property = parseComputedMember();
                expr = new WrappingNode(startToken).finishMemberExpression('[', expr, property);
            } else {
                break;
            }
        }
        state.allowIn = previousAllowIn;

        return expr;
    }

    function parseLeftHandSideExpression() {
        var expr, property, startToken;
        assert(state.allowIn, 'callee of new expression always allow in keyword.');

        startToken = lookahead;

        expr = matchKeyword('new') ? parseNewExpression() : parsePrimaryExpression();

        for (;;) {
            if (match(u"[")) {
                property = parseComputedMember();
                expr = new WrappingNode(startToken).finishMemberExpression('[', expr, property);
            } else if (match(u".")) {
                property = parseNonComputedMember();
                expr = new WrappingNode(startToken).finishMemberExpression('.', expr, property);
            } else {
                break;
            }
        }
        return expr;
    }

    // 11.3 Postfix Expressions

    function parsePostfixExpression() {
        var expr, token, startToken = lookahead;

        expr = parseLeftHandSideExpressionAllowCall();

        if (lookahead.type === Token.Punctuator) {
            if ((match(u"++") || match(u"--")) && !peekLineTerminator()) {
                // 11.3.1, 11.3.2
                if (strict && expr.type === Syntax.Identifier && isRestrictedWord(expr.name)) {
                    throwErrorTolerant({}, Messages.StrictLHSPostfix);
                }

                if (!isLeftHandSide(expr)) {
                    throwErrorTolerant({}, Messages.InvalidLHSInAssignment);
                }

                token = lex();
                expr = new WrappingNode(startToken).finishPostfixExpression(token.value, expr);
            }
        }

        return expr;
    }

    // 11.4 Unary Operators

    function parseUnaryExpression() {
        var token, expr, startToken;

        if (lookahead.type !== Token.Punctuator && lookahead.type !== Token.Keyword) {
            expr = parsePostfixExpression();
        } else if (match(u"++") || match(u"--")) {
            startToken = lookahead;
            token = lex();
            expr = parseUnaryExpression();
            // 11.4.4, 11.4.5
            if (strict && expr.type === Syntax.Identifier && isRestrictedWord(expr.name)) {
                throwErrorTolerant({}, Messages.StrictLHSPrefix);
            }

            if (!isLeftHandSide(expr)) {
                throwErrorTolerant({}, Messages.InvalidLHSInAssignment);
            }

            expr = new WrappingNode(startToken).finishUnaryExpression(token.value, expr);
        } else if (match(u"+") || match(u"-") || match(u"~") || match(u"!")) {
            startToken = lookahead;
            token = lex();
            expr = parseUnaryExpression();
            expr = new WrappingNode(startToken).finishUnaryExpression(token.value, expr);
        } else if (matchKeyword('delete') || matchKeyword('void') || matchKeyword('typeof')) {
            startToken = lookahead;
            token = lex();
            expr = parseUnaryExpression();
            expr = new WrappingNode(startToken).finishUnaryExpression(token.value, expr);
            if (strict && expr.operator === 'delete' && expr.argument.type === Syntax.Identifier) {
                throwErrorTolerant({}, Messages.StrictDelete);
            }
        } else {
            expr = parsePostfixExpression();
        }

        return expr;
    }

    function binaryPrecedence(token, allowIn) {
        var prec = 0;

        if (token.type !== Token.Punctuator && token.type !== Token.Keyword) {
            return 0;
        }

        switch (token.value) {
        case '||':
            prec = 1;
            break;

        case '&&':
            prec = 2;
            break;

        case '|':
            prec = 3;
            break;

        case '^':
            prec = 4;
            break;

        case '&':
            prec = 5;
            break;

        case '==':
        case '!=':
        case '===':
        case '!==':
            prec = 6;
            break;

        case '<':
        case '>':
        case '<=':
        case '>=':
        case 'instanceof':
            prec = 7;
            break;

        case 'in':
            prec = allowIn ? 7 : 0;
            break;

        case '<<':
        case '>>':
        case '>>>':
            prec = 8;
            break;

        case '+':
        case '-':
            prec = 9;
            break;

        case '*':
        case '/':
        case '%':
            prec = 11;
            break;

        default:
            break;
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
        var marker, markers, expr, token, prec, stack, right, operator, left, i;

        marker = lookahead;
        left = parseUnaryExpression();
        if (left === PlaceHolders.ArrowParameterPlaceHolder) {
            return left;
        }

        token = lookahead;
        prec = binaryPrecedence(token, state.allowIn);
        if (prec === 0) {
            return left;
        }
        token.prec = prec;
        lex();

        markers = [marker, lookahead];
        right = parseUnaryExpression();

        stack = [left, token, right];

        while ((prec = binaryPrecedence(lookahead, state.allowIn)) > 0) {

            // Reduce: make a binary expression from the three topmost entries.
            while ((stack.length > 2) && (prec <= stack[stack.length - 2].prec)) {
                right = stack.pop();
                operator = stack.pop().value;
                left = stack.pop();
                markers.pop();
                expr = new WrappingNode(markers[markers.length - 1]).finishBinaryExpression(operator, left, right);
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
            expr = new WrappingNode(markers.pop()).finishBinaryExpression(stack[i - 1].value, stack[i - 2], expr);
            i -= 2;
        }

        return expr;
    }


    // 11.12 Conditional Operator

    function parseConditionalExpression() {
        var expr, previousAllowIn, consequent, alternate, startToken;

        startToken = lookahead;

        expr = parseBinaryExpression();
        if (expr === PlaceHolders.ArrowParameterPlaceHolder) {
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

            expr = new WrappingNode(startToken).finishConditionalExpression(expr, consequent, alternate);
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

    function reinterpretAsCoverFormalsList(expressions) {
        var i, len, param, params, defaults, defaultCount, options, rest;

        params = [];
        defaults = [];
        defaultCount = 0;
        rest = null;
        options = {
            paramSet: {}
        };

        for (i = 0, len = expressions.length; i < len; i += 1) {
            param = expressions[i];
            if (param.type === Syntax.Identifier) {
                params.push(param);
                defaults.push(null);
                validateParam(options, param, param.name);
            } else if (param.type === Syntax.AssignmentExpression) {
                params.push(param.left);
                defaults.push(param.right);
                ++defaultCount;
                validateParam(options, param.left, param.left.name);
            } else {
                return null;
            }
        }

        if (options.message === Messages.StrictParamDupe) {
            throwError(
                strict ? options.stricted : options.firstRestricted,
                options.message
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

 Node parseArrowFunctionExpression(options, node) {
    var previousStrict, body;

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

    return node.finishArrowFunctionExpression(options.params, options.defaults, body, body.type !== Syntax.BlockStatement);
}

    // 11.13 Assignment Operators

    function parseAssignmentExpression() {
        var oldParenthesisCount, token, expr, right, list, startToken;

        oldParenthesisCount = state.parenthesisCount;

        startToken = lookahead;
        token = lookahead;

        expr = parseConditionalExpression();

        if (expr === PlaceHolders.ArrowParameterPlaceHolder || match(u"=>")) {
            if (state.parenthesisCount === oldParenthesisCount ||
                    state.parenthesisCount === (oldParenthesisCount + 1)) {
                if (expr.type === Syntax.Identifier) {
                    list = reinterpretAsCoverFormalsList([ expr ]);
                } else if (expr.type === Syntax.AssignmentExpression) {
                    list = reinterpretAsCoverFormalsList([ expr ]);
                } else if (expr.type === Syntax.SequenceExpression) {
                    list = reinterpretAsCoverFormalsList(expr.expressions);
                } else if (expr === PlaceHolders.ArrowParameterPlaceHolder) {
                    list = reinterpretAsCoverFormalsList([]);
                }
                if (list) {
                    return parseArrowFunctionExpression(list, new WrappingNode(startToken));
                }
            }
        }

        if (matchAssign()) {
            // LeftHandSideExpression
            if (!isLeftHandSide(expr)) {
                throwErrorTolerant({}, Messages.InvalidLHSInAssignment);
            }

            // 11.13.1
            if (strict && expr.type === Syntax.Identifier && isRestrictedWord(expr.name)) {
                throwErrorTolerant(token, Messages.StrictLHSAssignment);
            }

            token = lex();
            right = parseAssignmentExpression();
            expr = new WrappingNode(startToken).finishAssignmentExpression(token.value, expr, right);
        }

        return expr;
    }

    // 11.14 Comma Operator

    function parseExpression() {
        var expr, startToken = lookahead, expressions;

        expr = parseAssignmentExpression();

        if (match(u",")) {
            expressions = [expr];

            while (idx < length) {
                if (!match(u",")) {
                    break;
                }
                lex();
                expressions.push(parseAssignmentExpression());
            }

            expr = new WrappingNode(startToken).finishSequenceExpression(expressions);
        }

        return expr;
    }

    // 12.1 Block

    function parseStatementList() {
        var list = [],
            statement;

        while (idx < length) {
            if (match(u"}")) {
                break;
            }
            statement = parseSourceElement();
            if (typeof statement === 'undefined') {
                break;
            }
            list.push(statement);
        }

        return list;
    }

    function parseBlock() {
        var block, node = new Node();

        expect(u"{");

        block = parseStatementList();

        expect(u"}");

        return node.finishBlockStatement(block);
    }


// 12.2 Variable Statement

//#clear
Node parseVariableIdentifier() {
    TokenStruct token;
    Node node = new Node();

    token = lex();

    if (token.type != Token["Identifier"]) {
        throwUnexpected(token);
    }

    return node.finishIdentifier(token.value);
}

    function parseVariableDeclaration(kind) {
        var init = null, id, node = new Node();

        id = parseVariableIdentifier();

        // 12.2.1
        if (strict && isRestrictedWord(id.name)) {
            throwErrorTolerant({}, Messages.StrictVarName);
        }

        if (kind === 'const') {
            expect(u"=");
            init = parseAssignmentExpression();
        } else if (match(u"=")) {
            lex();
            init = parseAssignmentExpression();
        }

        return node.finishVariableDeclarator(id, init);
    }

//#partial
Vector< abcdef > parseVariableDeclarationList(kind) {
    vector<> list; //type parseVariableDeclaration

    do {
        list.push_back(parseVariableDeclaration(kind));
        if (!match(u",")) {
            break;
        }
        lex();
    } while (idx < length);

    return list;
}

//#partial
Node parseVariableStatement(Node node) {
    vector< abcdef > declarations;

    expectKeyword(u"var");

    declarations = parseVariableDeclarationList(); //! no arg provided but expects arg

    consumeSemicolon();

    return node.finishVariableDeclaration(declarations, 'var');
}

    // kind may be `const` or `let`
    // Both are experimental and not in the specification yet.
    // see http://wiki.ecmascript.org/doku.php?id=harmony:const
    // and http://wiki.ecmascript.org/doku.php?id=harmony:let
    function parseConstLetDeclaration(kind) {
        var declarations, node = new Node();

        expectKeyword(kind);

        declarations = parseVariableDeclarationList(kind);

        consumeSemicolon();

        return node.finishVariableDeclaration(declarations, kind);
    }

// 12.3 Empty Statement

//#clean
Node parseEmptyStatement() {
    var node = new Node();
    expect(u";");
    return node.finishEmptyStatement();
}

// 12.4 Expression Statement
//#clean
Node parseExpressionStatement(Node node) {
    var expr = parseExpression();
    consumeSemicolon();
    return node.finishExpressionStatement(expr);
}

// 12.5 If statement
//#partial
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
        alternate = NULLTOKEN; //!account for this in finishIfStatetment
    }

    return node.finishIfStatement(test, consequent, alternate);
}

    // 12.6 Iteration Statements

    function parseDoWhileStatement(node) {
        var body, test, oldInIteration;

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

        return node.finishDoWhileStatement(body, test);
    }

    function parseWhileStatement(node) {
        var test, body, oldInIteration;

        expectKeyword(u"while");

        expect(u"(");

        test = parseExpression();

        expect(u")");

        oldInIteration = state.inIteration;
        state.inIteration = true;

        body = parseStatement();

        state.inIteration = oldInIteration;

        return node.finishWhileStatement(test, body);
    }

    function parseForVariableDeclaration() {
        var token, declarations, node = new Node();

        token = lex();
        declarations = parseVariableDeclarationList();

        return node.finishVariableDeclaration(declarations, token.value);
    }

    function parseForStatement(node) {
        var init, test, update, left, right, body, oldInIteration, previousAllowIn = state.allowIn;

        init = test = update = null;

        expectKeyword(u"for");

        expect(u"(");

        if (match(u";")) {
            lex();
        } else {
            if (matchKeyword('var') || matchKeyword('let')) {
                state.allowIn = false;
                init = parseForVariableDeclaration();
                state.allowIn = previousAllowIn;

                if (init.declarations.length === 1 && matchKeyword('in')) {
                    lex();
                    left = init;
                    right = parseExpression();
                    init = null;
                }
            } else {
                state.allowIn = false;
                init = parseExpression();
                state.allowIn = previousAllowIn;

                if (matchKeyword('in')) {
                    // LeftHandSideExpression
                    if (!isLeftHandSide(init)) {
                        throwErrorTolerant({}, Messages.InvalidLHSInForIn);
                    }

                    lex();
                    left = init;
                    right = parseExpression();
                    init = null;
                }
            }

            if (typeof left === 'undefined') {
                expect(u";");
            }
        }

        if (typeof left === 'undefined') {

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

        return (typeof left === 'undefined') ?
                node.finishForStatement(init, test, update, body) :
                node.finishForInStatement(left, right, body);
    }

    // 12.7 The continue statement

    function parseContinueStatement(node) {
        var label = null, key;

        expectKeyword(u"continue");

        // Optimize the most common form: 'continue;'.
        if (source.charCodeAt(idx) === 0x3B) {
            lex();

            if (!state.inIteration) {
                throwError({}, Messages.IllegalContinue);
            }

            return node.finishContinueStatement(null);
        }

        if (peekLineTerminator()) {
            if (!state.inIteration) {
                throwError({}, Messages.IllegalContinue);
            }

            return node.finishContinueStatement(null);
        }

        if (lookahead.type === Token.Identifier) {
            label = parseVariableIdentifier();

            key = '$' + label.name;
            if (!Object.prototype.hasOwnProperty.call(state.labelSet, key)) {
                throwError({}, Messages.UnknownLabel, label.name);
            }
        }

        consumeSemicolon();

        if (label === null && !state.inIteration) {
            throwError({}, Messages.IllegalContinue);
        }

        return node.finishContinueStatement(label);
    }

    // 12.8 The break statement

    function parseBreakStatement(node) {
        var label = null, key;

        expectKeyword(u"break");

        // Catch the very common case first: immediately a semicolon (U+003B).
        if (source.charCodeAt(idx) === 0x3B) {
            lex();

            if (!(state.inIteration || state.inSwitch)) {
                throwError({}, Messages.IllegalBreak);
            }

            return node.finishBreakStatement(null);
        }

        if (peekLineTerminator()) {
            if (!(state.inIteration || state.inSwitch)) {
                throwError({}, Messages.IllegalBreak);
            }

            return node.finishBreakStatement(null);
        }

        if (lookahead.type === Token.Identifier) {
            label = parseVariableIdentifier();

            key = '$' + label.name;
            if (!Object.prototype.hasOwnProperty.call(state.labelSet, key)) {
                throwError({}, Messages.UnknownLabel, label.name);
            }
        }

        consumeSemicolon();

        if (label === null && !(state.inIteration || state.inSwitch)) {
            throwError({}, Messages.IllegalBreak);
        }

        return node.finishBreakStatement(label);
    }

    // 12.9 The return statement

    function parseReturnStatement(node) {
        var argument = null;

        expectKeyword(u"return");

        if (!state.inFunctionBody) {
            throwErrorTolerant({}, Messages.IllegalReturn);
        }

        // 'return' followed by a space and an identifier is very common.
        if (source.charCodeAt(idx) === 0x20) {
            if (isIdentifierStart(source.charCodeAt(idx + 1))) {
                argument = parseExpression();
                consumeSemicolon();
                return node.finishReturnStatement(argument);
            }
        }

        if (peekLineTerminator()) {
            return node.finishReturnStatement(null);
        }

        if (!match(u";")) {
            if (!match(u"}") && lookahead.type !== Token.EOF) {
                argument = parseExpression();
            }
        }

        consumeSemicolon();

        return node.finishReturnStatement(argument);
    }

    // 12.10 The with statement

    function parseWithStatement(node) {
        var object, body;

        if (strict) {
            // TODO(ikarienator): Should we update the test cases instead?
            skipComment();
            throwErrorTolerant({}, Messages.StrictModeWith);
        }

        expectKeyword(u"with");

        expect(u"(");

        object = parseExpression();

        expect(u")");

        body = parseStatement();

        return node.finishWithStatement(object, body);
    }

    // 12.10 The swith statement

    function parseSwitchCase() {
        var test, consequent = [], statement, node = new Node();

        if (matchKeyword(u"default")) {
            lex();
            test = null;
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
            consequent.push(statement);
        }

        return node.finishSwitchCase(test, consequent);
    }

Node parseSwitchStatement(Node node) {
    var discriminant; 
    vector<> cases; 
    var clause;
    bool oldInSwitch, defaultFound;

    expectKeyword(u"switch");

    expect(u"(");

    discriminant = parseExpression();

    expect(u")");

    expect(u"{");

    cases = [];

    if (match(u"}")) {
        lex();
        return node.finishSwitchStatement(discriminant, cases);
    }

    oldInSwitch = state.inSwitch;
    state.inSwitch = true;
    defaultFound = false;

    while (idx < length) {
        if (match(u"}")) {
            break;
        }
        clause = parseSwitchCase(); //!ret?
        if (clause.test === null) {
            if (defaultFound) {
                throwError(NULLTOKEN, Messages["MultipleDefaultsInSwitch"],"");
            }
            defaultFound = true;
        }
        cases.push(clause);
    }

    state.inSwitch = oldInSwitch;

    expect(u"}");

    return node.finishSwitchStatement(discriminant, cases);
}

// 12.13 The throw statement
//#partial
Node parseThrowStatement(Node node) {
    var argument; //!return

    expectKeyword(u"throw");

    if (peekLineTerminator()) {
        throwError(NULLTOKEN, Messages["NewlineAfterThrow"],"");
    }

    argument = parseExpression();

    consumeSemicolon();

    return node.finishThrowStatement(argument);
}

// 12.14 The try statement

//#partial
Node parseCatchClause() {
    var body;
    Node param, node = new Node();

    expectKeyword(u"catch");

    expect(u"(");
    if (match(u")")) {
        throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (strict && isRestrictedWord(param.name)) { //! param.name ???
        throwErrorTolerant(NULLTOKEN, Messages["StrictCatchVariable"]);
    }

    expect(u")");
    body = parseBlock(); //ret
    return node.finishCatchClause(param, body);
}

//#partial
Node parseTryStatement(Node node) {
    var block, finalizer; //! type (parseBlock)
    vector<> handlers; //!type (parseCatchClause)

    finalizer = null; //!

    expectKeyword(u"try");

    block = parseBlock();

    if (matchKeyword(u"catch")) {
        handlers.push(parseCatchClause());
    }

    if (matchKeyword(u"finally")) {
        lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && !finalizer) {
        throwError(NULLTOKEN, Messages["NoCatchOrFinally"], "");
    }

    return node.finishTryStatement(block, [], handlers, finalizer); //! call includes a dynamic aray.
}

// 12.15 The debugger statement

Node parseDebuggerStatement(Node node) {
    expectKeyword(u"debugger");

    consumeSemicolon();

    return node.finishDebuggerStatement();
}

// 12 Statements

//#partial
Node parseStatement() {
    int type = lookahead.type;
    var   expr,
        labeledBody,
        key;
    u16string tokval;
    Node node;

    if (type == Token["EOF"]) {
        throwUnexpected(lookahead);
    }

    if (type == Token["Punctuator"] && res_u16(lookahead.value) == u"{") {
        return parseBlock();
    }

    node = new Node();

    if (type == Token["Punctuator"]) {
        tokval = res_u16(lookahead.value);
        if (tokval == u";") {
            return parseEmptyStatement(node);
        } else if (tokval == u"(") {
            return parseExpressionStatement(node);
        }
    } else if (type == Token["Keyword"]) {
        tokval = res_u16(lookahead.value);
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

    expr = parseExpression(); //! type?

    // 12.12 Labelled Statements
    if ((expr.type === Syntax.Identifier) && match(u":")) {
        lex();

        key = '$' + expr.name;
        if (has(key, state.labelSet)) {
            throwError({}, Messages["Redeclaration"], 'Label', expr.name); //! pretty odd call here. 4 args?
        }

        state.labelSet[key] = true;
        labeledBody = parseStatement();
        delete state.labelSet[key];
        return node.finishLabeledStatement(expr, labeledBody);
    }

    consumeSemicolon();

    return node.finishExpressionStatement(expr);
}

// 13 Function Definition
//#partial
Node parseFunctionSourceElements() {
    Node sourceElement;
    vector< Node > sourceElements;
    TokenStruct token, firstRestricted;
    u16string directive,
        oldLabelSet, oldInIteration, oldInSwitch, oldInFunctionBody, oldParenthesisCount;
    Node node = new Node();
    StateStruct oldstate;

    expect(u"{");

    firstRestricted.isNull = true;
    while (idx < length) {
        if (lookahead.type != Token["StringLiteral"]) {
            break;
        }
        token = lookahead;

        sourceElement = parseSourceElement(); 
        sourceElements.push_back(sourceElement); 
        if (sourceElement.expression.type != Syntax["Literal"]) { //.expression.type
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

    oldstate.labelSet = state.labelSet;
    oldstate.inIteration = state.inIteration;
    oldstate.inSwitch = state.inSwitch;
    oldstate.inFunctionBody = state.inFunctionBody;
    oldstate.parenthesisCount = state.parenthesizedCount;

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

    return node.finishBlockStatement(sourceElements);
}

//#clear
struct ParseParamOpts {
    vector< Node > params;
    int defaultCount;
    vector< Node > defaults;
    TokenStruct firstRestricted;
    TokenStruct stricted;
    unordered_set<u16string> paramSet;
    u16string message;
};

//#clear
function validateParam(ParseParamOpts options, TokenStruct param, u16string name) {
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

//#clear
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
    validateParam(options, token, res_u16(token.value));
    if (match(u"=")) {
        lex();
        def = parseAssignmentExpression();
        ++(options.defaultCount);
    }

    options.params.push_back(param);
    options.defaults.push_back(def);

    return !match(u")");
}

//#clear
struct ParseParamsOut {
    TokenStruct firstRestricted;
    TokenStruct stricted;
    u16string message;
    vector< Node > params;
    vector< Node > defaults;
};

//#clear
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
    Node node = new Node();
    
    expectKeyword(u"function");
 
    token = lookahead;
    id = parseVariableIdentifier(); 
    tokval = res_u16(token.value);

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

    return node.finishFunctionDeclaration(id, params, defaults, body);
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
    Node node = new Node();
    
    expectKeyword(u"function");
    
    if (!match(u"(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 
        tokval = res_u16(token.value);
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

    return node.finishFunctionExpression(id, params, defaults, body);
}

    // 14 Program

//#PArtial
Node parseSourceElement() { //ret sig also rets of calls w/in
    u16string val;
    if (lookahead.type == Token["Keyword"]) {
        val = res_u16(lookahead_value);
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

//#PARTIAL
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
        if (sourceElement.expression.type != Syntax.Literal) { 
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
    node = new Node();
    strict = false;

    body = parseSourceElements();
    return node.finishProgram(body);
}

//#CLEAR
void filterTokenLocation() {
    int i;
    TokenStruct token, entry;
    vector<TokenStruct> tokens;

    for (i = 0; i < extra.tokens.size(); ++i) {
        entry = extra.tokens[i];
        token.type = entry.type;
        token.value = entry.value;
        if (extra.range) { 
            token.range[0] = entry.range[0];
            token.range[1] = entry.range[1];
        }
        if (extra.loc) { 
            token.loc = entry.loc;
        }
        tokens.push_back(token);
    }
    extra.tokens = tokens;
}

vector<TokenStruct> tokenize(code, options) {
        vector<TokenStruct> tokens;

    //! do this in outer javascript before call. 
    //! as it's a js-environment specific problem.

    //if (typeof code !== 'string' && !(code instanceof String)) {
    //    code = toString(code);
    //}

    source = code;
    idx = 0;
    lineNumber = (source.length > 0) ? 1 : 0;
    lineStart = 0;
    length = source.length;
    lookahead = null;
    state.allowIn = true;
    state.inFunctionBody = false;
    state.inIteration = false;
    state.inSwitch = false;
    state.lastCommentStart = -1
    //! parenthesisCount for state not provided here normally as in parse. 
    //! That going to be a problem for us later?

    // Of course we collect tokens here.
    options["tokens"] = true;
    extra.tokenTracking = true; 
    extra.tokenize = true;
    // The following two fields are necessary to compute the Regex tokens.
    extra.openParenToken = -1;
    extra.openCurlyToken = -1;

    extra.range = hasStringKey("range", options) && options["range"] == "true";
    extra.loc = hasStringKey("loc", options) && options["loc"] == "true";

    extra.commentTracking = hasStringKey("comment", options) && options["comment"] == "true";
    extra.errorTolerant = hasStringKey("tolerant", options) && options["tolerant"] == "true";

    try {
        peek();
        if (lookahead.type == Token["EOF"]) {
            return extra.tokens;
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
        tokens = extra.tokens;
        if (extra.commentTracking) {
            tokens.comments = extra.comments; //! how to pass this and errors back in a way
            // that strikes a balance between idiomatic c++ lib use, sane translation to javascript
            // in asm.js, and fitting within library style? 
        }
        if (extra.errorTolerant) {
            tokens.errors = extra.errors; //!
        }
    } catch (e) {
        throw e;
    } finally {
    }
    return tokens;
}


function parse(string code) {
    parse(code, {});
}
function parse(string code, Map<string, string> options) {
    parse(toU16string(code), options);
}
function parse(u16string code) {
    parse(code, {});
}

function parse(u16string code, Map<string, string> options) {
    var program, toString;

    toString = String;

    //! do this in outer javascript before call. 
    //! as it's a js-environment specific problem.

    //if (typeof code !== 'string' && !(code instanceof String)) { 
    //    code = toString(code);
    //}

    source = code;
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
    state.lastCommentStart = -1
    
    extra.range = hasStringKey("range", options) && options["range"] == "true";
    extra.loc = hasStringKey("loc", options) && options["loc"] == "true";
    extra.attachComment = hasStringKey("attachComment", options) && options["attachComment"] == "true"; 

    if (extra.loc && hasStringKey("source", options)) {
        extra.source = options.source;
    } else { extra.source = ""; } //! is using "" as a null value here going to be alright? check.

    extra.tokenTracking = hasStringKey("tokens", options) && options["tokens"] == "true";
    extra.commentTracking = hasStringKey("comment", options) && options["comment"] == "true";
    extra.errorTolerant = hasStringKey("tolerant", options) && options["tolerant"] == "true";

    //values which aren't strictly dependent on attachComment being true
    //but attachComment is sufficient for them to be true.
    if (extra.attachComment) {
        extra.range = true;
        extra.commentTracking = true;
    }


    try {
        program = parseProgram();
        if (extra.commentTracking) {
            program.comments = extra.comments; 
            //! how to pass comments and modules back idomatically to library, c++, asm.js module output?
        }
        if (extra.tokenTracking) {
            filterTokenLocation();
            program.tokens = extra.tokens;
        }
        if (extra.errorTolerant) {
            program.errors = extra.errors;
        }
    } catch (e) {
        throw e;
    } finally {
        extra = {};
    }

    return program;
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
            if (Syntax.hasOwnProperty(name)) {
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
