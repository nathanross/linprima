#line 1 "Node.cpp"
#include "Node.hpp"
#include "NodesComments.hpp"
#include "stringutils.hpp"
#include "jsonutils.hpp"
#include "debug.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <string>
using namespace std;
using namespace wojson;
using namespace fixedstr;

#define reqinline inline



reqinline
fixedstr::FixedStr fstr(string in) {
    return fixedstr::getFixedStr(in);
}

//#this is the ONLY constructor in this code capable of 
//#modifying task state, it ALWAYS and ONLY changes state if lookaheadAvail
//#is true. Important to keep in mind when making 
//#1:1 updates.

Node::Node(bool lookaheadAvail, 
           bool exists, 
           vector<Node*>* heapNodes,
           WojsonDocument* docArg,
           LinprimaTask * taskArg):
    jv(nullptr),
    loc(-1,-1,-1),
    doc(docArg),
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


void Node::delNode(Node * toDel) {
    //DEBUGIN("delnode", true);
    if (toDel == nullptr) { return; }
    auto iter = find(heapNodes->begin(), heapNodes->end(), toDel);
    if (iter != heapNodes->end()) { heapNodes->erase(iter); }
    delete toDel->jv;
    delete (toDel);
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


//# different name to prevent easy bug of forgetting the string.
//# root path, should be first in vector, then path within it, etc.
void Node::regNoadd(Node * child) { 
    //DEBUGIN("regNoAdd", false);
    if (child == nullptr) { return; }

    if (child->hasRange) {
        WojsonArr rangearr(doc);
        //bool m = find(heapNodes.begin(), haystack.end(), needle) != haystack.end();
        //int n = (m)? -1:0;
        //        rangearr.PushBack(n, *alloc);
        rangearr.push(child->range[0]);
        rangearr.push(child->range[1]);
        //rangearr.movePushRaw(fixedstr::getFixedStr(std::to_string(child->range[0])));
        //rangearr.movePushRaw(fixedstr::getFixedStr(std::to_string(child->range[1])));
        child->jv->moveAssign(text::_range, 
                              doc->regColl(nullptr, rangearr.toCompressedString())); 
        //child->jv->assignColl(text::_range, &rangearr);
    } 
    if (child->hasLoc) {        
        WojsonMap locjson(doc);
        child->loc.toJson(&locjson, doc);
        child->jv->moveAssign(text::_loc, 
                              doc->regColl(nullptr, locjson.toCompressedString())); 
        //child->jv->assignColl(text::_loc, &locjson);
    }
    //DEBUGOUT("Node::regNoAdd", false);
}

void Node::moveRegexes(const vector<RegexLeg> &paths, Node *child) {
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

void Node::lateResolve() {
    doc->replaceCollContents(completedPos,
                            jv->toCompressedString());
    delNode(this);       
}

void Node::reg(const SFixedStr &path, Node * child) { 
    //DEBUGIN("reg(string path, Node &child)", false);
    
    //addition of sequence expression's children is done lazily
    //so that they are available for reinterpretAsCoverList
    //to reinterpret them as children of a different node
    //and then cleanly delete sequenceexpression node
    //without having to extricate children objects from json, etc.
    if (child != nullptr) {
        if (child->type == Syntax[Synt::SequenceExpression]) {
            child->nodeVec(text::_expressions, child->expressions);
        } else if (child->type 
                   == Syntax[Synt::AssignmentExpression]) {
            child->reg(text::_left, child->leftAssign);
            child->reg(text::_right, child->rightAssign);
        }
        regNoadd(child);
        if (child->regexPaths.size() > 0) {
            vector<RegexLeg> pathlist;
            pathlist.emplace_back((&(path)));
            moveRegexes(pathlist, child);
        }
        
        if (( ! task->extra.attachComment) 
            || ( ! child->thisnc) 
            || child->thisnc->resolved) {

            jv->assignColl(path, child->jv);
            delNode (child); 
        } else {
            child->completedPos = jv->assignReserve(path);          
            child->thisnc->setNodeDetached(child);
        }

    } else {
        jv->assignNull(path);
    }

    //DEBUGOUT("node::reg", false);
}
void Node::regPush(WojsonArr* arr, const SFixedStr &path, int &ctr,
                   Node* child) {
    if (child != nullptr) {
        if (child->type == Syntax[Synt::SequenceExpression]) {
            child->nodeVec(text::_expressions, child->expressions);
        } else if (child->type == Syntax[Synt::AssignmentExpression]) {
            child->reg(text::_left, child->leftAssign);
            child->reg(text::_right, child->rightAssign);
        } else if (child->type == Syntax[Synt::Property]) {
            child->reg(text::_key, child->leftAssign);
            child->reg(text::_value, child->rightAssign);
            child->jv->scopedAssign(text::_kind, *(child->spareStrref));
        }

        regNoadd(child);
        if (child->regexPaths.size() > 0) {
            vector<RegexLeg> pathlist;
            pathlist.emplace_back((&(path)));
            //child not added yet, so no need for ->Size()-1
            pathlist.emplace_back((ctr));
            moveRegexes(pathlist, child);
        }
        ++ctr;

        if (task->extra.attachComment 
            && child->thisnc 
            && !(child->thisnc->resolved)) {

            child->completedPos = arr->pushReserve();
            child->thisnc->setNodeDetached(child);
        } else {
            arr->pushColl(child->jv);
            delNode (child);
        }
    } else {
        arr->pushNull();
    }
}
void Node::nodeVec(const SFixedStr &path, const vector< Node* > & nodes) { 
    //DEBUGIN("nodeVec(string path, vector< Node > & nodes)", false);
    WojsonArr arr(doc);
    for (unsigned int i=0; i<nodes.size(); i++) {
        if (nodes[i] != nullptr) {
            if (nodes[i]->type == Syntax[Synt::SequenceExpression]) {
                nodes[i]->nodeVec(text::_expressions, nodes[i]->expressions);
            } else if (nodes[i]->type == Syntax[Synt::AssignmentExpression]) {
                nodes[i]->reg(text::_left, nodes[i]->leftAssign);
                nodes[i]->reg(text::_right, nodes[i]->rightAssign);
            } else if (nodes[i]->type == Syntax[Synt::Property]) {
                nodes[i]->reg(text::_key, nodes[i]->leftAssign);
                nodes[i]->reg(text::_value, nodes[i]->rightAssign);
                nodes[i]->jv->scopedAssign(text::_kind, *(nodes[i]->spareStrref));
            }

            regNoadd(nodes[i]);

            if (nodes[i]->regexPaths.size() > 0) {
                vector<RegexLeg> pathlist;
                pathlist.emplace_back((&(path)));
                //child not added yet, so no need for ->Size()-1
                pathlist.emplace_back((i));
                moveRegexes(pathlist, nodes[i]);
            }
            if (task->extra.attachComment 
                && nodes[i]->thisnc 
                && !(nodes[i]->thisnc->resolved)) {

                nodes[i]->completedPos = arr.pushReserve();                
                nodes[i]->thisnc->setNodeDetached(nodes[i]);
            } else {
                arr.pushColl(nodes[i]->jv);
                delNode (nodes[i]);
            }

        } else {
            arr.pushNull();
        }
    } 
    jv->assignColl(path, &arr);
    //DEBUGOUT("node::nodeVec", false);
}

void Node::initJV(const Synt in) { 
    //DEBUGIN("initJV", false);
    type = Syntax[in];
    jv = new WojsonMap(doc);
    jv->scopedAssign(text::_type, *type);
}
void Node::regexPaths2json(WojsonArr& out, WojsonDocument *doc) { 
    //DEBUGIN("Node::regexPaths2json()", false);
    
    for (unsigned int i=0; i<regexPaths.size(); i++) {
        WojsonArr path(doc);
        
        for (int j=regexPaths[i].size()-1; j>=0; j--) {
            if (regexPaths[i][j].isNum) {
                path.push(regexPaths[i][j].num);
            } else {
                path.scopedPush(*(regexPaths[i][j].path));
            } 
        } 
        out.pushColl(&path);
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
    thisnc.reset(new NodesComments(*jv, doc));
    //bool LEADING = true, TRAILING= false;
    if (bottomRight->size() > 0) {
        last = bottomRight->back();
    }

    if (type == Syntax[Synt::Program]) {  
        if (completedPos > 0) { //one or more body element.
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
            //last->commentsIntoJson(&(task->extra), TRAILING);
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
                lastChild->resolve(&(task->extra));
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
        last->resolve(&(task->extra));
    }

    if (lastChild) {
        if (lastChild->leadingComments.size() > 0 &&
            lastChild->leadingComments.back()
            .range[1] <= thisnc->range[0]) {
            thisnc->leadingComments = lastChild->leadingComments;
            lastChild->leadingComments.clear();
            //lastChild->commentsIntoJson(&(task->extra), LEADING);
            //thisnc->commentsIntoJson(&(task->extra), LEADING);
        }
        lastChild->resolve(&(task->extra));
    } else if (extra.leadingComments.size() > 0 && 
               extra.leadingComments[extra.leadingComments.size() - 1]
               .range[1] <= thisnc->range[0]) {
        thisnc->leadingComments = extra.leadingComments;
        extra.leadingComments.clear();
        //thisnc->commentsIntoJson(&(task->extra), LEADING);
    }

    if (trailingComments.size() > 0) {
        thisnc->trailingComments = trailingComments;
        //thisnc->commentsIntoJson(&(task->extra), TRAILING);
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
    initJV(Synt::ArrayExpression);
    nodeVec(text::_elements, elements);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishArrowFunctionExpression(const vector< Node* >& params,
                                         const vector< Node* >& defaults, 
                                         Node * body, 
                                         const bool expression) {
    DEBUGIN("finishArrowFunctionExpression(vector< Node >& params, vector< Node >& defaults, Node& body, bool expression)", false);
    initJV(Synt::ArrowFunctionExpression);

    jv->assignNull(text::_id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jv->assignNull(text::_rest);
    jv->assign(text::_generator, false);
    jv->assign(text::_expression, expression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishAssignmentExpression(const string oper, 
                                      Node * left, 
                                      Node * right) {
    DEBUGIN("finishAssignmentExpression(u16string oper, Node& left, Node& right)", false);

    initJV(Synt::AssignmentExpression);
    jv->moveAssign(text::_operator, lstr(oper));

    leftAssign = left;
    rightAssign = right;

    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBinaryExpression(const string oper, 
                                  Node * left, 
                                  Node * right) {
    DEBUGIN("finishBinaryExpression(u16string oper, Node& left, Node& right)", false);
    initJV((oper == "||" || oper == "&&") ? 
            Synt::LogicalExpression : Synt::BinaryExpression);

    jv->moveAssign(text::_operator, lstr(oper));

    reg(text::_left, left); 
    reg(text::_right, right);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBlockStatement(const vector< Node* >& body) {
    DEBUGIN("finishBlockStatement(vector< Node >& body)", false);
    initJV(Synt::BlockStatement);
    nodeVec(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishBreakStatement(Node * label) {
    DEBUGIN("finishBreakStatement(Node& label)", false);
    initJV(Synt::BreakStatement);
    reg(text::_label, label);
    this->finish();  
    DEBUGOUT("", false);
}


void Node::finishCallExpression(Node * callee, 
                                const vector< Node* >& args) {
    DEBUGIN("finishCallExpression", false);
    initJV(Synt::CallExpression);
    reg(text::_callee, callee);
    nodeVec(text::_arguments, args);
    this->finish(); 
    DEBUGOUT("", false);
}


void Node::finishCatchClause(Node * param, 
                             Node * body) {
    DEBUGIN("finishCatchClause", false);
    initJV(Synt::CatchClause);
    reg(text::_param, param);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishConditionalExpression(Node * test, 
                                       Node * consequent,
                                       Node * alternate) {
    DEBUGIN("finishConditionalExpression", false);
    initJV(Synt::ConditionalExpression);
    reg(text::_test, test);
    reg(text::_consequent, consequent);
    reg(text::_alternate, alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishContinueStatement(Node * label) {
    DEBUGIN("finishContinueStatement", false);
    initJV(Synt::ContinueStatement);
    reg(text::_label, label);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishDebuggerStatement() {
    DEBUGIN("finishDebuggerStatement", false);
    initJV(Synt::DebuggerStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishDoWhileStatement(Node * body, Node * test) {
    DEBUGIN("finishDoWhileStatement", false);
    initJV(Synt::DoWhileStatement);
    reg(text::_body, body);
    reg(text::_test, test);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishEmptyStatement() {
    DEBUGIN("finishEmptyStatement()", false);
    initJV(Synt::EmptyStatement);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishExpressionStatement(Node * expression) {
    DEBUGIN("finishExpressionStatement", false);
    initJV(Synt::ExpressionStatement);
    spareStrref = expression->type;
    reg(text::_expression, expression);
    this->finish();  DEBUGOUT("", false);
}


void Node::finishForStatement(Node * init, 
                              Node * test, 
                              Node * update, 
                              Node * body) {
    DEBUGIN("finishForStatement", false);
    initJV(Synt::ForStatement);
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
    initJV(Synt::ForInStatement);
    reg(text::_left, left);
    reg(text::_right, right);
    reg(text::_body, body);
    jv->assign(text::_each, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionDeclaration(Node * id, 
                                     const vector< Node* >& params, 
                                     const vector< Node* >& defaults, 
                                     Node * body) {
    DEBUGIN("Node::finishFunctionDeclaration", false);
    initJV(Synt::FunctionDeclaration);
    reg(text::_id, id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jv->assignNull(text::_rest);
    jv->assign(text::_generator, false);
    jv->assign(text::_expression, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishFunctionExpression(Node * id, 
                                    const vector< Node* >& params, 
                                    const vector< Node* >& defaults, 
                                    Node * body) {  
    DEBUGIN("finishFunctionExpression", false);
    initJV(Synt::FunctionExpression);
    reg(text::_id, id);
    nodeVec(text::_params, params);
    nodeVec(text::_defaults, defaults);
    reg(text::_body, body);
    jv->assignNull(text::_rest);
    jv->assign(text::_generator, false);
    jv->assign(text::_expression, false);
    this->finish();
    DEBUGOUT("", false);
}
void Node::finishIdentifier(const string name) {
    DEBUGIN("finishIdentifier", false);
    initJV(Synt::Identifier);
    this->name = name;
    jv->moveAssign(text::_name, lstr(name));
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishIfStatement(Node * test, 
                             Node * consequent, 
                             Node * alternate) {
    DEBUGIN("finishIfStatement", false);
    initJV(Synt::IfStatement);
    reg(text::_test, test);
    reg(text::_consequent, consequent);
    reg(text::_alternate, alternate);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishLabeledStatement(Node * label, 
                                  Node * body) {
    DEBUGIN("finishLabeledStatement", false);
    initJV(Synt::LabeledStatement);
    reg(text::_label, label);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishLiteral(ptrTkn token) {
    DEBUGIN("finishLiteral(ptrTkn token)", false);
    initJV(Synt::Literal);
    if (token->literaltype == LiteralType["String"]) {
        name = token->strvalue;
        jv->moveAssign(text::_value, lstr(token->strvalue));
    } else if (token->literaltype == LiteralType["Int"]) {
        name = to_string(token->intvalue);
        jv->assign(text::_value, token->intvalue);
    } else if (token->literaltype == LiteralType["Double"]) {
        name = token->strvalue;
        jv->moveAssignRaw(text::_value, fstr(token->strvalue));
    } else if (token->literaltype == LiteralType["Bool"]) {
        name = (token->bvalue)? "true": "false";
        jv->assign(text::_value, token->bvalue);
    } else if (token->literaltype == LiteralType["Null"]) {
        name = "null";
        jv->assignNull(text::_value);
    } else if (token->literaltype == LiteralType["Regexp"]) {
        WojsonMap reg(doc);
        reg.moveAssign(text::_regexpBody, 
                      lstr(token->strvalue));
        reg.moveAssign(text::_regexpFlags,
                       lstr(token->flags));
        reg.assign(text::_lineNumber,
                      task->lineNumber); 
        reg.assign(text::_index,
                   token->end); 
        reg.assign(text::_column,
                   token->end+1); 
        jv->assignColl(text::_value, &reg);
        RegexLeg starter(-1);
        starter.isStart = true;
        regexPaths.push_back({starter});
    }
            jv->moveAssign(text::_raw, lstr(s(
                                              slice(task->sourceRaw, 
                                                    token->start, 
                                                    token->end)
                                              )));
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishMemberExpression(const char16_t accessor, 
                                  Node * object, 
                                  Node * property) {
    DEBUGIN("finishMemberExpression", false);
    initJV(Synt::MemberExpression);
    jv->assign(text::_computed, (accessor == u'['));
    reg(text::_object, object);
    reg(text::_property, property);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishNewExpression(Node * callee, 
                               const vector<Node*>& args) {
    DEBUGIN("finishNewExpression", false);
    initJV(Synt::NewExpression);
    reg(text::_callee, callee);
    nodeVec(text::_arguments, args);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishObjectExpression(const vector<Node*>& properties) {
    DEBUGIN("finishObjectExpression", false);
    initJV(Synt::ObjectExpression);
    nodeVec(text::_properties, properties);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishPostfixExpression(const string oper, 
                                   Node * argument) {
    DEBUGIN("finishPostfixExpression", false);
    initJV(Synt::UpdateExpression);
    jv->moveAssign(text::_operator, lstr(oper));
    reg(text::_argument, argument);
    jv->assign(text::_prefix, false);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishProgram(const vector< Node* >& body) {
    DEBUGIN("finishProgram", false);
    initJV(Synt::Program);
    nodeVec(text::_body, body);
    for (int i=0; i<task->extra.bottomRightStack.size(); i++) {
        task->extra.bottomRightStack[i]->resolve(&(task->extra));
    }
    this->finish();
    //no parent node to call reg so add these atts. here.
    if (task->extra.range) {
        WojsonArr rangearr(doc);
        rangearr.push(this->range[0]);
        rangearr.push(this->range[1]);
        jv->assignColl(text::_range, &rangearr);
    }
    if (task->extra.loc) {
        WojsonMap locjson(doc);
        this->loc.toJson(&locjson, doc);
        jv->assignColl(text::_loc, &locjson);
    }
    DEBUGOUT("", false);    
}


void Node::finishProperty(const SFixedStr &kind,
                          Node * key, 
                          Node * value) {
    DEBUGIN("finishProperty", false);
    initJV(Synt::Property);
    leftAssign = key;
    rightAssign = value;
    spareStrref = &kind;
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishReturnStatement(Node * argument) {
    DEBUGIN("finishReturnStatement", false);
    initJV(Synt::ReturnStatement);
    reg(text::_argument, argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSequenceExpression(const vector< Node* >& expressions) {
    DEBUGIN("finishSequenceExpression", false);
    initJV(Synt::SequenceExpression);
    this->expressions = expressions;    
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchCase(Node * test, 
                            const vector< Node* >& consequent) {
    DEBUGIN("finishSwitchCase", false);
    initJV(Synt::SwitchCase);
    reg(text::_test, test);
    nodeVec(text::_consequent, consequent);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishSwitchStatement(Node * discriminant, 
                                 const vector < Node* >& cases) {
    DEBUGIN("finishSwitchStatement", false);
    initJV(Synt::SwitchStatement);
    reg(text::_discriminant, discriminant);
    nodeVec(text::_cases, cases);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThisExpression() {
    DEBUGIN("finishThisExpression()", false);
    initJV(Synt::ThisExpression);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishThrowStatement(Node * argument) {
    DEBUGIN("finishThrowStatement", false);
    initJV(Synt::ThrowStatement);
    reg(text::_argument, argument);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishTryStatement(Node * block, 
                              const vector<Node*>& guardedHandlers, 
                              const vector<Node*>& handlers, 
                              Node * finalizer) {
    DEBUGIN("finishTryStatement", false);
    initJV(Synt::TryStatement);
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
    initJV((oper == "++" || oper == "--") ? 
            Synt::UpdateExpression : Synt::UnaryExpression);
    jv->moveAssign(text::_operator, lstr(oper));
    reg(text::_argument, argument);
    jv->assign(text::_prefix, true);
    this->finish();
    DEBUGOUT("", false);
}

void Node::finishVariableDeclaration(const vector< Node* >& declarations, 
                                     const SFixedStr& kind) {
    DEBUGIN("finishVariableDeclaration",false);
    initJV(Synt::VariableDeclaration);
    nodeVec(text::_declarations, declarations);
    jv->scopedAssign(text::_kind, kind);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishVariableDeclarator(Node * id, 
                                    Node * init) {
    DEBUGIN("finishVariableDeclarator", false);
    initJV(Synt::VariableDeclarator);
    reg(text::_id, id);
    reg(text::_init, init);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWhileStatement(Node * test, 
                                Node * body) {
    DEBUGIN("finishWhileStatement", false);
    initJV(Synt::WhileStatement);
    reg(text::_test, test);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}


void Node::finishWithStatement(Node * object, 
                               Node * body) {
    DEBUGIN("finishWithStatement", false);
    initJV(Synt::WithStatement);
    reg(text::_object, object);
    reg(text::_body, body);
    this->finish();
    DEBUGOUT("", false);
}
