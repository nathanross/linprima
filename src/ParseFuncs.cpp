//#line 1 "ParseFuncs.cpp"
#include "ParseFuncs.hpp"
#include "stringutils.hpp"
#include "texpconsts.hpp"
#include "jsonutils.hpp"
#include "debug.hpp"
using namespace std;
using namespace rapidjson;
using namespace wojson;
using namespace fixedstr;
#define reqinline inline



const SFixedStr EMPTY_STRREF = SFixedStr("");

unique_ptr<Node> make_unique_node() {
    unique_ptr<Node> a(new Node(false, false, 0x0, 0x0, 0x0));
    return move(a);
}


map<string, unique_ptr<Node>> PlaceHolders { 
};

bool globalsAssigned = false;

void initGlobals() {
    PlaceHolders["ArrowParameterPlaceHolder"] = make_unique_node(); 
    PlaceHolders["ArrowParameterPlaceHolder"]->type = (&(text::_ArrowParameterPlaceholder));
    globalsAssigned = true;
}

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
    if (!globalsAssigned) { initGlobals(); }
}

ParseTools::~ParseTools() {
    clearNodeHeap();
}

void ParseTools::clearNodeHeap() {
    Node *tmp;
    while (heapNodes.size() > 0) {
        auto it = heapNodes.begin();
        tmp = *it;
        heapNodes.erase(it);
        //if (tmp->jv != nullptr)
        //    { json_object_put(tmp->jv); tmp->jv = nullptr; }
        delete (tmp->jv);
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
    return new Node(lookavailInit, exists, &heapNodes, doc, task.get());
}
WrappingNode* ParseTools::makeWrappingNode(ptrTkn token) {
    return new WrappingNode(token, &heapNodes, doc, task.get());
}
WrappingNode* ParseTools::makeWrappingNode() {
    return new WrappingNode(&heapNodes, doc, task.get());
}

// 11.1.4 Array Initialiser

//throw_
Node* ParseTools::parseArrayInitialiser() {
    DEBUGIN(" parseArrayInitialiser()", false);
    vector< Node* > elements;
    Node *node = makeNode(true, true);    

    expect("[");
    int ctr=0;
    const SFixedStr& path = text::_elements;

    node->initJV(Synt::ArrayExpression);
    auto vec = doc->getReservedArr(node->jv, path);
    while (!match("]")) {

        if (match(",")) {
            scanner.lex();
            node->regPush(&vec, path, ctr, nullptr);
        } else {
#ifndef THROWABLE
            Node *tmp = parseAssignmentExpression();
            node->regPush(&vec, path, ctr, tmp);
#endif
#ifdef THROWABLE
            node->regPush(&vec, path, ctr,
                          parseAssignmentExpression());
#endif
            if (!match("]")) {
                expect(",");
            }
        }
    }
    vec.complete();

    scanner.lex();

    node->finish();
    DEBUGOUT("parseArrInit", false);
    return node;
}

/*

//throw_
Node* ParseTools::parseArrayInitialiser() {
    DEBUGIN(" parseArrayInitialiser()", false);
    vector< Node* > elements;
    Node *node = makeNode(true, true);    

    expect("[");

    while (!match("]")) {

        if (match(",")) {
            scanner.lex();
            elements.push_back(nullptr);
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
*/

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
    if (!(first->isNull) && task->strict && Tokenizer::isRestrictedWord(param[0]->name)) {
        task->throwErrorTolerant(first, Messages[Mssg::StrictParamName],{});
    }
    task->strict = previousStrict;
    node->finishFunctionExpression(nullptr, param, 
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
        return nullptr; //#just to satisfy warnings.
    } else {
        key = parseObjectPropertyKey();
        expect(":");
        value = parseAssignmentExpression();
        node->finishProperty(text::_init, key, value);
        DEBUGOUT("parseObjProp", false); 
        return node;
    }
}

