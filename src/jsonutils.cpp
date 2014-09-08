#line 1 "jsonutils.cpp"
#include "jsonutils.hpp"
#include "JsonDecompressor.hpp"
#include "FixedString.hpp"
#include "LinprimaTask.hpp"
using namespace std;
using namespace rapidjson;

#ifdef LIMITJSON
void AddDocument(LinprimaTask* task,
            const StrRef &path, Document &root, Document &branch) {
    Writer<StringBuffer> writer(task->buffer);
    branch.Accept(writer);
    task->completeObjects->push_back(fixedstring::getFixedStr(
                                                task->buffer.GetString()));
    task->buffer.Clear();
    string objectAddr = JsonDecompressor::encodeObjId(task->completeObjects->size()-1);
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
    task->completeObjects->push_back(fixedstring::getFixedStr(
                                                   task->buffer.GetString()));
    task->buffer.Clear();
    string objectAddr = JsonDecompressor::encodeObjId(task->completeObjects->size()-1);
    root.PushBack(Value(objectAddr.data(),
                        objectAddr.length(),
                        alloc).Move(), 
                  alloc);
}
#endif
