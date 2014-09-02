#line 2 "linprima.cpp"
//#include <vector>
//#include <map>
//#include <string>
#include <unordered_set>

//#include <rapidjson/...

//#include "debug.h"
//#include "utility.h"
//#include "enums.h"

//algorithm is for find(vector.begin()
#include <algorithm>
#include <functional>
#include <memory>

#ifdef USE_PROFINY
#include "Profiny.h"
#endif
#ifdef THROWABLE
#include <exception>
#endif

#define reqinline inline //save tweaking this for last. Talking maybe 10ms system / 20 ms asm in one rep of Chart.js (huge file), so pretty low return on optimization.

//using namespace rapidjson;
//using namespace std;

const char * emptystr = "";
const StrRef EMPTY_STRREF = StringRef(emptystr, 0);
//typedef Document::AllocatorType AllocatorType;


// -----------------------





class NodesComments;

struct ParseParamsOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    ptrTkn firstRestricted;
    ptrTkn stricted;
    unordered_set<string> paramSet;
    string message;
    ParseParamsOptions();
};

ParseParamsOptions::ParseParamsOptions() {
    firstRestricted = 0x0; 
    stricted = 0x0; 
}



struct ParseParamsOut {
#ifndef THROWABLE
    bool err;
#endif
    ptrTkn firstRestricted;
    ptrTkn stricted;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
    ParseParamsOut();
};

ParseParamsOut::ParseParamsOut() {
#ifndef THROWABLE
    err = false;
#endif
    message="";
    firstRestricted = 0x0; 
    stricted = 0x0; 
}


struct ReinterpretOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    Node *firstRestricted;
    Node *stricted;
    unordered_set<string> paramSet;
    string message;
    ReinterpretOptions();
};

ReinterpretOptions::ReinterpretOptions() {
    firstRestricted = NULLNODE; //?
    stricted = NULLNODE; //? nec.? had it only in reinOut before.
    //? not sure if context will make it different.
}

struct ReinterpretOut {
    Node *firstRestricted;
    Node *stricted;
    bool isNull;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
#ifndef THROWABLE
    bool err;
#endif
    void* rest; //seems to be a dummy var?
    ReinterpretOut();
};
ReinterpretOut::ReinterpretOut() {
#ifndef THROWABLE
    err = false;
#endif
    isNull=false;
    firstRestricted = NULLNODE;
    stricted = NULLNODE;
}

//---- ----------  -----------------------------
// signatures (temporary until we set up a header file):

 // 7.4 Comments





//#this is the ONLY constructor in this code capable of 
//#modifying task state, it ALWAYS and ONLY changes state 
//#if lookaheadAvail
//#is true. Important to keep in mind when making 
//#1:1 updates.
Node::Node(bool lookaheadAvail, 
           bool exists, 
           vector<Node*>* heapNodes,
           AllocatorType* allocArg,
           LinprimaTask * taskArg):
    loc(-1,-1,-1), 
#ifdef LIMITJSON
    alloc(&(jv.GetAllocator())), 
#endif
#ifndef LIMITJSON
    alloc(allocArg), 
#endif
    task(taskArg) {
 
    DEBUGIN("Node::Node(bool, bool)", true);
#ifndef THROWABLE
    err = false;
#endif
    hasJv = false;
    hasRange = false;
    hasLoc = false;
    if (lookaheadAvail) {
        //#not true for nullnode or parseProgram's node.
        lookavailInit();
    } 
    if (lookaheadAvail || exists) {
        this->heapNodes = heapNodes;
        heapNodes->push_back(this);
        if (task->extra.range) {
            hasRange = true;
            range[0] = task->idx;
            range[1] = 0;
        }

        if (task->extra.loc) {
            hasLoc = true;
            loc.startLine = task->lineNumber;
            loc.startColumn = task->idx - task->lineStart;
        } 
    }
    DEBUGOUT("", true);
}

string Node::s(const u16string in) { return toU8(in); }

void Node::lookavailInit() {
    hasJv = true;
    jv.SetObject();

     task->idx = task->lookahead->start;
     if (task->lookahead->type == TknType::StringLiteral) {
         task->lineNumber = task->lookahead->startLineNumber;
         task->lineStart = task->lookahead->startLineStart;
     } else {
         task->lineNumber = task->lookahead->lineNumber;
         task->lineStart = task->lookahead->lineStart;
     }
     if (hasRange) { //#should always be true, but keep it open while testing.
         loc.startLine = task->lineNumber;
         loc.startColumn = task->idx - task->lineStart;
         range[0] = task->idx;
     }
 }

 void Node::clear() {
     regexPaths.clear();
     expressions.clear();

     hasLoc = false;
     hasRange = false;
 }

 void Node::unused() {
     delNode(this);
 }


reqinline
void Node::jvput(const StrRef path, const string b)  {
    jv.AddMember(path, 
              Value(b.data(), b.length(), *alloc).Move(), 
              *alloc); 
}

void Node::jvput(const StrRef path, const StrRef &b) {
    jv.AddMember(path, b, *alloc); 
}
reqinline
void Node::jvput(const StrRef path, const int b) 
{jv.AddMember(path, b, *alloc); }

reqinline
void Node::jvput(const StrRef path, const bool b) 
{jv.AddMember(path, b, *alloc); }

reqinline
void Node::jvput_dbl(const StrRef path, const double b) 
{jv.AddMember(path, b, *alloc); }

reqinline
void Node::jvput_null(const StrRef path)
{ Value tmp; jv.AddMember(path, tmp, *alloc); }
 

//# different name to prevent easy bug of forgetting the string.
//# root path, should be first in vector, then path within it, etc.
void Node::regNoadd(const vector<RegexLeg> paths, Node * child) { 
    //string debugmsg = " Node::regNoadd(vector<string> paths, Node &child) :::";
    //debugmsg.append(paths[0]);
    //DEBUGIN(debugmsg, false);
    if (child == NULLNODE) { return; }

    if (child->hasRange) {
        Value rangearr(kArrayType);
        //bool m = find(heapNodes.begin(), haystack.end(), needle) != haystack.end();
        //int n = (m)? -1:0;
        //        rangearr.PushBack(n, *alloc);
        rangearr.PushBack(child->range[0], *alloc);
        rangearr.PushBack(child->range[1], *alloc);
        child->jv.AddMember(text::_range, rangearr, *alloc);
    } 
    if (child->hasLoc) {        
        Value locjson(kObjectType);
        child->loc.toJson(locjson, alloc);
        child->jv.AddMember(text::_loc, locjson, *alloc);
    }
    if (child->regexPaths.size() > 0) {
        if (child->regexPaths[0][0].isStart) {
            vector<RegexLeg> reverse;
            for (int i=paths.size()-1; i >= 0; i--) {
                reverse.push_back(paths[i]);
            }
            regexPaths.push_back(reverse);
        } else {
            for (unsigned int i=0; i<child->regexPaths.size(); i++) {
                regexPaths.push_back(child->regexPaths[i]);
                for (int j=paths.size()-1; j>=0; j--) {
                    regexPaths.back().push_back(paths[j]);
                }

            }
        }
    }
    //DEBUGOUT("Node::regNoAdd", false);
}
#ifdef LIMITJSON
void AddDocument(LinprimaTask* task,
            const StrRef &path, Document &root, Document &branch) {
    Writer<StringBuffer> writer(task->buffer);
    branch.Accept(writer);
    task->completeObjects->push_back(task->buffer.GetString());
    task->buffer.Clear();
    string objectAddr = encodeObjId(task->completeObjects->size()-1);
    root.AddMember(path,
                       Value(objectAddr.data(),
                                 objectAddr.length(),
                                 root.GetAllocator()).Move(), 
                    root.GetAllocator());
}
void PushDocument(LinprimaTask* task, AllocatorType &alloc,
                Value &root, Document &branch) {
    Writer<StringBuffer> writer(task->buffer);
    branch.Accept(writer);
    task->completeObjects->push_back(task->buffer.GetString());
    task->buffer.Clear();
    string objectAddr = encodeObjId(task->completeObjects->size()-1);
    root.PushBack(Value(objectAddr.data(),
                        objectAddr.length(),
                        alloc).Move(), 
                  alloc);
}
#endif
#ifdef LIMITJSON

size_t Node::addUnresolvedDocument(const StrRef &path) {
     size_t pos = task->completeObjects->size();
     task->completeObjects->push_back("{}");  
     string objectAddr = encodeObjId(pos);
     jv.AddMember(path, 
                  Value(objectAddr.data(),
                        objectAddr.length(),
                        *alloc).Move(),
                  *alloc);
     return pos;
}
size_t Node::pushUnresolvedDocument(Value &root) {
     size_t pos = task->completeObjects->size();
     task->completeObjects->push_back("{}");  
     string objectAddr = encodeObjId(pos);
     root.PushBack(Value(objectAddr.data(),
                         objectAddr.length(),
                         *alloc).Move(),
                    *alloc);
     return pos;
}

void Node::lateResolve() {
    Writer<StringBuffer> writer(task->buffer);
    jv.Accept(writer);
    string result = task->buffer.GetString(); 
    //printf(" late resolve %s \n", result.data());
    (*(task->completeObjects))[completedPos] = result;
    task->buffer.Clear();
    delNode(this);       
}

#endif

void Node::reg(const StrRef &path, Node * child) { 
    //DEBUGIN("reg(string path, Node &child)", false);
    
    //addition of sequence expression's children is done lazily
    //so that they are available for reinterpretAsCoverList
    //to reinterpret them as children of a different node
    //and then cleanly delete sequenceexpression node
    //without having to extricate children objects from json, etc.
    if (child != NULLNODE) {
        if (child->type == Syntax[Synt::SequenceExpression]) {
            child->nodeVec(text::_expressions, child->expressions);
        } else if (child->type 
                   == Syntax[Synt::AssignmentExpression]) {
            child->reg(text::_left, child->leftAssign);
            child->reg(text::_right, child->rightAssign);
        }
        vector<RegexLeg> pathlist;
        pathlist.emplace_back((&(path)));
        regNoadd(pathlist, child);
#ifndef LIMITJSON
        jv.AddMember(path, 
                     child->jv.Move(), 
                     *alloc);

        if (child->thisnc
            && !(child->thisnc->resolved)) {
            child->thisnc->nodesJv = &(jv[path]);
        }
        delNode (child); 
#endif
#ifdef LIMITJSON
        if (task->extra.attachComment 
            && child->thisnc 
            && !(child->thisnc->resolved)) {
            child->completedPos = addUnresolvedDocument(path);
          
            child->thisnc->setNodeDetached(child);
        } else {
            AddDocument(task, path, jv, child->jv);
            delNode (child); 
        }
#endif
    } else {
        Value tmp;
        jv.AddMember(path,
                      tmp, *alloc);
    }

    //DEBUGOUT("node::reg", false);
}

void Node::nodeVec(const StrRef &path, const vector< Node* > & nodes) { 
    //DEBUGIN("nodeVec(string path, vector< Node > & nodes)", false);
    Value arr(kArrayType);
    for (unsigned int i=0; i<nodes.size(); i++) {
        if (nodes[i] != NULLNODE) {
            if (nodes[i]->type == Syntax[Synt::SequenceExpression]) {
                nodes[i]->nodeVec(text::_expressions, nodes[i]->expressions);
            } else if (nodes[i]->type == Syntax[Synt::AssignmentExpression]) {
                nodes[i]->reg(text::_left, nodes[i]->leftAssign);
                nodes[i]->reg(text::_right, nodes[i]->rightAssign);
            } else if (nodes[i]->type == Syntax[Synt::Property]) {
                nodes[i]->reg(text::_key, nodes[i]->leftAssign);
                nodes[i]->reg(text::_value, nodes[i]->rightAssign);
                nodes[i]->jvput(text::_kind, *(nodes[i]->spareStrref));
            }
            vector<RegexLeg> pathlist;
            pathlist.emplace_back((&(path)));
            pathlist.emplace_back(i);
            regNoadd(pathlist, nodes[i]);
#ifndef LIMITJSON
            arr.PushBack(nodes[i]->jv.Move(), *alloc);
            if (nodes[i]->thisnc
                && !(nodes[i]->thisnc->resolved)) {
                nodes[i]->thisnc->nodesJv = &(arr[i]);
            }
            delNode (nodes[i]);
#endif
#ifdef LIMITJSON
            if (task->extra.attachComment 
                && nodes[i]->thisnc 
                && !(nodes[i]->thisnc->resolved)) {

                nodes[i]->completedPos = pushUnresolvedDocument(arr);                
                nodes[i]->thisnc->setNodeDetached(nodes[i]);
            } else {
                PushDocument(task, jv.GetAllocator(), arr, nodes[i]->jv);
                delNode (nodes[i]);
            }
#endif
        } else {
            Value tmp;
            arr.PushBack(tmp, *alloc);
        }
    } 
    jv.AddMember(path, arr, *alloc);
    //DEBUGOUT("node::nodeVec", false);
}
inline
void Node::addType(const Synt in) { 
    //DEBUGIN("addType", false);
    type = Syntax[in];
    
    jv.AddMember(text::_type, *type, *alloc);
}
void Node::regexPaths2json(Value& out, AllocatorType *alloc) { 
    //DEBUGIN("Node::regexPaths2json()", false);
    out.SetArray();    
    Value path;
    for (unsigned int i=0; i<regexPaths.size(); i++) {
        path.SetArray();
        for (int j=regexPaths[i].size()-1; j>=0; j--) {
            if (regexPaths[i][j].isNum) {
                path.PushBack(regexPaths[i][j].num,
                          *alloc);           
            } else {
                path.PushBack(*(regexPaths[i][j].path),
                          *alloc);           
            } 
        } 
        out.PushBack(path, *alloc);
    }
    //DEBUGOUT("", false);     
}