//throw_
Node* ParseTools::parseObjectInitialiser() {
    DEBUGIN(" parseObjectInitialiser()", false);
    vector<Node*> properties;
    ptrTkn token;
    Node *property, 
        *node = makeNode(true, true);
    
    string key, name;
    const SFixedStr *kindname;
    int kind;
    map<string, int> kmap;

    node->initJV(Synt::ObjectExpression);
    const SFixedStr &path = text::_properties;
    auto vec = doc->getReservedArr(node->jv, path);
    int ctr=0;
    expect("{");

    while (!match("}")) {
        property = parseObjectProperty();
        
        Node * keynode = property->leftAssign;

        if (keynode->type == Syntax[Synt::Identifier]) {
            name = keynode->name;            
        } else { //literal
            name = keynode->name;
        }
        kindname = property->spareStrref; //property->jv['kind']
        kind = (kindname == &(text::_init)) ? PropertyKind["Data"] : 
            (kindname == &(text::_get)) ? PropertyKind["Get"] : PropertyKind["Set"];

        key = "$";
        key.append(name);
        if (hasStringKey<int>(key,kmap)) {
            if (kmap[key] == PropertyKind["Data"]) {
                if (task->strict && kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                            Messages[Mssg::StrictDuplicateProperty],{});
                } else if (kind != PropertyKind["Data"]) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                }
            } else {
                if (kind == PropertyKind["Data"]) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                            Messages[Mssg::AccessorDataProperty],{});
                } else if (kmap[key] & kind) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                            Messages[Mssg::AccessorGetSet],{});
                }
            }
            kmap[key] |= kind;
        } else {
            kmap[key] = kind;
        }

        //problem with test case: (function () { 'use strict'; delete i; }())
        node->regPush(&vec, path, ctr, property);
        //properties.push_back(property);

        if (!match("}")) {
            expectTolerant(",");
        }
    }
    vec.complete();

    expect("}");

    node->finish();
    //node->finishObjectExpression(properties);
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
void ParseTools::parseArguments(Node *parent) {
    DEBUGIN(" parseArguments()", false);
    const SFixedStr& path = text::_arguments;
    auto vec = doc->getReservedArr(parent->jv, path);
    int ctr=0;    
    expect("(");
    if (!match(")")) {
        while (idx < length) {
            Node *tmp = parseAssignmentExpression();
            parent->regPush(&vec, path, ctr, tmp);
            if (match(")")) {
                break;
            }
            expectTolerant(",");
        }
    }
    vec.complete();
    expect(")");
    DEBUGOUT("parseArguments", false);
    return;
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
    node->initJV(Synt::NewExpression);
    node->reg(text::_callee, callee);


    if (match("(")) { 
        parseArguments(node);
    } else {
        WojsonArr emptyarr = doc->getArr(); 
        //! supposed to be empty map or empty arr?
        node->jv->assignColl(text::_arguments,
                          &(emptyarr));
    }
    
    node->finish();
    DEBUGOUT("parseNewExpr", false);
    return node;
}

//throw_
Node* ParseTools::parseLeftHandSideExpressionAllowCall() {
    DEBUGIN(" parseLeftHandSideExpressionAllowCall()", false);
    vector< Node * > args;
    Node *expr, *property;
    WrappingNode *tmpnode;
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
            tmpnode = makeWrappingNode();
            tmpnode->initJV(Synt::CallExpression);
            tmpnode->reg(text::_callee, expr);
            parseArguments(tmpnode);
            tmpnode->usualInit(startToken);
            tmpnode->finish();
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
                if (task->strict && expr->type == Syntax[Synt::Identifier] && 
                    Tokenizer::isRestrictedWord(expr->name)) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN,
                                       Messages[Mssg::StrictLHSPostfix],{});
                }

                if (!isLeftHandSide(expr)) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN,
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
        if (task->strict && expr->type == Syntax[Synt::Identifier] 
            && Tokenizer::isRestrictedWord(expr->name)) {
            task->throwErrorTolerant(Tokenizer::NULLPTRTKN, Messages[Mssg::StrictLHSPrefix],{});
        }

        if (!isLeftHandSide(expr)) {
            task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
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

        if (task->strict && token->strvalue == "delete" 
            && expr->type == Syntax[Synt::Identifier]) {
            task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                                     Messages[Mssg::StrictDelete], {});
        }
        tmpnode->finishUnaryExpression(token->strvalue, expr);
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
    nodestack.push_back(nullptr);
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
        nodestack.push_back(nullptr);
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

