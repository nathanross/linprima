#ifndef PARSE_FUNCS_HPP
#define PARSE_FUNCS_HPP

#line 4 "ParseFuncs.hpp"

#include "Tokenizer.hpp"
#include "LinprimaTask.hpp"
#include "parsepod.hpp"
#include "WrappingNode.hpp"
#include "Node.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include "fixedstr.hpp"
#include "wojson.hpp"
#include "t52types.hpp"
#include <rapidjson/document.h>
#include <string>
#include <vector>
#include <memory>

class ParseTools {
public:
    ParseTools(std::u16string  code,
               OptionsStruct options);
    ~ParseTools();
    //#throw_begin
    Node* parseProgram();
    //#throw_end
    void parse(wojson::WojsonDocument &doc,
               const bool retErrAsJson);
private:    
    wojson::WojsonDocument * doc;

std::shared_ptr<LinprimaTask> task;
    const char16_t * sourceRaw;
    const int length;
    int& idx;
    int& lineNumber;
    int& lineStart;
    ExtraStruct &extra;
    StateStruct &state;
    ptrTkn &lookahead;
    Tokenizer scanner;
    std::vector<Node*> heapNodes;

    Node * makeNode(bool lookavailInit, bool exists);
    WrappingNode * makeWrappingNode(ptrTkn token);
    WrappingNode * makeWrappingNode();

    char16_t source(long pos);


    void clearNodeHeap();
    //#throw_begin
    bool peekLineTerminator();
    void expect(const std::string value);
    void expectTolerant(const std::string value);
    void expectKeyword(const std::string keyword);
    //#throw_end
    bool match(const std::string value);
    bool matchKeyword(const std::string keyword);
    bool matchAssign();
    //#throw_begin
    void consumeSemicolon();
    //#throw_end
    static bool isLeftHandSide(Node* expr);

    //#throw_begin
    Node* parseArrayInitialiser();
    Node* parsePropertyFunction(std::vector<Node*>& param, ptrTkn first);
    Node* parseObjectPropertyKey();
    Node* parseObjectProperty();
    Node* parseObjectInitialiser();
    Node* parseGroupExpression();
    Node* parsePrimaryExpression();
    void parseArguments(Node *parent);
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
                           Node *param, const std::string name);
    //#throw_begin
    ReinterpretOut reinterpretAsCoverFormalsList(std::vector< Node* >& expressions);
    Node* parseArrowFunctionExpression(const ReinterpretOut options, 
                                   Node *node);
    Node* parseAssignmentExpression();
    Node* parseExpression();
    //std::vector< Node* > parseStatementList();
    Node* parseBlock();
    Node* parseVariableIdentifier();
    Node* parseVariableDeclaration(const fixedstr::SFixedStr &kind);
    void parseVariableDeclarationList(const fixedstr::SFixedStr &kind, 
                                      Node *parent);
    Node* parseVariableStatement(Node* node);
    Node* parseConstLetDeclaration(const std::string kind, 
                                   const fixedstr::SFixedStr &kindref);
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
                       ptrTkn param, const std::string name);
    bool parseParam(ParseParamsOptions& options);
    ParseParamsOut parseParams(ptrTkn firstRestricted);
    Node* parseFunctionDeclaration();
    Node* parseFunctionExpression();
    Node* parseSourceElement();
    //std::vector< Node* > parseSourceElements();
    bool parseSourceElements(Node * parent);
    //#throw_end
};
#endif
