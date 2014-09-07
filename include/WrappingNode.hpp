#ifndef WRAPPING_NODE_HPP
#define WRAPPING_NODE_HPP

#line 4 "WrappingNode.hpp"

#include "debug.hpp"
#include "podt.hpp"
#include "Node.hpp"
#include "t52types.hpp"
#include <rapidjson/document.h>
#include <vector>
#include <string>

class WrappingNode : public Node {
public:
    WrappingNode(ptrTkn startToken, 
                 std::vector<Node*>*heapNodesArg,
                 AllocatorType* alloc,
                 LinprimaTask* task);
    WrappingNode(std::vector<Node*>*heapNodesArg,
                 AllocatorType* alloc,
                 LinprimaTask* task); 
    Loc WrappingSourceLocation(ptrTkn startToken);
    void usualInit(ptrTkn startTkn);
};

#endif
