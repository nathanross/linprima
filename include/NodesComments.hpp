#ifndef NODES_COMMENTS_HPP
#define NODES_COMMENTS_HPP

#line 4 "NodesComments.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include <rapidjson/document.h>
#include <vector>

//# good to have these separated from individual nodes,
//# because unless we start storing nodes in heap,
//# what happens is we create a node, goes on stack,
//# reference it from extrastruct (storing it is a
//# prohibitive expense due to some expandable members)
//# exits stack, gone, ref. invalid.

//# better, even in highmem, to only keep in memory the data necessary.
//# these are the only data members that will be necessary
//# once the node has left the stack (barring Assignment
//# Expressions which which put a nodecopy on the heap)

class Node;

class NodesComments {
public:
    std::vector<Comment> leadingComments;
    std::vector<Comment> trailingComments;
#ifdef LIMITJSON
    rapidjson::Document * nodesJv;
#endif
#ifndef LIMITJSON
    rapidjson::Value * nodesJv;
#endif
    AllocatorType * nodesAlloc;
    int range[2];
    bool isNull;
    bool resolved;
    NodesComments(AllocatorType* alloc);
    NodesComments(rapidjson::Document& jv,AllocatorType* alloc);
    void commentsIntoJson(const ExtraStruct *extra,
                          const bool leading);
    void resolve();
#ifdef LIMITJSON
    void setNodeDetached(Node *detachedNodeAddr);
private:
    bool nodeIsDetached;
    Node * detachedNode;
#endif
};

#endif
