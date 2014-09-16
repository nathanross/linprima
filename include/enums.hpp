#ifndef ENUMS_HPP
#define ENUMS_HPP

#line 4 "enums.hpp"
#include "texpconsts.hpp"
#include "t52types.hpp"
#include <map>

//typedef GenericStringRef<char> StrRef

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

extern std::map<TknType, const fixedstr::SFixedStr*> TokenName;

extern std::map<std::string, int> PropertyKind;

 //#C++ specific type specifier
extern std::map<std::string, int> LiteralType;

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
/*
*/

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

extern std::map<Mssg, std::string> Messages;

extern std::map<Synt, const fixedstr::SFixedStr *> Syntax;

#endif

