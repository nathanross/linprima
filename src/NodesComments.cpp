#line 1 "NodesComments.cpp"
#include "NodesComments.hpp"
#include "Node.hpp"
#include "jsonutils.hpp"
using namespace std;
using namespace wojson;
using namespace fixedstr;

NodesComments::NodesComments(WojsonDocument* docArg): 
    doc(docArg),
    resolved(true),
    nodeIsDetached(false) 
{
    jv = 0x0;
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}
NodesComments::NodesComments(WojsonMap &jvArg, WojsonDocument* docArg) : 
    jv(&jvArg),
    doc(docArg),
    resolved(true),
    nodeIsDetached(false) 
{
    isNull = false;
    range[0] = -1;
    range[1] = -1;
    leadingComments.clear();
    trailingComments.clear();
}

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

void NodesComments::resolve(const ExtraStruct *extra) {
    const SFixedStr * key;
    vector<Comment> * commentVec;
    for (int i=0; i<2; i++) {        
        if (i == 0) {
            key = (&(text::_leadingComments));
            commentVec = &leadingComments;
        } else {
            key = (&(text::_trailingComments));
            commentVec = &trailingComments;
        }

        if (commentVec->size() > 0) {
            vec2jsonCallback<Comment>(jv, doc,
                                      extra,
                                      *key,
                                      *commentVec,
                                      &Comment::toJson);
        }
    }
    if (nodeIsDetached) {
        detachedNode->lateResolve();
    }
    resolved = true;
}
