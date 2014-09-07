#line 1 "WrappingNode.cpp"
#include "WrappingNode.hpp"
using namespace std;
using namespace rapidjson;
WrappingNode::WrappingNode(ptrTkn startToken, 
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

WrappingNode::WrappingNode(vector<Node*>*heapNodesArg,
                           AllocatorType* alloc,
                           LinprimaTask* task)
    : Node(false, true, heapNodesArg, 
           alloc, task) {
    DEBUGIN("WrappingNode(Token)", true);
    if (!hasJv) { 
        jv.SetObject();
        hasJv=true;
    }
    DEBUGOUT("WrappingNode(Token)", true);
}

void WrappingNode::usualInit(ptrTkn startToken) {
    if (task->extra.range) {
        hasRange = true;
        range[0] = startToken->start;
        range[1] = 0;
    }

    if (task->extra.loc) {
        loc = this->WrappingSourceLocation(startToken);
    }
}

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