//#not the most efficient way to do this. would be easy to choke
//#on big comments. TODO move NodesComments to heap, free
//on removal from bottomright and end of use.
void Node::processComment() {
    DEBUGIN("processComment()", false);
    //# assumes attachComments 
    //# so that means range is already true.

    ExtraStruct &extra = task->extra;
    vector< Comment > trailingComments;

    vector< shared_ptr<NodesComments> > * bottomRight = 
        &(extra.bottomRightStack);
    shared_ptr<NodesComments> lastChild;
    shared_ptr<NodesComments> last;
    thisnc.reset(new NodesComments(jv, alloc));
    bool LEADING = true, TRAILING= false;
    if (bottomRight->size() > 0) {
        last = bottomRight->back();
    }

    if (type == Syntax[Synt::Program]) {  
        if (jv[text::_body].Size() > 0) {
            DEBUGOUT("", false); 
            thisnc->resolved = true;
            return;
        }
    }

    thisnc->range[0] = range[0];
    thisnc->range[1] = range[1];

    if (extra.trailingComments.size() > 0) {
        if (extra.trailingComments[0].range[0] >= thisnc->range[1]) {
            trailingComments = extra.trailingComments;
            extra.trailingComments.clear();
        } else {
            extra.trailingComments.clear(); 
            //# originally first clause had =[] and this has .length = 0
            //# don't think there's an effective difference thoug
        }
    } else {
        if (last && 
            last->trailingComments.size() > 0 && 
            last->trailingComments[0].range[0] >= thisnc->range[1]) {
            trailingComments = last->trailingComments;
            last->trailingComments.clear();
            last->commentsIntoJson(&(task->extra), TRAILING);
            //delete last->trailingComments; 
            //? maybe have a boolean to say no trailing comments? length will prob. be workable.
        }
    }

    int iternum = 0;
    bool unattachedLast = false;
    // Eating the stack.
    if (last) {
        while ((last) && last->range[0] >= thisnc->range[0]) {
            if (iternum == 2){
                //after the first iteration lastChild and last are equal
                //because last is assigned above via a back() without
                //an accompanying pop_back. Only on the second iteration
                //
                //the first last is not actually removed from the stack.
                //only when last has been assigned to lastChild twice
                //does lastChild necesarily point to something that's
                //about to have 0 references. (be deallocated for good.)
                lastChild->resolve();
                ++iternum;
            }
            unattachedLast = false;
            lastChild = last;
            if (bottomRight->size() > 0) { 
                unattachedLast = true;
                last = bottomRight->back(); 
                bottomRight->pop_back();
            } else { 
                last.reset();
            }
            if (iternum < 2){ ++iternum; }
        }
    }
    if (iternum > 0 && unattachedLast) {
        last->resolve();
    }

    if (lastChild) {
        if (lastChild->leadingComments.size() > 0 &&
            lastChild->leadingComments.back()
            .range[1] <= thisnc->range[0]) {
            thisnc->leadingComments = lastChild->leadingComments;
            lastChild->leadingComments.clear();
            lastChild->commentsIntoJson(&(task->extra), LEADING);
            thisnc->commentsIntoJson(&(task->extra), LEADING);
        }
        lastChild->resolve();
    } else if (extra.leadingComments.size() > 0 && 
               extra.leadingComments[extra.leadingComments.size() - 1]
               .range[1] <= thisnc->range[0]) {
        thisnc->leadingComments = extra.leadingComments;
        extra.leadingComments.clear();
        thisnc->commentsIntoJson(&(task->extra), LEADING);
    }

    if (trailingComments.size() > 0) {
        thisnc->trailingComments = trailingComments;
        thisnc->commentsIntoJson(&(task->extra), TRAILING);
    }

    thisnc->resolved = false;
    bottomRight->push_back(thisnc);
    DEBUGOUT("", false);
}


void Node::finish() {
    DEBUGIN("finish()", false);
    if (task->extra.range) {
        this->range[1] = task->idx; 
    }
    if (task->extra.loc) {        
        loc.endLine = task->lineNumber;
        loc.endColumn = task->idx - task->lineStart;
        if (task->extra.hasSource) {
            loc.source = task->extra.source; 
            loc.hasSource = true;
        }
    }

    if (task->extra.attachComment) {
        this->processComment();
    }
    DEBUGOUT("node::finish", false);
}


