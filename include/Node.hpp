#ifndef NODE_HPP
#define NODE_HPP
#line 2 "Node.h"

//#include "NodesComments.h"
//#include "LinprimaTask.h"
class NodesComments;
struct LinprimaTask;

class Node {
public:
    const StrRef *type;

    bool hasJv;
    Document jv;
    Loc loc;
#ifndef THROWABLE
    bool err;
#endif
    bool hasLoc;
    bool hasRange;
    int range[2];
    vector< vector<RegexLeg> > regexPaths; 

    shared_ptr<NodesComments> thisnc;

    //todo consolidate these rather than wastefully store each of them
    //for each node
    string name;//for identifiers
    vector< Node* > expressions; //for sequence expressions.
    Node* leftAssign; //for assignment+reinterpretAsCover...
    Node* rightAssign; //same

    //for finishExpressionStatement's child node's type
    //used in parseFunctionSourceElements and parseSourceElements
    const StrRef *spareStrref; 

    AllocatorType* alloc;
    vector<Node*>* heapNodes;
    size_t completedPos; //for late resolves

    string s(const u16string in);
    Node(bool lookaheadAvail, bool storeStats, 
         vector<Node*>* heapNodes,
         AllocatorType *alloc,
         LinprimaTask *task);
    void lookavailInit();
    void clear();
    void unused();
    void jvput(const StrRef path, const string b);
    void jvput(const StrRef path, const StrRef &b);
    void jvput(const StrRef path, const int b); 
    void jvput(const StrRef path, const bool b);
    void jvput_dbl(const StrRef path, const double b);
    void jvput_null(const StrRef path); 
#ifdef LIMITJSON
    size_t addUnresolvedDocument(const StrRef &path);
    size_t pushUnresolvedDocument(Value &root);
    void lateResolve();
#endif
    void regNoadd(const vector<RegexLeg> paths, 
                  Node * child);
    void reg(const StrRef& path, 
             Node * child);
    void nodeVec(const StrRef& path, 
                 const vector<Node*>& nodes);
    void addType(const Synt in);
    void regexPaths2json(Value& out,
                         AllocatorType * alloc);
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
    void finishProperty(const StrRef &kind, 
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
                                   const StrRef &kind);
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
#endif