//works fine, just haven't seen any maxheap savings
/*//throw_
Node* ParseTools::parseConditionalExpression() {
    DEBUGIN(" parseConditionalExpression()", false);
    Node *expr,
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
        WrappingNode *tmpnode = makeWrappingNode();
        tmpnode->initJV(Synt::ConditionalExpression);
        tmpnode->reg(text::_test, expr);

        scanner.lex();
        previousAllowIn = state.allowIn;
        state.allowIn = true;
        consequent = parseAssignmentExpression();
        tmpnode->reg(text::_consequent, consequent);

        state.allowIn = previousAllowIn;
        expect(":");
        alternate = parseAssignmentExpression();
        tmpnode->reg(text::_alternate, alternate);

        tmpnode->usualInit(startToken);
        tmpnode->finish();
        DEBUGOUT("parseCondExpr2", false); 
        return tmpnode;
    }

    DEBUGOUT("parseCondExpr3", false);
    return expr;
}*/

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
    } else if (options.firstRestricted == nullptr) {
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
            defaults.push_back(nullptr);
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
                   Tokenizer::NULLPTRTKN,
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

    if (task->strict && options.firstRestricted != nullptr) { 
        task->throwError(Tokenizer::NULLPTRTKN, options.message, {});
        //        task->throwError(options.firstRestricted, options.message, {});
    }
    if (task->strict && options.stricted != nullptr) {
        task->throwError(Tokenizer::NULLPTRTKN, options.message, {});
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
            task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
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

/*
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
        if (statement == nullptr) { 
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
*/

//throw_
Node* ParseTools::parseBlock() { 
    DEBUGIN(" parseBlock()", false);
    Node *node = makeNode(true, true);

    node->initJV(Synt::BlockStatement);
    const SFixedStr& path = text::_body;
    auto vec = doc->getReservedArr(node->jv, path);
    int ctr=0;

    expect("{");
    //inline of parseStatementList
    Node *statement;

    while (idx < length) {
        if (match("}")) {
            break;
        }
        statement = parseSourceElement();
        if (statement == nullptr) { 
            break;
        }
        node->regPush(&vec, path, ctr, statement);
    }
    vec.complete();

    expect("}");
    node->finish();
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
Node* ParseTools::parseVariableDeclaration(const SFixedStr *kind) { 
    DEBUGIN(" parseVariableDeclaration(u16string kind)", false);
    Node *id, *init,
        *node = makeNode(true, true);
    init = nullptr;

    id = parseVariableIdentifier();

    // 12.2.1
    if (task->strict && Tokenizer::isRestrictedWord(id->name)) {
        task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                                 Messages[Mssg::StrictVarName], {});
    }
    if (kind == &(text::_const)) {
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
void ParseTools::parseVariableDeclarationList(const SFixedStr *kind,
                                                         Node *parent) {
    DEBUGIN("parseVariableDeclarationList", false);
    const SFixedStr &path = text::_declarations;
    auto vec = doc->getReservedArr(parent->jv, path);
    int ctr=0;

    parent->numChildren = 0;
    do {
        ++(parent->numChildren);
        Node *tmp = parseVariableDeclaration(kind);
        parent->regPush(&vec, path, ctr, tmp);
        if (!match(",")) {
            break;
        }
        scanner.lex();
    } while (idx < length);
    vec.complete();

    DEBUGOUT("parseVariableDeclarationList", false); 
    return;
}

//throw_
Node* ParseTools::parseVariableStatement(Node* node) { 
    DEBUGIN(" parseVariableStatement(Node node)", false);
    node->initJV(Synt::VariableDeclaration);

    expectKeyword("var");
    parseVariableDeclarationList(&EMPTY_STRREF, 
                                 node); 
    consumeSemicolon();

    node->jv->scopedAssign(text::_kind, text::_var);
    node->finish();
    DEBUGOUT("parseVariableStatement", false); 
    return node;
}

// kind may be `const` or `let`
// Both are experimental and not in the specification yet.
// see http://wiki.ecmascript.org/doku.php?id=harmony:const
// and http://wiki.ecmascript.org/doku.php?id=harmony:let
//throw_
Node* ParseTools::parseConstLetDeclaration(const string kind, 
                                           const SFixedStr *kindref) { 
    DEBUGIN(" parseConstLetDeclaration(u16string kind)", false);    
    Node *node = makeNode(true, true);
    node->initJV(Synt::VariableDeclaration);

    expectKeyword(kind);
    parseVariableDeclarationList(kindref, node);
    consumeSemicolon();
    
    node->jv->scopedAssign(text::_kind, *kindref);
    node->finish();
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
    node->initJV(Synt::IfStatement);
    node->reg(text::_test, test);

    expect(")");
    consequent = parseStatement();
    node->reg(text::_consequent, consequent);
    if (matchKeyword("else")) {
        scanner.lex();
        alternate = parseStatement();
    } else {
        alternate = nullptr;
    }
    node->reg(text::_alternate, alternate);
    node->finish();
    //node->finishIfStatement(test, consequent, alternate);
    DEBUGOUT("parseIfStatement", false); 
    return node;
}

/*
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
        alternate = nullptr;
    }
    node->finishIfStatement(test, consequent, alternate);
    DEBUGOUT("parseIfStatement", false); 
    return node;
}
*/
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
/*
//throw_
Node* ParseTools::parseWhileStatement(Node* node) { 
    DEBUGIN(" parseWhileStatement(Node node)", false);
    Node *test, *body;
    bool oldInIteration;
    node->initJV(Synt::WhileStatement);

    expectKeyword("while");
    expect("(");
    test = parseExpression();
    node->reg(text::_test, test);

    expect(")");
    oldInIteration = state.inIteration;
    state.inIteration = true;
    body = parseStatement();
    node->reg(text::_body, body);

    state.inIteration = oldInIteration;
    node->finish();
    DEBUGOUT("parseWhileStatement", false);
    return node;
}
*/

//throw_
Node* ParseTools::parseForVariableDeclaration() { 
    DEBUGIN(" parseForVariableDeclaration()", false);
    ptrTkn token;
    vector< Node* > declarations;
    Node *node = makeNode(true, true);
    node->initJV(Synt::VariableDeclaration);

    token = scanner.lex();
    parseVariableDeclarationList(&EMPTY_STRREF, node);

    node->jv->scopedAssign(text::_kind, (token->strvalue == "var")? text::_var: ((token->strvalue == "const")? text::_const : text::_let));
    node->finish();
    DEBUGOUT("parseForVariableDeclaration", false);     
    return node;
}

//throw_
Node* ParseTools::parseForStatement(Node* node) { 
    DEBUGIN(" parseForStatement(Node node)", false);

    bool oldInIteration, previousAllowIn = state.allowIn;

    Node *body, *left, *right =0x0, 
        *update, *test, *init;
    bool forIn = false;
    left=nullptr;
    update=nullptr; test=nullptr; init=nullptr;

    expectKeyword("for");
    expect("(");
    if (match(";")) {
        scanner.lex();
    } else {
        if (matchKeyword("var") || matchKeyword("let")) {
            state.allowIn = false;
            init = parseForVariableDeclaration();
            state.allowIn = previousAllowIn;

            if (init->numChildren == 1 
                && matchKeyword("in")) { 

                scanner.lex();
                left = init;
                if (left != nullptr) {
                    forIn = true;                    
                    node->initJV(Synt::ForInStatement);
                    node->reg(text::_left, left);
                }
                right = parseExpression();

                if (forIn) {
                    node->reg(text::_right, right);
                }
                init = nullptr;
            }
        } else {
            state.allowIn = false;
            init = parseExpression();
            state.allowIn = previousAllowIn;

            if (matchKeyword("in")) {
                // LeftHandSideExpression
                if (!isLeftHandSide(init)) {
                    task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                                       Messages[Mssg::InvalidLHSInForIn],
                                       {});
                }

                scanner.lex();
                left = init;
                if (left != nullptr) {
                    forIn = true;
                    node->initJV(Synt::ForInStatement);
                    node->reg(text::_left, left);
                }
                right = parseExpression();
                if (forIn) {
                    node->reg(text::_right, right);
                }
                init = nullptr;
            }
        }

        if (! forIn) {            
            expect(";");
        }
    }
    if (! forIn) {        
        node->initJV(Synt::ForStatement);   
        node->reg(text::_init, init);
                
        test = nullptr;
        if (!match(";")) {
            test = parseExpression(); 
        }
        node->reg(text::_test, test);
        expect(";");

        update = nullptr;
        if (!match(")")) {
            update = parseExpression();
        }
        node->reg(text::_update, update);
    }

    expect(")");

    oldInIteration = state.inIteration;
    state.inIteration = true;

    body = parseStatement();
    node->reg(text::_body, body);

    state.inIteration = oldInIteration;

    if (forIn) {
        node->jv->assign(text::_each, false);
    }
    node->finish();
    DEBUGOUT("parseForStatement", false);
    return node;
}

// 12.7 The continue statement
//throw_
Node* ParseTools::parseContinueStatement(Node* node) {
    DEBUGIN(" parseContinueStatement(Node node)", false);
    Node *label;
    label = nullptr;
    string key;
    bool pltresult;

    expectKeyword("continue");

    // Optimize the most common form: 'continue;'.
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration)) {
            task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(nullptr);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!state.inIteration) {
            task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
        }

        node->finishContinueStatement(nullptr);
        DEBUGOUT("parseContinueStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {

        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);
        if (!(hasSet(key, state.labelSet))) {
            task->throwError(Tokenizer::NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == nullptr && !(state.inIteration)) {
        task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalContinue],{});
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
    label = nullptr;

    expectKeyword("break");

    // Catch the very common case first: immediately a semicolon (U+003B).
    if (source(idx) == 0x3B) {
        scanner.lex();

        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(nullptr);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }
    
    pltresult = peekLineTerminator();
    if (pltresult) {
        if (!(state.inIteration || state.inSwitch)) {
            task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalBreak],{});
        }

        node->finishBreakStatement(nullptr);
        DEBUGOUT("parseBreakStatement", false); 
        return node;
    }

    if (lookahead->type == TknType::Identifier) {
        label = parseVariableIdentifier();

        key = "$";
        key.append(label->name);

        if (!(hasSet(key, state.labelSet))) {
            task->throwError(Tokenizer::NULLPTRTKN, 
                       Messages[Mssg::UnknownLabel], {label->name});
        }
    }

    consumeSemicolon();

    if (label == nullptr && !(state.inIteration || state.inSwitch)) {
        task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalBreak], {});
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
    argument = nullptr;

    expectKeyword("return");

    if (!(state.inFunctionBody)) {
        task->throwErrorTolerant(Tokenizer::NULLPTRTKN, Messages[Mssg::IllegalReturn], {});
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
        node->finishReturnStatement(nullptr);
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
        task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
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
Node* ParseTools::parseSwitchCase(bool &testIsNull) {
    DEBUGIN(" parseSwitchCase()", false);
    Node *test, *statement, 
        *node = makeNode(true, true);
    vector< Node* > consequent;

    if (matchKeyword("default")) {
        scanner.lex();
        test = nullptr;
    } else {
        expectKeyword("case");
        test = parseExpression();
    }
    node->initJV(Synt::SwitchCase);
    testIsNull = (test == nullptr);
    node->reg(text::_test, test);
    const SFixedStr& path = text::_consequent;
    auto vec = doc->getReservedArr(node->jv, path);
    int ctr;
    expect(":");

    while (idx < length) {
        if (match("}") 
            || matchKeyword("default") 
            || matchKeyword("case")) {
            break;
        }
        statement = parseStatement();
        node->regPush(&vec, path, ctr, statement);
    }
    vec.complete();

    node->finish();
    DEBUGOUT("parseSwitchCase", false);
    return node;
}

/*

// 12.10 The swith statement
//throw_
Node* ParseTools::parseSwitchCase() {
    DEBUGIN(" parseSwitchCase()", false);
    Node *test, *statement, 
        *node = makeNode(true, true);
    vector< Node* > consequent;

    if (matchKeyword("default")) {
        scanner.lex();
        test = nullptr;
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
*/

//throw_
Node* ParseTools::parseSwitchStatement(Node *node) {
    DEBUGIN(" parseSwitchStatement(Node node)", false);
    Node *discriminant, *clause; 
    vector< Node* > cases; 
    bool oldInSwitch, defaultFound;

    //    node->initJV(Synt::SwitchStatement);

    expectKeyword("switch");
    expect("(");
    discriminant = parseExpression();
    //discriminant doesn't have member ->expressions
    //available to read yet.
    //node->reg(text::_discriminant, discriminant);
    //Value *vec = node->initVec(text::_cases);

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
        bool testIsNull = false;
        clause = parseSwitchCase(testIsNull);
        if (testIsNull) {
            if (defaultFound) {
                task->throwError(Tokenizer::NULLPTRTKN, 
                           Messages[Mssg::MultipleDefaultsInSwitch],{});
            }
            defaultFound = true;
        }
        //node->regPush(vec, text::_cases, clause);
        cases.push_back(clause);
    }

    state.inSwitch = oldInSwitch;
    expect("}");
    node->finishSwitchStatement(discriminant, cases);
    //    node->finish();
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
        task->throwError(Tokenizer::NULLPTRTKN, 
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
        task->throwErrorTolerant(Tokenizer::NULLPTRTKN, 
                           Messages[Mssg::StrictCatchVariable],{});
    }

    expect(")");
    body = parseBlock();
    node->finishCatchClause(param, body);
    DEBUGOUT("parseCatchClause", false);
    return node;
}

