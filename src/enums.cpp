#line 1 "enums.cpp"
#include "enums.hpp"

std::map<TknType, const StrRef*> TokenName = {
    {TknType::BooleanLiteral, (&(text::_Boolean))},
    {TknType::EOFF, (&(text::_ZendZ))},
    {TknType::Identifier, (&(text::_Identifier))},
    {TknType::Keyword, (&(text::_Keyword))},
    {TknType::NullLiteral, (&(text::_Null))},
    {TknType::NumericLiteral, (&(text::_Numeric))},
    {TknType::Punctuator, (&(text::_Punctuator))},
    {TknType::StringLiteral, (&(text::_String))},
    {TknType::RegularExpression, (&(text::_RegularExpression))}
};

std::map<std::string, int> PropertyKind = {
    {"Data", 1},
    {"Get", 2},
    {"Set", 4}
};

std::map<std::string, int> LiteralType = {
    {"String", 1},
    {"Int", 2},
    {"Double", 3},
    {"Regexp", 4},
    {"Bool", 5},
    {"Null", 6}
};

std::map<Mssg, std::string> Messages = {
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

std::map<Synt, const StrRef *> Syntax = {
    {Synt::AssignmentExpression, (&(text::_AssignmentExpression))},
    {Synt::ArrayExpression, (&(text::_ArrayExpression))},
    {Synt::ArrowFunctionExpression, (&(text::_ArrowFunctionExpression))},
    {Synt::BlockStatement, (&(text::_BlockStatement))},
    {Synt::BinaryExpression, (&(text::_BinaryExpression))},
    {Synt::BreakStatement, (&(text::_BreakStatement))},
    {Synt::CallExpression, (&(text::_CallExpression))},
    {Synt::CatchClause, (&(text::_CatchClause))},
    {Synt::ConditionalExpression, (&(text::_ConditionalExpression))},
    {Synt::ContinueStatement, (&(text::_ContinueStatement))},
    {Synt::DoWhileStatement, (&(text::_DoWhileStatement))},
    {Synt::DebuggerStatement, (&(text::_DebuggerStatement))},
    {Synt::EmptyStatement, (&(text::_EmptyStatement))},
    {Synt::ExpressionStatement, (&(text::_ExpressionStatement))},
    {Synt::ForStatement, (&(text::_ForStatement))},
    {Synt::ForInStatement, (&(text::_ForInStatement))},
    {Synt::FunctionDeclaration, (&(text::_FunctionDeclaration))},
    {Synt::FunctionExpression, (&(text::_FunctionExpression))},
    {Synt::Identifier, (&(text::_Identifier))},
    {Synt::IfStatement, (&(text::_IfStatement))},
    {Synt::Literal, (&(text::_Literal))},
    {Synt::LabeledStatement, (&(text::_LabeledStatement))},
    {Synt::LogicalExpression, (&(text::_LogicalExpression))},
    {Synt::MemberExpression, (&(text::_MemberExpression))},
    {Synt::NewExpression, (&(text::_NewExpression))},
    {Synt::ObjectExpression, (&(text::_ObjectExpression))},
    {Synt::Program, (&(text::_Program))},
    {Synt::Property, (&(text::_Property))},
    {Synt::ReturnStatement, (&(text::_ReturnStatement))},
    {Synt::SequenceExpression, (&(text::_SequenceExpression))},
    {Synt::SwitchStatement, (&(text::_SwitchStatement))},
    {Synt::SwitchCase, (&(text::_SwitchCase))},
    {Synt::ThisExpression, (&(text::_ThisExpression))},
    {Synt::ThrowStatement, (&(text::_ThrowStatement))},
    {Synt::TryStatement, (&(text::_TryStatement))},
    {Synt::UnaryExpression, (&(text::_UnaryExpression))},
    {Synt::UpdateExpression, (&(text::_UpdateExpression))},
    {Synt::VariableDeclaration, (&(text::_VariableDeclaration))},
    {Synt::VariableDeclarator, (&(text::_VariableDeclarator))},
    {Synt::WhileStatement, (&(text::_WhileStatement))},
    {Synt::WithStatement, (&(text::_WithStatement))}
}; 
