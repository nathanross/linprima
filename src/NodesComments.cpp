#line 1 "NodesComments.cpp"
#include "NodesComments.hpp"
#include "Node.hpp"
#include "jsonutils.hpp"
using namespace std;
using namespace rapidjson;

NodesComments::NodesComments(AllocatorType* alloc): 
    nodesAlloc(alloc),
    resolved(true)
#ifdef LIMITJSON
    , nodeIsDetached(false) 
#endif
{
    nodesJv = 0x0;
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}
NodesComments::NodesComments(Document& jv, AllocatorType* alloc) : 
    nodesAlloc(alloc),
    resolved(true)
#ifdef LIMITJSON
    , nodeIsDetached(false) 
#endif
{
    this->nodesJv = &jv;
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}

void NodesComments::commentsIntoJson(const ExtraStruct *extra,
                                     const bool leading) { 
    //DEBUGIN(" NodesComments::commentsIntoJson(const bool leading)", false);
    const StrRef * key;
    vector<Comment> * commentVec;
    if (leading) {
        key = (&(text::_leadingComments));
        commentVec = &leadingComments;
    } else {
        key = (&(text::_trailingComments));
        commentVec = &trailingComments;
    }
    Value::ConstMemberIterator itr = nodesJv->FindMember(key->s);
    if (itr != nodesJv->MemberEnd())  {
        nodesJv->EraseMember(itr);
        //switch to RemoveMember if this function becomes timesink.
    }
    if (commentVec->size() > 0) {
#ifdef LIMITJSON
        vec2jsonCallback<Comment>(*nodesJv,
#endif
#ifndef LIMITJSON
        vec2jsonCallbackVal<Comment>(*nodesJv,
#endif
                                  nodesAlloc,
                                  extra,
                                  *key,
                                  *commentVec,
                                  &Comment::toJson);
    }
    //DEBUGOUT("commentsIntoJSon", false);
}
#ifdef LIMITJSON
void NodesComments::setNodeDetached(Node *detachedNode) {
    //call this if the only reason the node is still alive
    //is because of the comment. in highmem, you can delete
    //the node and still have access to its rapidjson object
    //but in lowmem mode, the lifetime of the document is tied
    //as a resource of the node (all node jv's children are allocated using
    //the jv's own allocator, instead of the allocator of the entire
    //task's root document) so the node is be kept alive
    //until any json changes for its contents are resolved.
    nodeIsDetached = true;
    this->detachedNode = detachedNode;
}
#endif
void NodesComments::resolve() {
#ifdef LIMITJSON
    if (nodeIsDetached) {
        detachedNode->lateResolve();
    }
#endif
    resolved = true;
}