/* //throw_
Node* ParseTools::parseTryStatement(Node* node) {
    DEBUGIN(" parseTryStatement(Node node)", false);
    Node *block, *finalizer; 
    vector< Node* > handlers;
    node->initJV(Synt::TryStatement);

    finalizer = nullptr;

    expectKeyword("try");

    block = parseBlock();
    node->reg(text::_block, block);
    node->initVec(text::_guardedHandlers);
    Value * vecHandlers = node->initVec(text::_handlers);

    if (matchKeyword("catch")) {
#ifndef THROWABLE
        Node *tmp = parseCatchClause();
        node->regPush(vecHandlers, text::_handlers, tmp);
#endif
#ifdef THROWABLE
        node->regPush(vecHandlers, text::_handlers, 
                      parseCatchClause());
#endif
    }

    if (matchKeyword("finally")) {
        scanner.lex();
        finalizer = parseBlock();
    }

    if (handlers.size() == 0 && finalizer == nullptr) {
        task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::NoCatchOrFinally], {});
    }
    node->reg(text::_finalizer, finalizer);
    node->finish();

    DEBUGOUT("parseTryStatement", false);
    return node;
    }*/


//throw_
Node* ParseTools::parseTryStatement(Node* node) {
    DEBUGIN(" parseTryStatement(Node node)", false);
    Node *block, *finalizer; 
    vector< Node* > handlers;

    finalizer = nullptr;

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

    if (handlers.size() == 0 && finalizer == nullptr) {
        task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::NoCatchOrFinally], {});
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
            task->throwError(Tokenizer::NULLPTRTKN, Messages[Mssg::Redeclaration], 
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
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;
    StateStruct oldstate;

    node->initJV(Synt::BlockStatement);
    const SFixedStr &path = text::_body;
    auto vec = doc->getReservedArr(node->jv, path);
    int ctr=0;
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
        if (sourceElement->spareStrref != Syntax[Synt::Literal]) {
            //? this one I doubt there's more an efficient way to do this
            //? then json-c accesses. Storing node hierarchies just to fix this call seems to 
            //? be likely less performant.
            // this is not directive
            node->regPush(&vec, path, ctr, sourceElement); 
            break;
        }
        node->regPush(&vec, path, ctr, sourceElement); 
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
        if (sourceElement == nullptr) {
            break;
        }
        node->regPush(&vec, path, ctr, sourceElement);
    }
    vec.complete();

    expect("}");

    state.labelSet = oldstate.labelSet;
    state.inIteration = oldstate.inIteration;
    state.inSwitch = oldstate.inSwitch;
    state.inFunctionBody = oldstate.inFunctionBody;
    state.parenthesisCount = oldstate.parenthesisCount;

    node->finish();
    DEBUGOUT("parseFunctionSourceElements", false);
    return node;
}