void Node::finishArrayExpression(const vector< Node* >& elements) {
    DEBUGIN("finishArrayExpression(vector< Node >& elements)", false);
    addType(Synt::ArrayExpression);
    nodeVec(text::_elements, elements);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishArrowFunctionExpression(const vector< Node* >& params,
                                         const vector< Node* >& defaults, 
                                         Node * body, 
                                         const bool expression) {
    DEBUGIN("finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression)", false);
    addType(Synt::ArrowFunctionExpression);

    jvput_null(text::_id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jvput_null(text::_rest);
    jvput(text::_generator, false);
    jvput(text::_expression, expression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishAssignmentExpression(const string oper, 
                                      Node * left, 
                                      Node * right) {
    DEBUGIN("finishAssignmentExpression(u16string oper, Node& left, Node& right)", false);

    addType(Synt::AssignmentExpression);
    jvput(text::_operator, oper);

    leftAssign = left;
    rightAssign = right;

    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBinaryExpression(const string oper, 
                                  Node * left, 
                                  Node * right) {
    DEBUGIN("finishBinaryExpression(u16string oper, Node& left, Node& right)", false);
    addType((oper == "||" || oper == "&&") ? 
            Synt::LogicalExpression : Synt::BinaryExpression);
    jvput(text::_operator, oper);
    reg(text::_left, left); 
    reg(text::_right, right);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBlockStatement(const vector< Node* >& body) {
    DEBUGIN("finishBlockStatement(vector< Node >& body)", false);
    addType(Synt::BlockStatement);
    nodeVec(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBreakStatement(Node * label) {
    DEBUGIN("finishBreakStatement(Node& label)", false);
    addType(Synt::BreakStatement);
    reg(text::_label, label);
    this->finish();  
    DEBUGOUT("", false);
}


void Node::finishCallExpression(Node * callee, 
                                const vector< Node* >& args) {
    DEBUGIN("finishCallExpression", false);
    addType(Synt::CallExpression);
    reg(text::_callee, callee);
    nodeVec(text::_arguments, args);
    this->finish(); 
    DEBUGOUT("", false);
}


void Node::finishCatchClause(Node * param, 
                             Node * body) {
    DEBUGIN("finishCatchClause", false);
    addType(Synt::CatchClause);
    reg(text::_param, param);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishConditionalExpression(Node * test, 
                                       Node * consequent,
                                       Node * alternate) {
    DEBUGIN("finishConditionalExpression", false);
    addType(Synt::ConditionalExpression);
    reg(text::_test, test);
    reg(text::_consequent, consequent);
    reg(text::_alternate, alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishContinueStatement(Node * label) {
    DEBUGIN("finishContinueStatement", false);
    addType(Synt::ContinueStatement);
    reg(text::_label, label);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishDebuggerStatement() {
    DEBUGIN("finishDebuggerStatement", false);
    addType(Synt::DebuggerStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishDoWhileStatement(Node * body, Node * test) {
    DEBUGIN("finishDoWhileStatement", false);
    addType(Synt::DoWhileStatement);
    reg(text::_body, body);
    reg(text::_test, test);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishEmptyStatement() {
    DEBUGIN("finishEmptyStatement()", false);
    addType(Synt::EmptyStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishExpressionStatement(Node * expression) {
    DEBUGIN("finishExpressionStatement", false);
    addType(Synt::ExpressionStatement);
    spareStrref = expression->type;
    reg(text::_expression, expression);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishForStatement(Node * init, 
                              Node * test, 
                              Node * update, 
                              Node * body) {
    DEBUGIN("finishForStatement", false);
    addType(Synt::ForStatement);
    reg(text::_init, init);
    reg(text::_test, test);
    reg(text::_update, update);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishForInStatement(Node * left, 
                                Node * right, 
                                Node * body) {
    DEBUGIN("finishForInStatement", false);
    addType(Synt::ForInStatement);
    reg(text::_left, left);
    reg(text::_right, right);
    reg(text::_body, body);
    jvput(text::_each, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionDeclaration(Node * id, 
                                     const vector< Node* >& params, 
                                     const vector< Node* >& defaults, 
                                     Node * body) {
    DEBUGIN("Node::finishFunctionDeclaration", false);
    addType(Synt::FunctionDeclaration);
    reg(text::_id, id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jvput_null(text::_rest);
    jvput(text::_generator, false);
    jvput(text::_expression, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionExpression(Node * id, 
                                    const vector< Node* >& params, 
                                    const vector< Node* >& defaults, 
                                    Node * body) {  
    DEBUGIN("finishFunctionExpression", false);
    addType(Synt::FunctionExpression);
    reg(text::_id, id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jvput_null(text::_rest);
    jvput(text::_generator, false);
    jvput(text::_expression, false);
    this->finish();
    DEBUGOUT("", false);
}
void Node::finishIdentifier(const string name) {
    DEBUGIN("finishIdentifier", false);
    addType(Synt::Identifier);
    this->name = name;
    jvput(text::_name, name);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishIfStatement(Node * test, 
                             Node * consequent, 
                             Node * alternate) {
    DEBUGIN("finishIfStatement", false);
    addType(Synt::IfStatement);
    reg(text::_test, test);
    reg(text::_consequent, consequent);
    reg(text::_alternate, alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishLabeledStatement(Node * label, 
                                  Node * body) {
    DEBUGIN("finishLabeledStatement", false);
    addType(Synt::LabeledStatement);
    reg(text::_label, label);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishLiteral(ptrTkn token) {
    DEBUGIN("finishLiteral(ptrTkn token)", false);
    addType(Synt::Literal);
    if (token->literaltype == LiteralType["String"]) {
        jvput(text::_value, token->strvalue);
    } else if (token->literaltype == LiteralType["Int"]) {
        jvput(text::_value, token->intvalue);
    } else if (token->literaltype == LiteralType["Double"]) {
        jvput_dbl(text::_value, stod(token->strvalue));
    } else if (token->literaltype == LiteralType["Bool"]) {
        jvput(text::_value, token->bvalue);
    } else if (token->literaltype == LiteralType["Null"]) {
        jvput_null(text::_value);
    } else if (token->literaltype == LiteralType["Regexp"]) {
        Value reg(kObjectType);
        reg.AddMember(text::_regexpBody, 
                      Value(token->strvalue.data(),
                            token->strvalue.length(),
                            *alloc).Move(),
                      *alloc);
        reg.AddMember(text::_regexpFlags,
                      Value(token->flags.data(),
                            token->flags.length(),
                            *alloc).Move(),
                      *alloc);
        reg.AddMember(text::_lineNumber,
                      task->lineNumber, *alloc); 
        reg.AddMember(text::_index,
                      token->end, *alloc); 
        reg.AddMember(text::_column,
                      token->end+1, *alloc); 
        jv.AddMember(text::_value, reg, *alloc);
        RegexLeg starter(-1);
        starter.isStart = true;
        regexPaths.push_back({starter});
    }
    jvput(text::_raw, s(slice(task->sourceRaw, 
                              token->start, 
                              token->end)));
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishMemberExpression(const char16_t accessor, 
                                  Node * object, 
                                  Node * property) {
    DEBUGIN("finishMemberExpression", false);
    addType(Synt::MemberExpression);
    jvput(text::_computed, (accessor == u'['));
    reg(text::_object, object);
    reg(text::_property, property);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishNewExpression(Node * callee, 
                               const vector<Node*>& args) {
    DEBUGIN("finishNewExpression", false);
    addType(Synt::NewExpression);
    reg(text::_callee, callee);
    nodeVec(text::_arguments, args);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishObjectExpression(const vector<Node*>& properties) {
    DEBUGIN("finishObjectExpression", false);
    addType(Synt::ObjectExpression);
    nodeVec(text::_properties, properties);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishPostfixExpression(const string oper, 
                                   Node * argument) {
    DEBUGIN("finishPostfixExpression", false);
    addType(Synt::UpdateExpression);
    jvput(text::_operator, oper);
    reg(text::_argument, argument);
    jvput(text::_prefix, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishProgram(const vector< Node* >& body) {
    DEBUGIN("finishProgram", false);
    addType(Synt::Program);
    nodeVec(text::_body, body);
    for (int i=0; i<task->extra.bottomRightStack.size(); i++) {
        task->extra.bottomRightStack[i]->resolve();
    }
    this->finish();
    //no parent node to call reg so add these atts. here.
    if (task->extra.range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        jv.AddMember(text::_range, rangearr, *alloc);
    }
    if (task->extra.loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        jv.AddMember(text::_loc, locjson, *alloc);
    }
    DEBUGOUT("", false);    
}


void Node::finishProperty(const StrRef &kind,
                          Node * key, 
                          Node * value) {
    DEBUGIN("finishProperty", false);
    addType(Synt::Property);
    leftAssign = key;
    rightAssign = value;
    spareStrref = &kind;
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishReturnStatement(Node * argument) {
    DEBUGIN("finishReturnStatement", false);
    addType(Synt::ReturnStatement);
    reg(text::_argument, argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSequenceExpression(const vector< Node* >& expressions) {
    DEBUGIN("finishSequenceExpression", false);
    addType(Synt::SequenceExpression);
    this->expressions = expressions;    
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchCase(Node * test, 
                            const vector< Node* >& consequent) {
    DEBUGIN("finishSwitchCase", false);
    addType(Synt::SwitchCase);
    reg(text::_test, test);
    nodeVec(text::_consequent, consequent);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchStatement(Node * discriminant, 
                                 const vector < Node* >& cases) {
    DEBUGIN("finishSwitchStatement", false);
    addType(Synt::SwitchStatement);
    reg(text::_discriminant, discriminant);
    nodeVec(text::_cases, cases);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThisExpression() {
    DEBUGIN("finishThisExpression()", false);
    addType(Synt::ThisExpression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThrowStatement(Node * argument) {
    DEBUGIN("finishThrowStatement", false);
    addType(Synt::ThrowStatement);
    reg(text::_argument, argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishTryStatement(Node * block, 
                              const vector<Node*>& guardedHandlers, 
                              const vector<Node*>& handlers, 
                              Node * finalizer) {
    DEBUGIN("finishTryStatement", false);
    addType(Synt::TryStatement);
    reg(text::_block, block);
    nodeVec(text::_guardedHandlers, guardedHandlers);
    nodeVec(text::_handlers, handlers);
    reg(text::_finalizer, finalizer);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishUnaryExpression(const string oper, 
                                 Node * argument) {
    DEBUGIN("finishUnaryExpression", false);
    addType((oper == "++" || oper == "--") ? 
            Synt::UpdateExpression : Synt::UnaryExpression);
    jvput(text::_operator, oper);
    reg(text::_argument, argument);
    jvput(text::_prefix, true);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishVariableDeclaration(const vector< Node* >& declarations, 
                                     const StrRef& kind) {
    DEBUGIN("finishVariableDeclaration",false);
    addType(Synt::VariableDeclaration);
    nodeVec(text::_declarations, declarations);
    jvput(text::_kind, kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclarator(Node * id, 
                                    Node * init) {
    DEBUGIN("finishVariableDeclarator", false);
    addType(Synt::VariableDeclarator);
    reg(text::_id, id);
    reg(text::_init, init);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWhileStatement(Node * test, 
                                Node * body) {
    DEBUGIN("finishWhileStatement", false);
    addType(Synt::WhileStatement);
    reg(text::_test, test);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWithStatement(Node * object, 
                               Node * body) {
    DEBUGIN("finishWithStatement", false);
    addType(Synt::WithStatement);
    reg(text::_object, object);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


class WrappingNode : public Node {
public:
    WrappingNode(ptrTkn startToken, 
                 vector<Node*>*heapNodesArg,
                 AllocatorType* alloc,
                 LinprimaTask* task) 
        : Node(false, true, heapNodesArg, 
               alloc, task) {
        DEBUGIN("WrappingNode(Token)", true);
        if (!hasJv) { 
            jv.SetObject();
            hasJv=true;
        }
        if (task->extra.range) {
            hasRange = true;
            range[0] = startToken->start;
            range[1] = 0;
        }

        if (task->extra.loc) {
            loc = this->WrappingSourceLocation(startToken);
        }

        DEBUGOUT("WrappingNode(Token)", true);
    }
    Loc WrappingSourceLocation(ptrTkn startToken);
};

Loc WrappingNode::WrappingSourceLocation(ptrTkn startToken) {
    DEBUGIN("WrappingSourceLocation (Token)", true);
    Loc result(0,0,0);
    if (startToken->type == TknType::StringLiteral) {
        result.startLine = startToken->startLineNumber;
        result.startColumn = 
            startToken->start - startToken->startLineStart;
    } else {
        result.startLine = startToken->lineNumber;
        result.startColumn = startToken->start - startToken->lineStart;
    }
    result.endLine = -1;
    result.endColumn = -1;
    //return result;
    DEBUGOUT("WrappingSrcLoc", true); 
    return result;
}

// Return true if there is a line terminator before the next token.

    // Expect the next token to match the specified punctuator.
    // If not, an exception will be thrown.


// Return true if the next token matches the specified punctuator.

//clang++ _refuses_ to inline match(val) properly on its own.
//TODO: switch this to typedefs for better typing integration.
//#define match(A) (lookahead->type == TknType::Punctuator && lookahead->strvalue == A)

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

ParseTools::ParseTools(const u16string code, 
                       OptionsStruct options) :    
    task(make_shared<LinprimaTask>(code,
                                        //code.data(), 
                                        //code.length(), 
                                       options)),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead),
         scanner(task) {
}

ParseTools::~ParseTools() {
    clearNodeHeap();
}


void Node::delNode(Node * toDel) {
    if (toDel == NULLNODE) { return; }
    auto iter = find(heapNodes->begin(), heapNodes->end(), toDel);
    if (iter != heapNodes->end()) { heapNodes->erase(iter); }
    delete (toDel);
}
void ParseTools::clearNodeHeap() {
    Node *tmp;
    while (heapNodes.size() > 0) {
        auto it = heapNodes.begin();
        tmp = *it;
        heapNodes.erase(it);
        //if (tmp->jv != nullptr)
        //    { json_object_put(tmp->jv); tmp->jv = nullptr; }

        delete (tmp);
    }
}

char16_t ParseTools::source(long pos) { return *(sourceRaw + pos); }

//throw_
bool ParseTools::peekLineTerminator() {
    DEBUGIN(" peekLineTerminator()", false);
    int pos = idx,
        line = lineNumber,
        start = lineStart;
    bool found;
    scanner.skipComment();
    found = (lineNumber != line);
    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("", false);
    return found;
}

reqinline
//throw_
void ParseTools::expect(const string value) { 
    //DEBUGIN(" expect(u16string value)", false);
    ptrTkn token = scanner.lex();

    if (token->type != TknType::Punctuator || 
          token->strvalue != value) {
        task->throwUnexpected(token); 
    }
    // DEBUGOUT("expect", false);
    return;
}



//throw_
void ParseTools::expectTolerant(const string value) {
    // DEBUGIN(" expectTolerant(u16string value)", false);
    if (extra.errorTolerant) {
        ptrTkn token = lookahead;


        if (token->type != TknType::Punctuator || 
              token->strvalue != value) {
            task->throwErrorTolerant(token, Messages[Mssg::UnexpectedToken], 
                               {token->strvalue});
        } else {
            scanner.lex();
        }
    } else {
        expect(value);
    }
    // DEBUGOUT("expectTol", false);
    return;
}

// Expect the next token to match the specified keyword.
// If not, an exception will be thrown.
reqinline
//throw_
void ParseTools::expectKeyword(const string keyword) { 
    //DEBUGIN("expectKeyword", false);
    ptrTkn token = scanner.lex();
    if (token->type != TknType::Keyword || 
        token->strvalue != keyword) {
        task->throwUnexpected(token);
    }
    return;
}

bool ParseTools::match(const string value) { 
    //DEBUGIN("match", false);
    return lookahead->type == TknType::Punctuator 
        && lookahead->strvalue == value;
        } 

// Return true if the next token matches the specified keyword

bool ParseTools::matchKeyword(const string keyword) {
    // DEBUGIN(" matchKeyword(const u16string keyword)", false);
    //  DEBUGOUT("matchKey", false); 
    return lookahead->type == TknType::Keyword 
        && lookahead->strvalue == keyword;
}

    // Return true if the next token is an assignment operator

bool ParseTools::matchAssign() { 
    if (lookahead->type != TknType::Punctuator) {
        return false;
    }
    return has(lookahead->strvalue, {
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

//#can't dynamically initialize empty vectors
//#if func is passed by reference.
vector<Node*> EMPTY_NODE_LIST;

//throw_
void ParseTools::consumeSemicolon() {
    DEBUGIN(" consumeSemicolon()", false);
    int line;

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B || match(";")) { 
        scanner.lex();
        DEBUGOUT("", false); 
        return;
    }

    line = lineNumber;
    scanner.skipComment(); 
    if (lineNumber != line) {
        DEBUGOUT("", false); 
        return;
    }

    if (lookahead->type != TknType::EOFF && !match("}")) { 
        task->throwUnexpected(lookahead); 
    }
    DEBUGOUT("consumeSemi", false);
    return;
}

    // Return true if provided expression is LeftHandSideExpression

bool ParseTools::isLeftHandSide(Node* expr) {
    DEBUGIN("   isLeftHandSide(Node expr)", false);
    DEBUGOUT("isLeft", false);
    return expr->type == Syntax[Synt::Identifier] || expr->type == Syntax[Synt::MemberExpression];
};


Node* ParseTools::makeNode(bool lookavailInit, bool exists) {
    return new Node(lookavailInit, exists, &heapNodes, alloc, task.get());
}
WrappingNode* ParseTools::makeWrappingNode(ptrTkn token) {
    return new WrappingNode(token, &heapNodes, alloc, task.get());
}

// 11.1.4 Array Initialiser

//throw_
Node* ParseTools::parseArrayInitialiser() {
    DEBUGIN(" parseArrayInitialiser()", false);
    vector< Node* > elements;
    Node *node = makeNode(true, true);    

    expect("[");

    while (!match("]")) {

        if (match(",")) {
            scanner.lex();
            elements.push_back(NULLNODE);
        } else {
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
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

    scanner.lex();

    node->finishArrayExpression(elements);
    DEBUGOUT("parseArrInit", false);
    return node;
}

// 11.1.5 Object Initialiser

//throw_
Node* ParseTools::parsePropertyFunction(vector<Node*>& param, 
                                            ptrTkn first) {
    DEBUGIN(" parsePropertyFunction", false);
    bool previousStrict;
    Node *body,
        *node = makeNode(true, true);

    previousStrict = task->strict;
    body = parseFunctionSourceElements();
    if (!(first->isNull) 
        && task->strict 
        && Tokenizer::isRestrictedWord(param[0]->name)) {
        task->throwErrorTolerant(first, Messages[Mssg::StrictParamName],{});
    }
    task->strict = previousStrict;
    node->finishFunctionExpression(NULLNODE, param, 
                                  EMPTY_NODE_LIST, body); 
    DEBUGOUT("parsePropFunction", false);
    return node;
}

//throw_
Node* ParseTools::parseObjectPropertyKey() {
    DEBUGIN(" parseObjectPropertyKey()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    // Note: This function is called only from parseObjectProperty(), where
    // EOF and Punctuator tokens are already filtered out.

    if (token->type == TknType::StringLiteral || 
        token->type == TknType::NumericLiteral) {
        if (task->strict && token->octal) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictOctalLiteral], {});
        }
        node->finishLiteral(token);
        DEBUGOUT("", false); 
        return node;
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("ParseObjPropKey", false);
    return node;
}



//@Empty_node_list is passed as a vector_of_arrays,
//@ where normally what would be passed into .finishFunctionExpression is
//@ an undefined variable. this might make the value null expectedly,
//@ while making this version render empty list.
//throw_
Node* ParseTools::parseObjectProperty() {
    DEBUGIN(" parseObjectProperty()", false);
    ptrTkn token;
    vector<Node*> param;
    Node *id, *key, *value, 
        *node = makeNode(true, true);

    token = lookahead;

    if (token->type == TknType::Identifier) {

        id = parseObjectPropertyKey();

        // Property Assignment: Getter and Setter.

        if (token->strvalue == "get" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            expect(")");
            ptrTkn tmpNull = scanner.makeToken();
            tmpNull->isNull = true;
            value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                          tmpNull);
            node->finishProperty(text::_get, key, value);
            DEBUGOUT("parseObjProp", false); 
            return node;
        }
        if (token->strvalue == "set" && !match(":")) {
            key = parseObjectPropertyKey();
            expect("(");
            token = lookahead;
            if (token->type != TknType::Identifier) {
                expect(")");
                task->throwErrorTolerant(token, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {token->strvalue});

                ptrTkn tmpNull = scanner.makeToken();
                tmpNull->isNull = true;                
                value = parsePropertyFunction(EMPTY_NODE_LIST, 
                                              tmpNull);
            } else {
#ifndef THROWABLE
                Node *tmp = parseVariableIdentifier();
                param = vector< Node* >({ tmp });
#endif
#ifdef THROWABLE
                param = vector< Node* >({ parseVariableIdentifier() });
#endif
                expect(")");
                value = parsePropertyFunction(param, token);
            }
            node->finishProperty(text::_set, key, value);
            DEBUGOUT("parseObjProp", false); 
            return node;
        }
        expect(":");
        value = parseAssignmentExpression();
        node->finishProperty(text::_init, id, value);
        DEBUGOUT("parseObjProp", false); 
        return node;
    }
    if (token->type == TknType::EOFF 
        || token->type == TknType::Punctuator) {
        task->throwUnexpected(token);
        DEBUGOUT("parseObjProp", false); 
        return NULLNODE; //#just to satisfy warnings.
    } else {
        key = parseObjectPropertyKey();
        expect(":");
        value = parseAssignmentExpression();
        node->finishProperty(text::_init, key, value);
        DEBUGOUT("parseObjProp", false); 
        return node;
    }
}

string GetStringCorrect(const Value& val) {
    return string(val.GetString(), val.GetStringLength());
}

//throw_
Node* ParseTools::parseObjectInitialiser() {
    DEBUGIN(" parseObjectInitialiser()", false);
    vector<Node*> properties;
    ptrTkn token;
    Node *property, 
        *node = makeNode(true, true);
    
    string keytype, key, name;
    const StrRef *kindname;
    int kind;
    map<string, int> kmap;

    expect("{");

    while (!match("}")) {
        property = parseObjectProperty();
        
        Node * keyobj = property->leftAssign; //property->jv['key']

        keytype = GetStringCorrect(keyobj->jv[text::_type]);

        if (keytype == Syntax[Synt::Identifier]->s) {
            name = GetStringCorrect(keyobj->jv[text::_name]);            
        } else {
            if (keyobj->jv[text::_value].IsString()) {
                name = GetStringCorrect(keyobj->jv[text::_value]);
            } else {
                name = to_string(keyobj->jv[text::_value].GetDouble());
            }
        }
        kindname = property->spareStrref; //property->jv['kind']
        kind = (kindname == &(text::_init)) ? PropertyKind["Data"] : 
            (kindname == &(text::_get)) ? PropertyKind["Get"] : PropertyKind["Set"];

        key = "$";
        key.append(name);
        if (hasStringKey<int>(key,kmap)) {
            if (kmap[key] == PropertyKind["Data"]) {
                if (task->strict && kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::StrictDuplicateProperty],{});
                } else if (kind != PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                }
            } else {
                if (kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                } else if (kmap[key] & kind) {
                    task->throwErrorTolerant(NULLPTRTKN, 
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

    node->finishObjectExpression(properties);
    DEBUGOUT("parseObjectInit", false);
    return node;
}

 // 11.1.6 The Grouping Operator
//throw_
Node* ParseTools::parseGroupExpression() {
    DEBUGIN(" parseGroupExpression()", false);
    Node *expr;
    expect("(");
    if (match(")")) {
        scanner.lex();
        DEBUGOUT("", false);
        Node *tmpnode = makeNode(false, true);
        tmpnode->type = PlaceHolders["ArrowParameterPlaceHolder"]->type;
        return tmpnode;
    }
    ++(state.parenthesisCount);
    expr = parseExpression();
    expect(")");
    string debugmsg="parseGroupExpr()";
    DEBUGOUT(debugmsg, false);
    return expr;
}


// 11.1 Primary Expressions
//throw_
Node* ParseTools::parsePrimaryExpression() {
    DEBUGIN(" parsePrimaryExpression()", false);
    //# there's some opportunity here for nested function calls
    //# by using preprocess if defined / else / endifs 
    //# that use nested in non-asm. But they're just tokens so no
    //# depth (and high copy cost) like nodes.
    //# and because there are so many it makes this function visually
    //# hard to follow if we use nested whenever possible in this.
    TknType type; 
    ptrTkn token;
    Node *expr, *node;

    if (match("(")) {
        return DBGRET("", parseGroupExpression());
    }

    if (match("[")) {
        return DBGRET("", parseArrayInitialiser());
    }

    if (match("{")) {
        return DBGRET("", parseObjectInitialiser());
    }

    type = lookahead->type;
    node = makeNode(true, true);
    expr = node;

    if (type == TknType::Identifier) {
        ptrTkn tmp = scanner.lex();
        expr->finishIdentifier(tmp->strvalue);
    } else if (type == TknType::StringLiteral || 
               type == TknType::NumericLiteral) {

        if (task->strict && lookahead->octal) {
            task->throwErrorTolerant(lookahead, 
                               Messages[Mssg::StrictOctalLiteral], {});
        }
        ptrTkn tmp = scanner.lex();
        expr->finishLiteral(tmp);
    } else if (type == TknType::Keyword) {
        if (matchKeyword("function")) {
            expr->unused();
            return DBGRET("", parseFunctionExpression());
        }
        if (matchKeyword("this")) {
            scanner.lex();
            expr->finishThisExpression();
        } else {
            ptrTkn tmp = scanner.lex();
            task->throwUnexpected(tmp);
        }
    } else if (type == TknType::BooleanLiteral) {
        token = scanner.lex();
        token->bvalue = (token->strvalue == "true");
        token->literaltype = LiteralType["Bool"];
        expr->finishLiteral(token);
    } else if (type == TknType::NullLiteral) {
        token = scanner.lex();
        token->isNull = true;
        token->literaltype = LiteralType["Null"];
        expr->finishLiteral(token);
    } else if (match("/") || match("/=")) {
        ptrTkn tmp;
        if (extra.tokenTracking) {
            tmp = scanner.collectRegex();
            expr->finishLiteral(tmp);
        } else {
            tmp = scanner.scanRegExp();
            expr->finishLiteral(tmp);
        }
        scanner.peek();
    } else {
        ptrTkn tmp = scanner.lex();
        task->throwUnexpected(tmp);
    }

    DEBUGOUT("", false);
    return expr;
}

// 11.2 Left-Hand-Side Expressions
//throw_
vector< Node* > ParseTools::parseArguments() {
    DEBUGIN(" parseArguments()", false);
    vector< Node* > args; 
    expect("(");
    if (!match(")")) {
        while (idx < length) {
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
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
    DEBUGOUT("parseArguments", false);
    return args;
}

//throw_
Node* ParseTools::parseNonComputedProperty() {
    DEBUGIN(" parseNonComputedProperty()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    if (!Tokenizer::isIdentifierName(token->type)) {
        task->throwUnexpected(token);
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("", false);
    return node;
}

//throw_
Node* ParseTools::parseNonComputedMember() {
    DEBUGIN(" parseNonComputedMember()", false);
    expect(".");
    return DBGRET("parseNonComp", parseNonComputedProperty());
}

//throw_
Node* ParseTools::parseComputedMember() {
    DEBUGIN(" parseComputedMember()", false);
    Node *expr;
    expect("[");
    expr = parseExpression();
    expect("]");
    DEBUGOUT("parseComp", false);
    return expr;
}

//throw_
Node* ParseTools::parseNewExpression() {
    DEBUGIN(" parseNewExpression()", false);
    vector< Node* > args;
    Node *callee, 
        *node = makeNode(true, true);

    expectKeyword("new");
    callee = parseLeftHandSideExpression();
    if (match("(")) { 
        args = parseArguments(); 
    }

    node->finishNewExpression(callee, args);
    DEBUGOUT("parseNewExpr", false);
    return node;
}

//throw_
Node* ParseTools::parseLeftHandSideExpressionAllowCall() {
    DEBUGIN(" parseLeftHandSideExpressionAllowCall()", false);
    vector< Node * > args;
    Node *expr, *property, *tmpnode;
    ptrTkn startToken;
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
            tmpnode = makeWrappingNode(startToken);

            tmpnode->finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else if (match("(")) {
            args = parseArguments();
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishCallExpression(expr, args);
            expr = tmpnode;
        } else if (match("[")) {
            property = parseComputedMember();
            tmpnode = makeWrappingNode(startToken);            
            tmpnode->finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else {
            break;
        }
    }

    state.allowIn = previousAllowIn;

    DEBUGOUT("parseLeftHandSideExprAllow", false); 
    return expr; 
}

//throw_
Node* ParseTools::parseLeftHandSideExpression() {
    DEBUGIN(" parseLeftHandSideExpression()", false);
    Node *tmpnode, *expr, *property;
    ptrTkn startToken;

    task->softAssert(state.allowIn, 
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
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishMemberExpression(u'[', expr, property);
            expr = tmpnode;
        } else if (match(".")) {
            property = parseNonComputedMember();
            tmpnode = makeWrappingNode(startToken);
            tmpnode->finishMemberExpression(u'.', expr, property);
            expr = tmpnode;
        } else {            
            break;
        }
    }
    DEBUGOUT("parseLeftHandSideExpr", false);
    return expr;
}

// 11.3 Postfix Expressions
//throw_
Node* ParseTools::parsePostfixExpression() {
    DEBUGIN(" parsePostfixExpression()", false);
    Node *expr, *tmpnode;
    bool pltresult;
    ptrTkn token, startToken = lookahead;

    expr = parseLeftHandSideExpressionAllowCall();
    
    if (lookahead->type == TknType::Punctuator) {
        if (match("++") || match("--")) {
            pltresult = peekLineTerminator(); //#throw52
            if (!pltresult) {
                // 11.3.1, 11.3.2
                if (task->strict 
                    && expr->type == Syntax[Synt::Identifier] && 
                    Tokenizer::isRestrictedWord(expr->name)) {
                    task->throwErrorTolerant(NULLPTRTKN,
                                       Messages[Mssg::StrictLHSPostfix],{});
                }

                if (!isLeftHandSide(expr)) {
                    task->throwErrorTolerant(NULLPTRTKN,
                                       Messages[Mssg::InvalidLHSInAssignment],
                                       {});
                }

                token = scanner.lex();
                tmpnode = makeWrappingNode(startToken);
                tmpnode->finishPostfixExpression(token->strvalue, expr);
                DEBUGOUT("parsePostfix", false); 
                return tmpnode;
            }
        }
    }

    DEBUGOUT("parsePostfix", false);
    return expr;
}

// 11.4 Unary Operators
//throw_
Node* ParseTools::parseUnaryExpression() {
    DEBUGIN(" parseUnaryExpression()", false);
    ptrTkn token, startToken;
    Node *expr, *tmpnode;
    u16string exprname;

    if (lookahead->type != TknType::Punctuator 
        && lookahead->type != TknType::Keyword) {
        expr = parsePostfixExpression();
    } else if (match("++") || match("--")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        // 11.4.4, 11.4.5
        if (task->strict 
            && expr->type == Syntax[Synt::Identifier] 
            && Tokenizer::isRestrictedWord(expr->name)) {
            task->throwErrorTolerant(NULLPTRTKN, 
                                     Messages[Mssg::StrictLHSPrefix],{});
        }

        if (!isLeftHandSide(expr)) {
            task->throwErrorTolerant(NULLPTRTKN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else if (match("+") || match("-") || match("~") || match("!")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else if (matchKeyword("delete") 
               || matchKeyword("void") 
               || matchKeyword("typeof")) {
        startToken = lookahead;
        token = scanner.lex();
        expr = parseUnaryExpression();
        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishUnaryExpression(token->strvalue, expr);
        if (task->strict && token->strvalue == "delete" 
            && expr->type == Syntax[Synt::Identifier]) {
            task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::StrictDelete], {});
        }
        DEBUGOUT("parseUnary", false); 
        return tmpnode;
    } else {
        expr = parsePostfixExpression();
    }

    DEBUGOUT("parseUnary", false);
    return expr;
}

int ParseTools::binaryPrecedence(const TokenStruct *token, 
                     const bool allowIn) {
    DEBUGIN(" binaryPrecedence(Tokenstruct token, bool allowIn)", false);
    int prec = 0;
    string tokval;

    if (token->type != TknType::Punctuator 
        && token->type != TknType::Keyword) {
        DEBUGOUT("binaryPrec", false); 
        return 0;
    }
    tokval = token->strvalue;

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

    DEBUGOUT("binaryPrec", false);
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
Node* ParseTools::parseBinaryExpression() {
    DEBUGIN(" parseBinaryExpression()", false);

    Node *tmpnode, *expr, *left, 
        *right;
    vector < Node* > nodestack;
    ptrTkn marker, token, nulltok = scanner.makeToken();
    nulltok->isNull = true;
    vector< ptrTkn > markers, tokstack;
    string oper;
    int i, prec;

    marker = lookahead;

    left = parseUnaryExpression();

    if (left->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) {
        //? placeholder
        DEBUGOUT("parseBinary1", false); 
        return left;
    }

    token = lookahead;    
    prec = binaryPrecedence(token.get(), state.allowIn);
    if (prec == 0) {
        DEBUGOUT("parseBinary2", false); 
        return left;
    }
    token->prec = prec;
    scanner.lex();

    markers.push_back(marker);
    markers.push_back(lookahead);
    right = parseUnaryExpression();

    //we want to mirror esprima's js as close as possible. because it uses
    //a stack of mixed tokens or nodes (and the math is done so it always 
    //knows the type of the position its using) we've simply made two different
    //type stackeds (where a position is always only occupied by a non-null value in one of the stacks)
    //and access the right stack at the right time.
    nodestack.push_back(left);
    tokstack.push_back(nulltok);
    nodestack.push_back(NULLNODE);
    tokstack.push_back(token);
    nodestack.push_back(right);
    tokstack.push_back(nulltok);

    while ((prec = binaryPrecedence(lookahead.get(), state.allowIn)) > 0) { 

        // Reduce: make a binary expression from the three topmost entries.
        while ((tokstack.size() > 2) && 
               (prec <= tokstack[tokstack.size() - 2]->prec)) { 
            //? will this work the same in c++ as in js?
            right = nodestack.back(); 
            nodestack.pop_back(); tokstack.pop_back();
            oper = tokstack.back()->strvalue;
            nodestack.pop_back(); tokstack.pop_back();
            left = nodestack.back();
            nodestack.pop_back(); tokstack.pop_back();
            markers.pop_back();

            expr = makeWrappingNode(markers[markers.size() - 1]);

            expr->finishBinaryExpression(oper, left, right);

            nodestack.push_back(expr); tokstack.push_back(nulltok);
        }

        // Shift.
        token = scanner.lex();
        token->prec = prec;
        nodestack.push_back(NULLNODE);
        tokstack.push_back(token);
        markers.push_back(lookahead);
        expr = parseUnaryExpression();
        nodestack.push_back(expr);
        tokstack.push_back(nulltok);
    }

    // Final reduce to clean-up the stack.
    i = nodestack.size() - 1;
    expr = nodestack[i];
    markers.pop_back();
    while (i > 1) {
        tmpnode = makeWrappingNode(markers.back());
        markers.pop_back();
        tmpnode->finishBinaryExpression(tokstack[i - 1]->strvalue, 
                                        nodestack[i - 2], expr);
        expr = tmpnode;
        i -= 2;
    }

    DEBUGOUT("parseBinary3", false);
    return expr;
}


// 11.12 Conditional Operator

//throw_
Node* ParseTools::parseConditionalExpression() {
    DEBUGIN(" parseConditionalExpression()", false);
    Node *expr, *tmpnode, 
        *consequent, *alternate;
    bool previousAllowIn;
    ptrTkn startToken;

    startToken = lookahead;

    expr = parseBinaryExpression();
    if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) { 
        //? ever supposed to eval. to true? cause it might in some cases
        //? even tho it seems in javascript it never ever will.

        DEBUGOUT("parseCondExpr1", false); 
        return expr;
    }
    if (match("?")) {
        scanner.lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        state.allowIn = previousAllowIn;
        expect(":");
        alternate = parseAssignmentExpression();

        tmpnode = makeWrappingNode(startToken);
        tmpnode->finishConditionalExpression(expr, consequent, alternate);
        DEBUGOUT("parseCondExpr2", false); 
        return tmpnode;
    }

    DEBUGOUT("parseCondExpr3", false);
    return expr;
}

// [ES6] 14.2 Arrow Function
//throw_
Node* ParseTools::parseConciseBody() {
    DEBUGIN(" parseConciseBody()", false);
    if (match("{")) {
        return DBGRET("parseConciseBody", parseFunctionSourceElements());
    }
    return DBGRET("parseConciseBody", parseAssignmentExpression());
}

void ParseTools::validateParamNode(ReinterpretOptions& options,
                       Node *param, const string name) {
    DEBUGIN("validateParamNode(Reinterp, Node, u16str)", false);
    string key = "$";
    key.append(name);
    if (task->strict) {
        if (Tokenizer::isRestrictedWord(name)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamName];
        }
        if (hasSet(key, options.paramSet)) {
            options.stricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    } else if (options.firstRestricted == NULLNODE) {
        if (Tokenizer::isRestrictedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamName];
        } else if (Tokenizer::isStrictModeReservedWord(name)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictReservedWord];
        } else if (hasSet(key, options.paramSet)) {
            options.firstRestricted = param;
            options.message = Messages[Mssg::StrictParamDupe];
        }
    }
    options.paramSet.insert(key);
    DEBUGOUT("validateParamNode", false);
}

//throw_
ReinterpretOut ParseTools::reinterpretAsCoverFormalsList(
                                    vector< Node* >& expressions) { 
    DEBUGIN("reinterpretAsCover", false);
    int i, len, defaultCount;
    Node *param;
    vector< Node* > params;
    vector< Node* > defaults;
    ReinterpretOptions opts;
    ReinterpretOut reOut; 
    void* rest;

    defaultCount = 0;
    rest = nullptr;

    for (i = 0, len = expressions.size(); i < len; i += 1) {
        param = expressions[i];
        if (param->type == Syntax[Synt::Identifier]) {
            params.push_back(param);
            defaults.push_back(NULLNODE);
            //null defaults might be skipped instead of regged check this.
            validateParamNode(opts, param, param->name);
        } else if (param->type == Syntax[Synt::AssignmentExpression]) {
            Node *l = param->leftAssign; 
            Node *r = param->rightAssign; 

            params.push_back(l);
            defaults.push_back(r);
            ++defaultCount;
            validateParamNode(opts, l, l->name);
        } else {
            reOut.isNull = true;
            DEBUGOUT("", false); 
            return reOut; 
        }
    }

    if (opts.message == Messages[Mssg::StrictParamDupe]) {
        task->throwError(
                   //strict ? opts.stricted : opts.firstRestricted, //? replicate?
                   NULLPTRTKN,
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

    DEBUGOUT("reinterpret", false);
    return reOut;
}

//throw_
Node* ParseTools::parseArrowFunctionExpression(
                                   const ReinterpretOut options, 
                                   Node *node) {
    DEBUGIN(" parseArrowFunctionExpression", false);
    bool previousStrict;
    Node *body;

    expect("=>");
    previousStrict = task->strict;

    body = parseConciseBody();

    if (task->strict && options.firstRestricted != NULLNODE) { 
        task->throwError(NULLPTRTKN, options.message, {});
        //        task->throwError(options.firstRestricted, options.message, {});
    }
    if (task->strict && options.stricted != NULLNODE) {
        task->throwError(NULLPTRTKN, options.message, {});
        //        task->throwErrorTolerant(options.stricted, options.message, {});
    }

    task->strict = previousStrict;

    node->finishArrowFunctionExpression(options.params, 
                                       options.defaults, 
                                       body, 
                                       body->type != Syntax[Synt::BlockStatement]);
    DEBUGOUT("parseArrowFuncExpr", false);
    return node;
}

    // 11.13 Assignment Operators


//throw_
Node* ParseTools::parseAssignmentExpression() {
    DEBUGIN(" parseAssignmentExpression()", false);
    int oldParenthesisCount;
    ptrTkn token, startToken;
    Node *expr, *right, *tmpnode;
    vector<Node*> reIn;
    ReinterpretOut list;


    oldParenthesisCount = state.parenthesisCount;

    startToken = lookahead;
    token = lookahead;

    expr = parseConditionalExpression();
    list.isNull = true;
    if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type //? will work?
        || match("=>")) {

        if (state.parenthesisCount == oldParenthesisCount ||
            state.parenthesisCount == (oldParenthesisCount + 1)) {      
            if (expr->type == Syntax[Synt::Identifier]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn); 
            } else if (expr->type == Syntax[Synt::AssignmentExpression]) {
                reIn.push_back(expr);
                list = reinterpretAsCoverFormalsList(reIn);
                expr->unused();
            } else if (expr->type == Syntax[Synt::SequenceExpression]) {
                list = reinterpretAsCoverFormalsList(expr->expressions);
                expr->unused();
            } else if (expr->type == PlaceHolders["ArrowParameterPlaceHolder"]->type) {
                expr->unused();
                list = reinterpretAsCoverFormalsList(reIn); 
            }
            if (!(list.isNull)) {
                return DBGRET("parseAssignExpr1", parseArrowFunctionExpression(list, makeWrappingNode(startToken)));
            }
        }
    }

    if (matchAssign()) {
        // LeftHandSideExpression
        if (!isLeftHandSide(expr)) {
            task->throwErrorTolerant(NULLPTRTKN, 
                               Messages[Mssg::InvalidLHSInAssignment], {});
        }

        // 11.13.1
        if (task->strict 
            && expr->type == Syntax[Synt::Identifier] 
            && Tokenizer::isRestrictedWord(expr->name)) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictLHSAssignment], {});
        }

        token = scanner.lex();
        right = parseAssignmentExpression();
        tmpnode = makeWrappingNode(startToken);

        tmpnode->finishAssignmentExpression(token->strvalue, expr, right); 
        DEBUGOUT("parseAssignExpr2", false); 
        return tmpnode;
    }

    DEBUGOUT("parseAssignExpr3", false); 
    return expr;
}

// 11.14 Comma Operator
//throw_
Node* ParseTools::parseExpression() { 
    DEBUGIN(" parseExpression()", false);
    Node *expr; 
    ptrTkn startToken = lookahead;
    vector< Node* > expressions;

    expr = parseAssignmentExpression();

    if (match(",")) {
        expressions.push_back(expr);
        while (idx < length) {
            if (!match(",")) {
                break;
            }
            scanner.lex();
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
            expressions.push_back(tmp);
#endif
#ifdef THROWABLE
            expressions.push_back(parseAssignmentExpression());
#endif
        }

        expr = makeWrappingNode(startToken);
        expr->finishSequenceExpression(expressions);
    }

    DEBUGOUT("parseExpr", false); 
    return expr;
}

// 12.1 Block
//throw_
vector< Node* > ParseTools::parseStatementList() { 
    DEBUGIN(" parseStatementList()", false);
    vector< Node* > list;
    Node *statement;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement == NULLNODE) { 
            break;
        }
        list.push_back(statement);
    }

    DEBUGOUT("parseStatementList", false); 
    return list;
}

//throw_
Node* ParseTools::parseBlock() { 
    DEBUGIN(" parseBlock()", false);
    vector< Node* > block;
    Node *node = makeNode(true, true);

    expect("{");
    block = parseStatementList();
    expect("}");
    node->finishBlockStatement(block);
    DEBUGOUT("parseBlock", false); 
    return node;
}


// 12.2 Variable Statement

//throw_
Node* ParseTools::parseVariableIdentifier() { 
    DEBUGIN(" parseVariableIdentifier()", false);
    ptrTkn token;
    Node *node = makeNode(true, true);

    token = scanner.lex();

    if (token->type != TknType::Identifier) {
        task->throwUnexpected(token);
    }

    node->finishIdentifier(token->strvalue);
    DEBUGOUT("parseVariableIdent", false); 
    return node;
}

//throw_
Node* ParseTools::parseVariableDeclaration(const StrRef &kind) { 
    DEBUGIN(" parseVariableDeclaration(u16string kind)", false);
    Node *id, *init,
        *node = makeNode(true, true);
    init = NULLNODE;

    id = parseVariableIdentifier();

    // 12.2.1
    if (task->strict && Tokenizer::isRestrictedWord(id->name)) {
        task->throwErrorTolerant(NULLPTRTKN, 
                                 Messages[Mssg::StrictVarName], {});
    }
    if (kind == text::_const) {
        expect("=");
        init = parseAssignmentExpression();
    } else if (match("=")) {
        scanner.lex();
        init = parseAssignmentExpression();
    }

    node->finishVariableDeclarator(id, init);
    DEBUGOUT("parseVariableDecl", false); 
    return node;
}

//throw_
vector< Node* > ParseTools::parseVariableDeclarationList(const StrRef &kind) {
    DEBUGIN("parseVariableDeclarationList", false);
    vector< Node* > list; 

    do {
#ifndef THROWABLE
        Node *tmp = parseVariableDeclaration(kind);
        list.push_back(tmp);
#endif
#ifdef THROWABLE
        list.push_back(parseVariableDeclaration(kind));
#endif
        if (!match(",")) {
            break;
        }
        scanner.lex();
    } while (idx < length);

    DEBUGOUT("parseVariableDeclarationList", false); 
    return list;
}

//throw_
Node* ParseTools::parseVariableStatement(Node* node) { 
    DEBUGIN(" parseVariableStatement(Node node)", false);
    vector< Node* > declarations;

    expectKeyword("var");
    declarations = parseVariableDeclarationList(EMPTY_STRREF); 
    consumeSemicolon();

    node->finishVariableDeclaration(declarations, text::_var);
    DEBUGOUT("parseVariableStatement", false); 
    return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//throw_
Node* ParseTools::parseConstLetDeclaration(const string kind, 
                                           const StrRef &kindref) { 
    DEBUGIN(" parseConstLetDeclaration(u16string kind)", false);
    vector< Node* > declarations;
    Node *node = makeNode(true, true);

    expectKeyword(kind);
    declarations = parseVariableDeclarationList(kindref);
    consumeSemicolon();
    node->finishVariableDeclaration(declarations, kindref);
    DEBUGOUT("parseConstLetDeclaration", false); 
    return node;
}

// 12.3 Empty Statement

//throw_
Node* ParseTools::parseEmptyStatement() { 
    DEBUGIN(" parseEmptyStatement()", false);
    Node *node = makeNode(true, true);
    expect(";");
    node->finishEmptyStatement();
    DEBUGOUT("parseEmptyStatement", false); 
    return node;
}

// 12.4 Expression Statement
//throw_
Node* ParseTools::parseExpressionStatement(Node *node) { 
    DEBUGIN(" parseExpressionStatement(Node node)", false);
    Node *expr;
    expr = parseExpression();
    consumeSemicolon();
    node->finishExpressionStatement(expr);
    DEBUGOUT("", false); 
    return node;
}

// 12.5 If statement
//throw_
Node* ParseTools::parseIfStatement(Node *node) { 
    DEBUGIN(" parseIfStatement(Node node)", false);
    Node *test, 
        *consequent, *alternate;
    expectKeyword("if");
    expect("(");
    test = parseExpression();
    expect(")");
    consequent = parseStatement();
    if (matchKeyword("else")) {
        scanner.lex();
        alternate = parseStatement();
    } else {
        alternate = NULLNODE;
    }
    node->finishIfStatement(test, consequent, alternate);
    DEBUGOUT("parseIfStatement", false); 
    return node;
}

// 12.6 Iteration Statements

//throw_
Node* ParseTools::parseDoWhileStatement(Node *node) { 
    DEBUGIN(" parseDoWhileStatement(Node node)", false);
    Node *body, *test;
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
        scanner.lex();
    }
    node->finishDoWhileStatement(body, test);
    DEBUGOUT("parseDoWhileStatement", false);
    return node;
}

//throw_
Node* ParseTools::parseWhileStatement(Node* node) { 
    DEBUGIN(" parseWhileStatement(Node node)", false);
    Node *test, *body;
    bool oldInIteration;
    expectKeyword("while");
    expect("(");
    test = parseExpression();
    expect(")");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    state.inIteration = oldInIteration;
    node->finishWhileStatement(test, body);
    DEBUGOUT("parseWhileStatement", false);
    return node;
}

//throw_
Node* ParseTools::parseForVariableDeclaration() { 
    DEBUGIN(" parseForVariableDeclaration()", false);
    ptrTkn token;
    vector< Node* > declarations;
    Node *node = makeNode(true, true);

    token = scanner.lex();
    declarations = parseVariableDeclarationList(EMPTY_STRREF);
    node->finishVariableDeclaration(declarations, (token->strvalue == "var")? text::_var: ((token->strvalue == "const")? text::_const : text::_let));
    DEBUGOUT("parseForVariableDeclaration", false); 
    return node;
}

//throw_
Node* ParseTools::parseForStatement(Node* node) { 
    DEBUGIN(" parseForStatement(Node node)", false);

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node *body, *left, *right =0x0, 
        *update, *test, *init;
    left=NULLNODE;
    update=NULLNODE; test=NULLNODE; init=NULLNODE;

    expectKeyword("for");
    expect("(");

    if (match(";")) {
        scanner.lex();
    } else {
        if (matchKeyword("var") || matchKeyword("let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (init->jv[text::_declarations].Size() == 1 
                && matchKeyword("in")) { 

                scanner.lex();
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
                    task->throwErrorTolerant(NULLPTRTKN, 
                                       Messages[Mssg::InvalidLHSInForIn],
                                       {});
                }

                scanner.lex();
                left = init;
                right = parseExpression();
                init = NULLNODE;
            }
        }

        if (left == NULLNODE) {
            expect(";");
        }
    }

    if (left == NULLNODE) {

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

    if (left == NULLNODE) {
        node->finishForStatement(init, test, update, body);
    } else {
        node->finishForInStatement(left, right, body);
    }
    DEBUGOUT("parseForStatement", false);
    return node;
}

// 12.7 The continue statement
//throw_
Node* ParseTools::parseContinueStatement(Node* node) {
    DEBUGIN(" parseContinueStatement(Node node)", false);
    Node *label;
    label = NULLNODE;
    string key;
    bool pltresult;

    expectKeyword("continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!state.inIteration) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(NULLNODE);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {

        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);
        if (!(hasSet(key, state.labelSet))) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == NULLNODE && !(state.inIteration)) {
        task->throwError(NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
    }

    node->finishContinueStatement(label);
    DEBUGOUT("parseContinueStatement", false);
    return node;
}

// 12.8 The break statement
//throw_
Node* ParseTools::parseBreakStatement(Node* node) {
    DEBUGIN(" parseBreakStatement(Node node)", false);
    Node *label;
    string key;
    bool pltresult;
    label = NULLNODE;

    expectKeyword("break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(NULLNODE);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {
        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);

        if (!(hasSet(key, state.labelSet))) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == NULLNODE && !(state.inIteration || state.inSwitch)) {
        task->throwError(NULLPTRTKN, Messages[Mssg::IllegalBreak], {});
    }

    node->finishBreakStatement(label);
    DEBUGOUT("parseBreakStatement", false);
    return node;
}

// 12.9 The return statement
//throw_
Node* ParseTools::parseReturnStatement(Node* node) {
    DEBUGIN(" parseReturnStatement(Node node)", false);
    Node *argument;
    bool pltresult;
    argument = NULLNODE;

    expectKeyword("return");

    if (!(state.inFunctionBody)) {
        task->throwErrorTolerant(NULLPTRTKN, Messages[Mssg::IllegalReturn], {});
    }

    // 'return' followed by a space and an identifier is very common.
    if (source(idx) == 0x20) {
        if (Tokenizer::isIdentifierStart(source(idx + 1))) {
            argument = parseExpression();
            consumeSemicolon();
            node->finishReturnStatement(argument);
            DEBUGOUT("parseReturnStatement", false); 
            return node;
        }
    }

    pltresult = peekLineTerminator();
    if (pltresult) {
        node->finishReturnStatement(NULLNODE);
        DEBUGOUT("parseReturnStatement", false);
        return node;
    }

    if (!match(";")) {
        if (!match("}") && lookahead->type != TknType::EOFF) {
            argument = parseExpression();
        }
    }

    consumeSemicolon();

    node->finishReturnStatement(argument);
    DEBUGOUT("parseReturnStatement", false);
    return node;
}

// 12.10 The with statement
//throw_
Node* ParseTools::parseWithStatement(Node* node) {
    DEBUGIN(" parseWithStatement(Node node)", false);
    Node *object, *body;

    if (task->strict) {
        // TODO(ikarienator): Should we update the test cases instead?
        scanner.skipComment(); //ev
        task->throwErrorTolerant(NULLPTRTKN, 
                           Messages[Mssg::StrictModeWith], {});
    }

    expectKeyword("with");
    expect("(");
    object = parseExpression();
    expect(")");
    body = parseStatement();
    node->finishWithStatement(object, body);
    DEBUGOUT("parseWithStatement", false);
    return node;
}

// 12.10 The swith statement
//throw_
Node* ParseTools::parseSwitchCase() {
    DEBUGIN(" parseSwitchCase()", false);
    Node *test, *statement, 
        *node = makeNode(true, true);
    vector< Node* > consequent;

    if (matchKeyword("default")) {
        scanner.lex();
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

    node->finishSwitchCase(test, consequent);
    DEBUGOUT("parseSwitchCase", false);
    return node;
}

//throw_
Node* ParseTools::parseSwitchStatement(Node *node) {
    DEBUGIN(" parseSwitchStatement(Node node)", false);
    Node *discriminant, *clause; 
    vector< Node* > cases; 
    bool oldInSwitch, defaultFound;

    expectKeyword("switch");
    expect("(");
    discriminant = parseExpression();
    expect(")");
    expect("{");
    if (match("}")) {
        scanner.lex();
        node->finishSwitchStatement(discriminant, cases);
        DEBUGOUT("parseSwitchStatement", false); 
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
        if (clause->jv[text::_test].IsNull()) {
            if (defaultFound) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::MultipleDefaultsInSwitch],{});
            }
            defaultFound = true;
        }
        cases.push_back(clause);
    }

    state.inSwitch = oldInSwitch;
    expect("}");
    node->finishSwitchStatement(discriminant, cases);
    DEBUGOUT("parseSwitchStatement", false);
    return node;
}

// 12.13 The throw statement
//throw_
Node* ParseTools::parseThrowStatement(Node* node) {
    DEBUGIN(" parseThrowStatement", false);
    Node* argument;
    bool pltresult;

    expectKeyword("throw");
    pltresult = peekLineTerminator();
    if (pltresult) {
        task->throwError(NULLPTRTKN, 
                   Messages[Mssg::NewlineAfterThrow],{});
    }
    argument = parseExpression();
    consumeSemicolon();
    node->finishThrowStatement(argument);
    DEBUGOUT("parseThrowStatement", false);
    return node;
}

// 12.14 The try statement

//throw_
Node* ParseTools::parseCatchClause() {
    DEBUGIN(" parseCatchClause()", false);
    Node *body, *param, 
        *node = makeNode(true, true);

    expectKeyword("catch");

    expect("(");
    if (match(")")) {
        task->throwUnexpected(lookahead);
    }

    param = parseVariableIdentifier();
    // 12.14.1
    if (task->strict && Tokenizer::isRestrictedWord(param->name)) { 
        task->throwErrorTolerant(NULLPTRTKN, 
                           Messages[Mssg::StrictCatchVariable],{});
    }

    expect(")");
    body = parseBlock();
    node->finishCatchClause(param, body);
    DEBUGOUT("parseCatchClause", false);
    return node;
}

//throw_
Node* ParseTools::parseTryStatement(Node* node) {
    DEBUGIN(" parseTryStatement(Node node)", false);
    Node *block, *finalizer; 
    vector< Node* > handlers;

    finalizer = NULLNODE;

    expectKeyword("try");

    block = parseBlock();

    if (matchKeyword("catch")) {
#ifndef THROWABLE
        Node *tmp = parseCatchClause();
        handlers.push_back(tmp);
#endif
#ifdef THROWABLE
        handlers.push_back(parseCatchClause());
#endif
    }

    if (matchKeyword("finally")) {
        scanner.lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && finalizer == NULLNODE) {
        task->throwError(NULLPTRTKN, Messages[Mssg::NoCatchOrFinally], {});
    }

    node->finishTryStatement(block, EMPTY_NODE_LIST, 
                            handlers, finalizer); 
    DEBUGOUT("parseTryStatement", false);
    return node;
}

// 12.15 The debugger statement

//throw_
Node* ParseTools::parseDebuggerStatement(Node* node) {
    DEBUGIN(" parseDebuggerStatement(Node node)", false);
    expectKeyword("debugger");
    consumeSemicolon();
    node->finishDebuggerStatement();
    DEBUGOUT("parseDebuggerStatement", false);
    return node;
}

// 12 Statements

//#partial
//throw_
Node* ParseTools::parseStatement() {
    DEBUGIN(" parseStatement()", false);
    TknType type = lookahead->type;
    string key, tokval;
    Node *expr, *node, *labeledBody;

    if (type == TknType::EOFF) {
        task->throwUnexpected(lookahead);
    }

    if (type == TknType::Punctuator && lookahead->strvalue == "{") {
        return DBGRET("parseStatement", parseBlock());
    }
    
    node = makeNode(true, true);

    if (type == TknType::Punctuator) {
        tokval = lookahead->strvalue;
        if (tokval == ";") {
            node->unused();
            return DBGRET("parseStatement", parseEmptyStatement());
        } else if (tokval == "(") {
            return DBGRET("parseStatement", parseExpressionStatement(node));
        }
    } else if (type == TknType::Keyword) {
        tokval = lookahead->strvalue;
        if (tokval == "break") {
            return DBGRET("parseStatement", parseBreakStatement(node));
        } else if (tokval == "continue") {
            return DBGRET("parseStatement", parseContinueStatement(node));
        } else if (tokval == "debugger") {
            return DBGRET("parseStatement", parseDebuggerStatement(node));
        } else if (tokval == "do") {
            return DBGRET("parseStatement", parseDoWhileStatement(node));
        } else if (tokval == "for") {
            return DBGRET("parseStatement", parseForStatement(node));
        } else if (tokval == "function") {
            //#oddly enough in js passes node here.
            //#even though has no param even in js.
            return DBGRET("parseStatement", parseFunctionDeclaration());
        } else if (tokval == "if") {
            return DBGRET("parseStatement", parseIfStatement(node));
        } else if (tokval == "return") {
            return DBGRET("parseStatement", parseReturnStatement(node));
        } else if (tokval == "switch") {
            return DBGRET("parseStatement", parseSwitchStatement(node));
        } else if (tokval == "throw") {
            return DBGRET("parseStatement", parseThrowStatement(node));
        } else if (tokval == "try") {
            return DBGRET("parseStatement", parseTryStatement(node));
        } else if (tokval == "var") {
            return DBGRET("parseStatement", parseVariableStatement(node));
        } else if (tokval == "while") {
            return DBGRET("parseStatement", parseWhileStatement(node));
        } else if (tokval == "with") {
            return DBGRET("parseStatement", parseWithStatement(node));
        }
    }

    expr = parseExpression(); 

    // 12.12 Labelled Statements
    if ((expr->type == Syntax[Synt::Identifier]) && match(":")) {
        scanner.lex();

        key = "$";
        key.append(expr->name);

        if (hasSet(key, state.labelSet)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::Redeclaration], 
                       {"Label", expr->name}); 
        }
        state.labelSet.insert(key);
        labeledBody = parseStatement();
        state.labelSet.erase(key);
        node->finishLabeledStatement(expr, labeledBody);
        DEBUGOUT("parseStatement", false); 
        return node;
    }

    consumeSemicolon();

    node->finishExpressionStatement(expr);
    DEBUGOUT("parseStatement", false);
    return node;
}

// 13 Function Definition

//throw_
Node* ParseTools::parseFunctionSourceElements() {
    DEBUGIN(" parseFunctionSourceElements()", false);
    Node *sourceElement, 
        *node = makeNode(true, true);
    vector< Node* > sourceElements;
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;
    StateStruct oldstate;

    expect("{");

    firstRestricted->isNull = true;
    while (idx < length) {
        if (lookahead->type != TknType::StringLiteral) {
            break;
        }
        token = lookahead;

        sourceElement = parseSourceElement(); 
        //# returns in turn the value of parseStatement for stringLiteral 
        //# so returns a string literal expression node wrapped in an expressionStatement node.
        sourceElements.push_back(sourceElement); 
        if (sourceElement->spareStrref != Syntax[Synt::Literal]) {
            //? this one I doubt there's more an efficient way to do this
            //? then json-c accesses. Storing node hierarchies just to fix this call seems to 
            //? be likely less performant.
            // this is not directive
            break;
        }
        directive = slice(sourceRaw, token->start + 1, token->end - 1);
        if (directive == u"use strict") {
            task->strict = true;
            if (!(firstRestricted->isNull)) {
                task->throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral], {});
            }
        } else {
            if (firstRestricted->isNull && token->octal) {
                firstRestricted = token;
                firstRestricted->isNull = false;
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
        if (sourceElement == NULLNODE) {
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

    node->finishBlockStatement(sourceElements);
    DEBUGOUT("parseFunctionSourceElements", false);
    return node;
}

//throw_ 
void ParseTools::validateParam(ParseParamsOptions& options, 
                               ptrTkn param, const string name) {
     DEBUGIN("validateParam", false);
     string key = "$";
     key.append(name);
     if (task->strict) {
         if (Tokenizer::isRestrictedWord(name)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamName];
         }

         if (hasSet(key, options.paramSet)) {
             options.stricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     } else if (options.firstRestricted->isNull) {
         if (Tokenizer::isRestrictedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamName];
         } else if (Tokenizer::isStrictModeReservedWord(name)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictReservedWord];
         } else if (hasSet(key, options.paramSet)) {
             options.firstRestricted = param;
             options.message = Messages[Mssg::StrictParamDupe];
         }
     }
     options.paramSet.insert(key);
     DEBUGOUT("validateParam", false);
     return;
}


//throw_ 
bool ParseTools::parseParam(ParseParamsOptions& options) {
    DEBUGIN(" parseParam(ParseParamsOptions options)", false);
    ptrTkn token; 
    Node *param, *def;

    token = lookahead;
    param = parseVariableIdentifier();
    //printf("token strvalue %s \n", (toU8string(token.strvalue)).data());
    //! MAJOR CONCERN: if a number is put where an argument is here,
    //! will that cause an ungraceful crash because validateParam is expecting
    //! a string so we cast it here, but the token grabbed might be
    //! a numeric literal?? if so perhaps the thing to do is check
    //! for token.type and if it is of numeric types cast it?
    validateParam(options, token, token->strvalue);

    if (match("=")) {
        scanner.lex();
        def = parseAssignmentExpression();
        ++(options.defaultCount);
        options.defaults.push_back(def);
    }

    options.params.push_back(param);

    return DBGRET("parseParam", !match(")"));
}

//throw_ 
ParseParamsOut ParseTools::parseParams(ptrTkn firstRestricted) {
    DEBUGIN(" parseParamS(ptrTkn firstRestricted)", false);
    ParseParamsOptions options;
    ParseParamsOut out;
    options.defaultCount = 0;
    options.firstRestricted = firstRestricted;
    if (options.stricted != 0x0)
        { options.stricted->isNull = true; }

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
    DEBUGOUT("parseParamS", false);
    return out;
}

//throw_ 
Node* ParseTools::parseFunctionDeclaration() {
    DEBUGIN(" parseFunctionDeclaration()", false);
    ptrTkn token, stricted, firstRestricted = scanner.makeToken();
    string message;
    Node *body, *id, 
        *node = makeNode(true, true);    
    ParseParamsOut tmp;
    vector< Node* > params;
    vector< Node* > defaults;
    bool previousStrict;
    id = NULLNODE;

    expectKeyword("function");

    token = lookahead;
    id = parseVariableIdentifier(); 

    firstRestricted->isNull = true;
    if (task->strict) {
        if (Tokenizer::isRestrictedWord(token->strvalue)) {
            task->throwErrorTolerant(token, 
                               Messages[Mssg::StrictFunctionName], {});
        }
    } else {
        if (Tokenizer::isRestrictedWord(token->strvalue)) {
            firstRestricted = token;
            message = Messages[Mssg::StrictFunctionName];
        } else if (Tokenizer::isStrictModeReservedWord(token->strvalue)) {
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

    previousStrict = task->strict;
    body = parseFunctionSourceElements();
    if (task->strict 
        && firstRestricted != 0x0 
        && !(firstRestricted->isNull)) {
        task->throwError(firstRestricted, message, {});
    }
    if (task->strict 
        && stricted != 0x0 
        && !(stricted->isNull)) {
        task->throwErrorTolerant(stricted, message, {});
    }
    task->strict = previousStrict;

    node->finishFunctionDeclaration(id, params, defaults, body);
    DEBUGOUT("parseFunctionDecl", false);
    return node;
} 

//throw_ 
Node* ParseTools::parseFunctionExpression() {
    DEBUGIN(" parseFunctionExpression()", false);
    ptrTkn token, firstRestricted = scanner.makeToken(), stricted;
    firstRestricted->isNull = true;
    string message, tokval;
    Node *body, *id;
    Node
        *node = makeNode(true, true);    
    ParseParamsOut tmp;
    vector< Node* > params;
    vector< Node* > defaults;
    bool previousStrict;

    id = NULLNODE;
    expectKeyword("function");

    if (!match("(")) {
        token = lookahead;
        id = parseVariableIdentifier(); 

        tokval = token->strvalue;
        if (task->strict) {
            if (Tokenizer::isRestrictedWord(tokval)) {
                task->throwErrorTolerant(token, 
                                   Messages[Mssg::StrictFunctionName],{});
            }
        } else {
            if (Tokenizer::isRestrictedWord(tokval)) {
                firstRestricted = token;
                message = Messages[Mssg::StrictFunctionName];
            } else if (Tokenizer::isStrictModeReservedWord(tokval)) {
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

    previousStrict = task->strict;
    body = parseFunctionSourceElements();
    if (task->strict && !(firstRestricted->isNull)) {
        task->throwError(firstRestricted, message, {});
    }

    if (task->strict && !(stricted->isNull)) {
        task->throwErrorTolerant(stricted, message, {});
    }
    task->strict = previousStrict;
    node->finishFunctionExpression(id, params, defaults, body);

    DEBUGOUT("parseFuncExpr", false);
    return node;
}

    // 14 Program

//throw_ 
Node* ParseTools::parseSourceElement() {
    DEBUGIN(" parseSourceElement()", false);
    string val;
    if (lookahead->type == TknType::Keyword) {
        val = lookahead->strvalue;
        if (val == "const" || val == "let") {
            if (val == "const") {
                return DBGRET("", parseConstLetDeclaration("const", 
                                                           text::_const));
            } else {
                return DBGRET("", parseConstLetDeclaration("let", 
                                                           text::_let));
            }
        } else if (val == "function") {
            return DBGRET("", parseFunctionDeclaration()); 
        } else {

            return DBGRET("", parseStatement());
        }
    }

    if (lookahead->type != TknType::EOFF) {
        return DBGRET("", parseStatement());
    }

    DEBUGOUT("parseSourceElement", false);
    return NULLNODE;
}

//throw_ 
vector< Node* > ParseTools::parseSourceElements() {
    DEBUGIN(" parseSourceElementS() ", false);
    Node *sourceElement;
    vector< Node* > sourceElements;
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;

    firstRestricted->isNull = true;
    while (idx < length) {
        token = lookahead;
        if (token->type != TknType::StringLiteral) {
            break;
        }

        sourceElement = parseSourceElement();
        sourceElements.push_back(sourceElement);
        //#todo make a function that accepts vector of nested finds
        //#so we can make tests like this more legible.

        if (sourceElement->spareStrref != Syntax[Synt::Literal]) {
            // this is not directive
            break;
        }
        directive = slice(sourceRaw, token->start + 1, token->end - 1);
        if (directive == u"use strict") {
            task->strict = true;

            if (!(firstRestricted->isNull)) { 

                task->throwErrorTolerant(firstRestricted, 
                                   Messages[Mssg::StrictOctalLiteral],{});
            }
        } else {
            if (firstRestricted->isNull && token->octal) {
                firstRestricted = token;
                firstRestricted->isNull = false; //#probably not neces.
            }
        }
    }

    while (idx < length) {
        sourceElement = parseSourceElement();

        if (sourceElement == NULLNODE) {
            break;
        }
        sourceElements.push_back(sourceElement);
    }

    DEBUGOUT("parseSourceElementS", false);
    return sourceElements;
}

//throw_ 
Node* ParseTools::parseProgram() {
    DEBUGIN(" parseProgram()", false);
    Node *node;
    vector< Node* > body;

    scanner.skipComment(); //ev
    scanner.peek();
    node = makeNode(true, true);
    task->strict = false;
    body = parseSourceElements();
    node->finishProgram(body);

    DEBUGOUT("parseProgram", false);
    return node;
}

void Tokenizer::filterTokenLocation() {
    return;
    /*    DEBUGIN(" filterTokenLocation()", false);
    TokenRecord token(0,0,0),entry;
    vector<TokenRecord> tokens;

    for (unsigned i = 0; i < extra.tokenRecords.size(); ++i) {
        entry = extra.tokenRecords[i];
        token.type = entry.type;
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

    DEBUGOUT("filterToken", false);    
    return; */
}




#ifdef LOWMEM


int getDecodeIdx(char in) {
    if (in >= '0' && in <= '9') {
        return in - '0';
    } else if (in >= 'A' && in <= 'Z') {
        return (in - 'A') + 10;
    } else if (in >= 'a' && in <= 'z') {
        return (in - 'a') + 36;
    } else {
        return -1;
    }
}

/* 

JsonDecompressor.decompress is a function that takes a json
and for each key, and string value that does not follow 
particular keys, uses a dictionary (implemented as an 
array) to replace that key or string value with an 
original. We can afford to do this because in the js ast,
the circumstances under which a string value may be 
arbitrary or not worth compressing 

1. don't have overlap with those good for compression.
ie. 
  a. for string values which include arbitrary user input 
  such as string literals, there aren't many strings which 
  predictably show up at high frequency those that do are 
  not long enough and/or predictable enough to justify 
  compression.
  b. operators which, almost never would experience savings 
  via compression have their own distinct key they 
  correspond to, so they take up a small amount of space 
  while we can use their own characters to represent a larger
  number of vars.

2. inicate themselves clearly, from previous characters at 
  set relative positions (in a maximally compressed json) 
  so you can tell if a string value exists in such a position.

*/

char *asmRetVal(0x0);

class JsonDecompressor {
public:
    typedef char Ch;


    JsonDecompressor(
#ifdef LIMITJSON
                     vector<string> * completeObjs, 
#endif
                     long len);
    void Put(Ch c);
    void Flush() { };
    void decompress(char *&out, long &lenOut);
    
    //dummy functions
    Ch Peek() const { assert(false); return '\0'; }
    Ch Take() { assert(false); return '\0'; }
    size_t Tell() const { return 0; }
    Ch* PutBegin() { assert(false); return 0; }
    size_t PutEnd(Ch*) { assert(false); return 0; }
private:
    const long MAX_BLOCK_SIZE = 200000;
    long len;
    long blockSize;

    long i;
    vector<char*> blocks;
    char * current;

#ifdef LIMITJSON
    int objExpandSeq=0;
    vector<string> * completeObjects;

    const int OBJ_NONE = 0;
    const char OBJ_MARKER_BEGIN=1;
    const char OBJ_MARKER_END=5;
    const int OBJ_GET_ADDR =7;
    const char *MARKER = "\"#`@$";

    //use vector to avoid another include.
    vector<const char*> putStack;
    vector<int> putStackLen;
    vector<int> putStackPos;
    int addr;
    //    OStreamWrapper(const OStreamWrapper&);
    //OStreamWrapper& operator=(const OStreamWrapper&);    
#endif
};


JsonDecompressor::JsonDecompressor(
#ifdef LIMITJSON
                                   vector<string> * completeObjs,
#endif
                                   long lenArg)
#ifdef LIMITJSON
    : completeObjects(completeObjs) 
#endif
{
    len = lenArg+50;//+50 for a basic floor.
    //ideally, we want most calls to fit within one block.
    //because there's way more allocated mem
    //to going over a little bit (and way more alloc calls
    // for setting block size low) then by going
    // under by half.
    blockSize = (len) *90; 
    if (blockSize > MAX_BLOCK_SIZE) {
        blockSize = MAX_BLOCK_SIZE;
    }
    //printf("received input length of %li , calculated blockSize of %li \n", len,blockSize);
    current = 0x0;
    i = blockSize;    
}
void JsonDecompressor::Put(char in) {
#ifndef LIMITJSON
    if (i == blockSize) {
        current = (char *) malloc(blockSize);
        blocks.push_back(current);
        i = 0;
    }
    current[i] = in;
    ++i;    
#endif
#ifdef LIMITJSON
    //printf("\n%c|",in);
    putStack.push_back(&in);
    putStackLen.push_back(1);
    putStackPos.push_back(0);
    char c = in;

    //review:
    //current: current block
    //i: current position in block
    //c: current character in source document or expanded string
    //    expanded string is read right after quotation following
    //    addr is read.
    //putStack: stack of expanded strings.
    int inpos=0;
    while (! putStack.empty()) {
        if (i == blockSize) {
            current = (char *) malloc(blockSize);
            blocks.push_back(current);
            i = 0;
        }
        if (inpos == putStackLen.back()) {
            putStack.pop_back();
            putStackLen.pop_back();
            putStackPos.pop_back();
            if (putStack.empty()) {
                return;
            }
            inpos = putStackPos.back();
            //printf(" stackframe complete adding end bracket }\n");
            //current[i] = '}';
            //++i;
            continue;
        }
        //printf("\ndepth %i pos %i out of len %i \n", (int) putStackPos.size(), (int) inpos, (int) putStackLen.back());
        c = putStack.back()[inpos];
        //printf(" -%c-",c);

        //printf("seq %i ", (int) objExpandSeq);
        if (objExpandSeq == OBJ_NONE && 
            c != '[' &&
            c != ',' &&
            c != ':') {

            current[i] = c;
            ++i;
            ++inpos;
            continue;
        } else if (objExpandSeq == OBJ_NONE) {
            //printf(" found init  ");
            objExpandSeq = OBJ_MARKER_BEGIN;
        } else if (objExpandSeq >= OBJ_MARKER_BEGIN &&
                   objExpandSeq <= OBJ_MARKER_END) {
            if (c == MARKER[objExpandSeq-OBJ_MARKER_BEGIN]) {
                //printf(" found marker part ");
                ++objExpandSeq;
                if (objExpandSeq > OBJ_MARKER_END) {
                    addr = 0;
                    objExpandSeq = OBJ_GET_ADDR;
                }
                ++(inpos);
                continue;
            } else {  
                //                printf(" sequence unfinished ");
                for (int m=0;m<objExpandSeq-OBJ_MARKER_BEGIN;m++){
                    //printf("adding back %c", MARKER[m]);
                    current[i] = MARKER[m];
                    ++i;
                    if (i == blockSize) {
                        current = (char *) malloc(blockSize);
                        blocks.push_back(current);
                        i = 0;
                    }
                }
                objExpandSeq = OBJ_NONE;
                continue;
            } 
        } else if (objExpandSeq == OBJ_GET_ADDR) {
            if (c != '"') {
                addr *= 62;
                addr += getDecodeIdx(c);
                //printf(" getting addr... ");
                ++(inpos);
                continue;
            } else {
                ++(inpos);
                putStackPos.pop_back();
                putStackPos.push_back(inpos);
                //printf(" seeking addr %i ", (int) addr);
                //printf("at completeObjs of size %i \n", 
                       //       (int) completeObjects->size());
                putStack.push_back(completeObjects->at(addr).data());
                putStackLen.push_back(completeObjects->at(addr).length());
                putStackPos.push_back(0);
                inpos = 0;
                objExpandSeq = OBJ_NONE;
                continue;
            }
        }

        current[i] = c;
        ++i;    
        ++(inpos);
    }
#endif
}

void JsonDecompressor::decompress(char *&decoded, long &lenOut) {
    int lastBlock = blocks.size()-1;
    long lastBlockSize = i;
    //blocks[lastidx] = (char *) realloc(lastidx, i); 

    long ALLOC_CEILING = 20000; 
    /*
      THRESHOLD: maximum # of bytes to alloc or realloc at a time. 
      that is, the amount of memory that will at sometimes be 
      allocated in rough excess to the amount allocated by the 
      decompressed string. This const is a balancing act between 
      reducing the maximum possible heap peak for large inputs, 
      and the performance cost with large inputs of frequent 
      reallocations of  large (or very frequent reallocations 
      of small) amounts of memory. */

    const long HIGH_COMP_FACTOR = 8; 
    /* 
       if the string is much smaller than the allocation ceiling,
       then by guessing a factor which it will probably not expand
       past we can reduce the amount of memory allocated

       effectively this brings down the allocation ceiling 
       (though never raise it) so if HCF is 8, and the 
       compressed string passed in is 1000 chars, the new effective
       alloc ceiling for that input is 8000 chars. 

       This const is a balancing act between the cost of 
       unnecessary reallocation (if for many cases you 
       guess too low) and the performance cost of unnecessary
       allocation and free'ing (if you guess too high)  */
    if (HIGH_COMP_FACTOR * len < ALLOC_CEILING) {
        ALLOC_CEILING = HIGH_COMP_FACTOR * len;
    }

    long curSize = 0;
    long iOut =0;

    //idx here refers to the index on the product (reversed) array.
    //char *decoded;

    //should be at least equal to the longest char length a sequence
    //may be replaced by.
    long longestReplacement = 20;
    long iOutStopAt = -1; 

    int sequenceStep = 0;
    const int NONE = 0;
    const int FOUND_INIT = 1;
    const int FOUND_QUOTE = 2;
    const int FIRST_CHAR_ATE = 3;

    int firstDecodeIdx;
    int decodeIdx = -1;
    const char * decodedPtr;
    bool firstAdd = true;
    int ignoreNextStrval = 0;

    long curBlock = 0;
    long curBlockSize = (lastBlock ==0)? lastBlockSize : blockSize;
    char *block = blocks[0];
    char lastChar;
    long b =0;    
    //printf("\n\n%s", string(blocks[0], curBlockSize-1).data());
    while (curBlock <= lastBlock) {
        if (iOut >= iOutStopAt) {
            curSize += ALLOC_CEILING;
            iOutStopAt = curSize - longestReplacement;
            if (firstAdd) { 
                decoded = (char*) malloc(curSize); 
                firstAdd = false;
            } else {
                //we've used up the threshold, so we 
                //allocate more memory (while reducing the size of the
                // original string)
                decoded = (char*) realloc(decoded, curSize); 
            }            
        }
        // within the inner while(...) loop:
        // 1.
        // never depend on the block for access to a previous 
        // value of the block (instead store the value if it
        // may be relevant later) as any values may
        // be deallocated by the next char. in other words,
        // never decrement b.
        // 
        // 2.
        // only access the character at the current position
        // if you increment or change the position (b),
        // wait for the next loop (so that block[b] is available
        // even if it's in the next block) because we want
        // to read every character (and can't go back with b)
        // never increment b more than once per loop.
        while (iOut < iOutStopAt && b < curBlockSize) {
            //printf("\n %i %c ", (int) iOut, block[b]);
            if (sequenceStep == NONE) {
                if (block[b] != ':' 
                    && block[b] != ',' 
                    && block[b] != '{'
                    && block[b] != '[') {
                    //although redundant with the 'default case' 
                    // at the bottom which several
                    //consequents below fall to, because most 
                    // characters won't be part of a sequence
                    //we want to check if it's a character 
                    //like that first to reduce the number of
                    //tests/jumps.

                    decoded[iOut] = block[b];
                    ++iOut;
                    ++b;
                    continue;
                } else {
                    if (ignoreNextStrval == 0 ||
                        block[b] != ':') {
                        //printf("found init");
                        sequenceStep = FOUND_INIT;
                    }
                    if (ignoreNextStrval > 0) {
                        ignoreNextStrval--;
                    }
                }
            } else if (sequenceStep == FOUND_INIT) {
                if (block[b] == '"') {
                    sequenceStep = FOUND_QUOTE;
                } else {
                    sequenceStep = NONE;
                    continue; //check for a new init here.
                }
            } else if (sequenceStep == FOUND_QUOTE) {
                //"eating first char";               
                firstDecodeIdx = getDecodeIdx(block[b]);
                if (firstDecodeIdx >= 0) {
                    //it's a valid char.
                    lastChar = block[b];
                    sequenceStep = FIRST_CHAR_ATE;
                    ++b;
                    continue;
                } else {
                    //not part of a sequence or empty str. no big deal.
                    sequenceStep = NONE;
                    continue; //check for a new init here.
                }
            } else if (sequenceStep == FIRST_CHAR_ATE) {
                //looking at second char;
                sequenceStep = NONE;
                decodeIdx = getDecodeIdx(block[b]);
                //if returned decodeIdx is -1, gets the 
                //first ptr in the array 
                // (the one for a single character)
                decodedPtr = text::decoder
                    [firstDecodeIdx][decodeIdx +1];

                if (decodedPtr !=0x0) { 
                    //printf("expanding: %s ", decodedPtr);
                    if (decodedPtr == text::_raw_FULL
                        || decodedPtr == text::_name_FULL
                        || decodedPtr == text::_operator_FULL
                        || decodedPtr == text::_description_FULL
                        || decodedPtr == text::_message_FULL
                        || decodedPtr == text::_value_FULL
                        || decodedPtr == text::_source_FULL
                        || decodedPtr == text::_regexpBody_FULL
                        || decodedPtr == text::_regexpFlags_FULL) {
                        //printf(", ignNextStrval if this is a key");
                        // if it's a key.
                        // we should ignore the next value.
                        // string values to ignore (arbitrary or 
                        // rare compression gain) conveniently 
                        // correspond 1:1 with these keys that 
                        // directly precede any of them. 
                        // (e.g. there is never a list with some 
                        // or all strings to ignore)
                        ignoreNextStrval = (decodeIdx >=0)?2:1;
                    }

                    strcpy(decoded + iOut, decodedPtr);
                    while (decoded[iOut] != 0) {
                        ++iOut;
                    }
                    if (decodeIdx == -1) {
                        decoded[iOut] = block[b];
                        ++iOut;
                    }
                    ++b;
                    continue;
                } else {
                    // no decoded string here... 
                    //  false positive from the marker.
                    //printf("false positive from marker sequence in json decompress. null ptr at %i, %i \n", firstDecodeIdx, getDecodeIdx(block[b]));
                    decoded[iOut] = lastChar;
                    ++iOut;
                }   
            } else if (sequenceStep > NONE) {
                sequenceStep = NONE;
                continue; 
                //check for the init of a new sequence 
                // at this character.
            }

            decoded[iOut] = block[b];
            ++iOut;
            
            ++b;
        }
        if (b == curBlockSize) {
            free (blocks[curBlock]);
            curBlock++;

            if (curBlock == lastBlock){
                curBlockSize = lastBlockSize;
            }
            if (curBlock <= lastBlock) {
                block = blocks[curBlock];
                //printf("%s", string(block, curBlockSize-1).data());
            }
            b=0;
        }

    }
    //    decoded[iOut] = 0;
    decoded = (char *) realloc(decoded, iOut+1);
    lenOut = iOut;
    //printf(" final decoded: \n%s\n", decoded);
    //printf("eventual input length was %li \n", curBlockSize + (((long) lastBlock)*blockSize));
    //    return decoded;
}
#endif




#ifndef LIMITJSON

void tokenizeImpl(Document &outJson,
                  const u16string code, 
                  OptionsStruct options,
                  const bool retErrorsAsJson) { 
    options.tokens = true;
    options.tokenize = true;
    initglobals();
    Tokenizer tknr(code, options);
    tknr.tokenize(outJson, retErrorsAsJson);
    return;
}



    //? parenthesisCount for state not provided here normally as in parse. 
    //? That going to be a problem for us later?

    // Of course we collect tokens here.

    // The following two fields are necessary to compute the Regex tokens.


void tokenizeImpl(Document& d, 
                  const u16string code, const OptionsStruct options) { 
    tokenizeImpl(d, code, options, false);
}
void tokenizeImpl(Document &d,
                  const string code, const OptionsStruct options) { 
    tokenizeImpl(d, toU16string(code), options, false);
}
void tokenizeImpl(Document &d, const string code) { 
    OptionsStruct o;
    tokenizeImpl(d, toU16string(code), o, false);
}
void tokenizeImpl(Document &d, const u16string code) { 
    OptionsStruct o;
    tokenizeImpl(d, code, o, false);
}
#endif

string tokenizeRetString(const u16string code, OptionsStruct options){
    
    Document *out = new Document();

#ifdef LOWMEM
    if (asmRetVal != 0x0) {
        free (asmRetVal);
    }
    options.tokens = true;
    options.tokenize = true;
    initglobals();
    Tokenizer tknr(code, options);
#ifdef LIMITJSON
    vector<string> completeObjects;
    tknr.tokenize(*out, completeObjects, true);
    JsonDecompressor wrapper(&completeObjects, code.length());
#endif
#ifndef LIMITJSON
    tknr.tokenize(*out, true);
    JsonDecompressor wrapper(code.length());
#endif
    Writer<JsonDecompressor> writer(wrapper);
    out->Accept(writer); 
    delete out;
    long length;
    wrapper.decompress(asmRetVal, length);
    return string(asmRetVal, length);
#endif
#ifndef LOWMEM
    tokenizeImpl(*out, code, options, true);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    out->Accept(writer);    
    string result = buffer.GetString();
    delete out;
    return result;
#endif  
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

void ParseTools::parse(Document& outJson, 
#ifdef LIMITJSON
                       vector<string> &completeObjectsOut,
#endif
                       const bool retErrorsAsJson) {
#ifdef LIMITJSON
    task->completeObjects = &completeObjectsOut;
#endif
    outJson.SetObject();
    AllocatorType& alloclocal = outJson.GetAllocator();    
    alloc = &alloclocal;
    //relevant options are range, loc, attachComment
    //source, (hasSource also of course), and tracking and tolerance.
    //also attachComment implies range = true and commentTracking = true.
 
    Node *programNode;

#ifndef THROWABLE
    ErrWrapNodePtr tmp = parseProgram();
    if (tmp.err) {
        //pt.clearHeap();
        //json_object_put(programJson);
        if (task->errorType == 0) {
            task->retError.toJson(&extra, outJson, alloc);
            return;
        }
        task->retAssertError.toJson(&extra, outJson, alloc); 
        return;
    }
    programNode = tmp.val;
#endif
#ifdef THROWABLE
    try {
        programNode = parseProgram();
    } catch(ExError& e) {        
        //pt.clearHeap();
        if (retErrorsAsJson) {
            //json_object_put(programJson);
            e.toJson(&extra, outJson, alloc);
            return;
        }
        throw e;
    }
#endif
#ifdef LIMITJSON
    AddDocument(task.get(), text::_program, outJson, programNode->jv); 
#endif
#ifndef LIMITJSON
    outJson.AddMember(text::_program, programNode->jv.Move(), *alloc);
#endif

    Value regexList(kArrayType);
    programNode->regexPaths2json(regexList, alloc);

    outJson.AddMember(text::_regexp, 
        regexList, 
        *alloc);

   if (extra.commentTracking) {
       vec2jsonCallback<Comment>(outJson, alloc, &extra,
                                 text::_comments, extra.comments,
                                 &Comment::toJson); 
   }
   if (extra.tokenTracking) {
       scanner.filterTokenLocation();
       vec2jsonCallback<TokenRecord>(outJson, alloc, &extra,
                                     text::_tokens,
                                     extra.tokenRecords,
                                     &TokenRecord::toJson);
   }
   //   string s = "debug";
   //Value dbgval((int) extra.tokenRecords.size());
   //outJson.AddMember(Value(s.data(), s.length(), alloc).Move(),
   //                  dbgval, alloc);

   if (extra.errorTolerant) {
       vec2jsonCallback<ExError>(outJson, alloc, &extra,
                                 text::_errors,
                                 extra.errors,
                                 &ExError::toJsonTolerant);
   }

   extra.clear();
   programNode->delNode(programNode);
   return;
}

#ifndef LIMITJSON
void parseImpl(Document &outJson,
               const u16string code, 
               OptionsStruct options, //# nonconst 1:1
               const bool retErrorsAsJson) { 
    initglobals();
    ParseTools pt(code, options);
    pt.parse(outJson, retErrorsAsJson);
    return;
};


void parseImpl(Document& d, const u16string code,
                OptionsStruct options) {    
    parseImpl(d, code, options, false);
}
void parseImpl(Document& d, const string code, 
                OptionsStruct options) {    
    parseImpl(d, toU16string(code), options, false);
}

void parseImpl(Document& d, const string code) { 
    OptionsStruct o;
    parseImpl(d, toU16string(code), o, false);
}

void parseImpl(Document& d, const u16string code) { 
    OptionsStruct o;
    parseImpl(d, code, o, false);
}
#endif

//# return json as string.
string parseRetString(const u16string code, OptionsStruct options) {    
    Document *out = new Document();
    out->SetObject();

    //walkJson("root", out);
    //    StringBuffer buffer;
#ifdef LOWMEM
    if (asmRetVal != 0x0) {
        free (asmRetVal);
    }
    initglobals();
    ParseTools pt(code, options);
#ifdef LIMITJSON
    vector<string> completeObjects;
    pt.parse(*out, completeObjects,
                       true);
    JsonDecompressor wrapper(&completeObjects, code.length());
#endif
#ifndef LIMITJSON
    pt.parse(*out, true);
    JsonDecompressor wrapper(code.length());
#endif
    Writer<JsonDecompressor> writer(wrapper);
    out->Accept(writer); 
    delete out;
    long length;
    wrapper.decompress(asmRetVal, length);
    return string(asmRetVal, length);
#endif
#ifndef LOWMEM
    parseImpl(*out, code, options, true);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    out->Accept(writer);    
    string result = buffer.GetString();
    delete out;
    return result;
#endif  
}
string parseRetString(const string code,
                      OptionsStruct options) { 
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

    //#include "profiler.h"

//#include <chrono>
//using std::chrono::system_clock;

#ifdef HASMAIN
#include <fstream>

int main() {
    string result, allopt;
    unsigned int resultlength = 0;
    
    string finput;
    string finputOpt;
    //ifstream ifs("/home/n/coding/esp3/bench/cases/active/mootools.js");
    ifstream ifs("/home/n/coding/esp7/test/codetotest.js");
    ifstream optifs("/home/n/coding/esp7/test/opttotest");

    finput.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );    
    finputOpt.assign( (std::istreambuf_iterator<char>(optifs) ),
                    (std::istreambuf_iterator<char>()    ) );    

    vector<string> codeSamples = { finput };
    
    //vector<string> codeSamples = { 
    //   "var x = { null: 42 }" 
            //};
   
    //allopt = finputOpt;
    //allopt = "{ \"range\": true }";
    allopt = "{ \"loc\":true, \"range\":true, \"tokens\":true }";
    //    ProfilerStart("/tmp/profile2");

    //system_clock::time_point begin = system_clock::now();
    int reps = 1;
    for (int j = 0; j<reps; j++) {
        for (unsigned int i=0; i<codeSamples.size(); i++){ 
           //result = string(tokenizeRetString(
           result = string(parseRetString(
                                           toU16string(codeSamples[i]),
                                           OptionsStruct(allopt.data())));
            resultlength += result.length() % 6;
        }
    }
#ifdef LOWMEM
    free (asmRetVal);
#endif
    codeSamples.clear();
    finput = "";
    
    //system_clock::time_point end = system_clock::now();
    //auto timediff = end - begin;
    //int millis = std::chrono::duration_cast<std::chrono::milliseconds>(timediff).count();
    //printf("milliseconds: %i\n", (int) ((double) millis / (double) reps));
    //    ProfilerStop();
    printf("total length %u\n", resultlength);

    printf("text::_Identifier %s\n", text::_Identifier.s);
    
    ofstream lastResult;
    lastResult.open("/tmp/lastresult.txt");
    lastResult << result;
    lastResult.close();
    

    //printf("last result %s\n", result.data());
}
#endif
