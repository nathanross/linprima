
//#this is the ONLY constructor in this code capable of 
//#modifying state, it ALWAYS and ONLY changes state if lookaheadAvail
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