/*//throw_
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
        if (sourceElement == nullptr) {
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

 */

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
    id = nullptr;

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
    if (task->strict && firstRestricted != 0x0 && !(firstRestricted->isNull)) {
        task->throwError(firstRestricted, message, {});
    }
    if (task->strict && stricted != 0x0 && !(stricted->isNull)) {
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

    id = nullptr;
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
                                                           &(text::_const)));
            } else {
                return DBGRET("", parseConstLetDeclaration("let", 
                                                           &(text::_let)));
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
    return nullptr;
}


//throw_ 
int ParseTools::parseSourceElements(Node * parent) {
    DEBUGIN(" parseSourceElementS() ", false);
    Node *sourceElement;
    vector< Node* > sourceElements;
    ptrTkn token, firstRestricted = scanner.makeToken();
    u16string directive;
    bool hasElements = false;

    const SFixedStr &path = text::_body;
    auto vec = doc->getReservedArr(parent->jv, path);
    int ctr=0;

    firstRestricted->isNull = true;
    while (idx < length) {
        token = lookahead;
        if (token->type != TknType::StringLiteral) {
            break;
        }

        sourceElement = parseSourceElement();
        //#todo make a function that accepts vector of nested finds
        //#so we can make tests like this more legible.
        hasElements = true;
        if (sourceElement->spareStrref != Syntax[Synt::Literal]) {
            // this is not directive
            parent->regPush(&vec, path, ctr, sourceElement);
            break;
        }
        parent->regPush(&vec, path, ctr, sourceElement);
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

        if (sourceElement == nullptr) {
            break;
        }
        hasElements = true;
        parent->regPush(&vec, path, ctr, sourceElement);
    }
    vec.complete();

    DEBUGOUT("parseSourceElementS", false);
    return (int) hasElements; //throw52;
}

