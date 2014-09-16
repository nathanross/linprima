#ifndef NODE_HPP
#define NODE_HPP

#line 4 "Node.hpp"
#include "LinprimaTask.hpp"
#include "NodesComments.hpp"
#include "strref.hpp"
#include "enums.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include "wojson.hpp"
#include "fixedstr.hpp"
#include <string>
#include <vector>
#include <memory>

class Node {
public:
    const fixedstr::SFixedStr *type;

    bool hasJv;
    wojson::WojsonMap *jv;
    Loc loc;
#ifndef THROWABLE
    bool err;
#endif
    bool hasLoc;
    bool hasRange;
    int range[2];
    std::vector< std::vector<RegexLeg> > regexPaths; 

    std::shared_ptr<NodesComments> thisnc;


    //todo consolidate these rather than wastefully store each of them
    //for each node
    std::string name;//for identifiers
    std::vector< Node* > expressions; //for sequence expressions.
    Node* leftAssign; //for assignment+reinterpretAsCover...
    Node* rightAssign; //same

    //for finishExpressionStatement's child node's type
    //used in parseFunctionSourceElements and parseSourceElements
    const fixedstr::SFixedStr *spareStrref; 

    wojson::WojsonDocument* doc;
    std::vector<Node*>* heapNodes;

    size_t completedPos; 
    //for late resolves
    //SEMANTIC OVERRIDE : for program completedPos signals # of children > 0


    std::string s(const std::u16string in);
    Node(bool lookaheadAvail, bool storeStats, 
         std::vector<Node*>* heapNodes,
         wojson::WojsonDocument *doc,
         LinprimaTask *task);
    void lookavailInit();
    void clear();
    void unused();


    size_t addUnresolvedDocument(const fixedstr::FixedStr &path);
    size_t pushUnresolvedDocument(rapidjson::Value &root);
    void lateResolve();

    void regNoadd(const std::vector<RegexLeg> paths, 
                  Node * child);
    void reg(const fixedstr::SFixedStr& path, 
             Node * child);
    rapidjson::Value* initVec(const fixedstr::SFixedStr &path);
    void regPush(wojson::WojsonArr *arr, fixedstr::SFixedStr &path,
                 int &ctr, Node* child);
    void nodeVec(const fixedstr::SFixedStr& path, 
                 const std::vector<Node*>& nodes);
    void initJV(const Synt in);
    void regexPaths2json(wojson::WojsonArr &parent,
                         wojson::WojsonDocument * doc);
    //void commentsIntoJson(const bool leading);
    void processComment();
    void finish();
    void finishArrayExpression(const std::vector< Node* >& elements);
    void finishArrowFunctionExpression(const std::vector< Node* >& params, 
                                       const std::vector< Node* >& defaults, 
                                       Node * body, 
                                       const bool expression);
    void finishAssignmentExpression(const std::string oper,
                                    Node *left, 
                                    Node *right);
    void finishBinaryExpression(const std::string oper, 
                                Node *left, 
                                Node *right);
    void finishBlockStatement(const std::vector< Node* >& body);
    void finishBreakStatement(Node * label);
    void finishCallExpression(Node * callee, 
                              const std::vector< Node* >& args);
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
                                   const std::vector< Node* >& params, 
                                   const std::vector< Node* >& defaults, 
                                   Node * body);
    void finishFunctionExpression(Node * id, 
                                  const std::vector< Node* >& params, 
                                  const std::vector< Node* >& defaults, 
                                  Node * body);
    void finishIdentifier(const std::string name);
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
                             const std::vector<Node*>& args);
    void finishObjectExpression(const std::vector<Node*>& properties);
    void finishPostfixExpression(const std::string oper, 
                                 Node * argument);
    void finishProgram(const std::vector<Node*>& body);
    void finishProperty(const fixedstr::SFixedStr &kind, 
                        Node * key, 
                        Node * value);
    void finishReturnStatement(Node * argument);
    void finishSequenceExpression(const std::vector<Node*>& expressions);
    void finishSwitchCase(Node * test, 
                          const std::vector<Node*>& consequent);
    void finishSwitchStatement(Node * discriminant, 
                               const std::vector<Node*>& cases);
    void finishThisExpression();
    void finishThrowStatement(Node * argument);
    void finishTryStatement(Node * block, 
                            const std::vector<Node*>& guardedHandlers, 
                            const std::vector<Node*>& handlers, 
                            Node * finalizer);
    void finishUnaryExpression(const std::string oper, 
                               Node * argument);
    void finishVariableDeclaration(const std::vector<Node*>& declarations, 
                                   const fixedstr::SFixedStr &kind);
    void finishVariableDeclarator(Node * id, 
                                  Node * init);
    void finishWhileStatement(Node * test,
                              Node * body);
    void finishWithStatement(Node * object, 
                             Node * body);
    void delNode(Node * toDel);

protected:
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
    std::vector<Node*> val;
    ErrWrapvectorNodePtr() {
        err = false;
    }
    ErrWrapvectorNodePtr(std::vector<Node*> in) {
        val = in;
        err = false;
    }
};
#endif
#endif
