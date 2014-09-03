#ifndef PARSE_FUNCS_HPP
#define PARSE_FUNCS_HPP
#line 1  "ParseFuncs.h"
class ParseTools {
public:
    ParseTools(u16string code,
               OptionsStruct options);
    ~ParseTools();
    //#throw_begin
    Node* parseProgram();
    //#throw_end
    void parse(Document& out, 
#ifdef LIMITJSON
               vector<string> & completedObjectsOut,
#endif
               const bool retErrAsJson);
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
    Node* parseVariableDeclaration(const StrRef &kind);
    vector< Node* > parseVariableDeclarationList(const StrRef &kind);
    Node* parseVariableStatement(Node* node);
    Node* parseConstLetDeclaration(const string kind, 
                                   const StrRef &kindref);
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
#endif