//throw_ 
Node* ParseTools::parseProgram() {
    DEBUGIN(" parseProgram()", false);
    Node *node;
    vector< Node* > body;

    scanner.skipComment(); //ev
    scanner.peek();
    node = makeNode(true, true);
    node->initJV(Synt::Program);
    task->strict = false;
    node->completedPos = parseSourceElements(node);
    for (int i=0; i<task->extra.bottomRightStack.size(); i++) {
        task->extra.bottomRightStack[i]->resolve(&(task->extra));
    }
    node->finish();

    if (task->extra.range) {
        WojsonArr rangearr = doc->getArr();
        rangearr.push(node->range[0]);
        rangearr.push(node->range[1]);        
        node->jv->assignColl(text::_range, &rangearr);
    }
    if (task->extra.loc) {
        WojsonMap locjson = doc->getMap();
        node->loc.toJson(&locjson, doc);
        node->jv->assignColl(text::_loc, &locjson);
    }

    DEBUGOUT("parseProgram", false);
    return node;
}

/*
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

        if (sourceElement == nullptr) {
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
}*/

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

SFixedStr ParseTools::parse(const bool retErrorsAsJson) {
    //debugin("parse()", false);
    WojsonDocument doc(true);
    this->doc = &doc;
    WojsonMap& outJson = doc.getRootMap();

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
            task->retError.toJson(&outJson, &doc, &extra);
            return doc.toDecompressedString(&outJson, true, text::decoder);
        }
        task->retAssertError.toJson(&outJson, &doc, &extra);
        return doc.toDecompressedString(&outJson, true, text::decoder);
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
            e.toJson(&outJson, &doc, &extra);
            return doc.toDecompressedString(&outJson, true, text::decoder);
        }
        throw e;
    }
#endif
    outJson.assignColl(text::_program, programNode->jv);

    WojsonArr regexList = doc.getArr();
    programNode->regexPaths2json(regexList, &doc);

    outJson.assignColl(text::_regexp, &regexList);

   if (extra.commentTracking) {
       vec2jsonCallback<Comment>(&outJson, &doc, 
                                 &extra,
                                 text::_comments, extra.comments,
                                 &Comment::toJson); 
   }
   if (extra.tokenTracking) {
       scanner.filterTokenLocation();
       vec2jsonCallback<TokenRecord>(&outJson, &doc,
                                     &extra,
                                     text::_tokens,
                                     extra.tokenRecords,
                                     &TokenRecord::toJson);
   }
   //   string s = "debug";
   //Value dbgval((int) extra.tokenRecords.size());
   //outJson.AddMember(Value(s.data(), s.length(), alloc).Move(),
   //                  dbgval, alloc);

   if (extra.errorTolerant) {
       vec2jsonCallback<ExError>(&outJson, &doc, 
                                 &extra,
                                 text::_errors,
                                 extra.errors,
                                 &ExError::toJsonTolerant);
   }

   extra.clear();
   programNode->delNode(programNode);
   return doc.toDecompressedString(&outJson, true, text::decoder);
}
