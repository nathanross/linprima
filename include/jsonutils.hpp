#ifndef JSONUTILS_HPP
#define JSONUTILS_HPP

#line 4 "jsonutils.hpp"
#include "strref.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include "debug.hpp"
#include <rapidjson/document.h>
#include <vector>
#include <functional>

//convenience function for using l-value (stack/non-temporary) as
//json keys when using AddMember
inline void MemberAdd(rapidjson::Value& out,
                      const std::string& key, 
                      rapidjson::Value& val, 
                      AllocatorType& alloc) {
    out.AddMember(rapidjson::Value(key.data(), key.length(), alloc).Move(),
                  val,
                  alloc);
}
inline void MemberAdd(rapidjson::Value& out,
                      const StrRef key, 
                      rapidjson::Value& val, 
                      AllocatorType& alloc) {
    out.AddMember(key,
                  val,
                  alloc);
}

//adds string via copy semantics.
inline
void jsonAddString(rapidjson::Value& out, AllocatorType* alloc, 
                   const std::string key, std::string val) {
    MemberAdd(out, key, 
                  rapidjson::Value(val.data(), 
                        val.length(), 
                        *alloc).Move(),
                  *alloc);
}
//adds string via copy semantics.
inline
void jsonAddStringRef(rapidjson::Value& out, AllocatorType* alloc, 
                   const StrRef key, std::string val) {
    out.AddMember(key, 
                  rapidjson::Value(val.data(), 
                        val.length(), 
                        *alloc).Move(),
                  *alloc);
}

template <typename T>
void vec2jsonCallback(rapidjson::Document& root,
                      AllocatorType* alloc,
                      const ExtraStruct *extra,
                      const StrRef &path,
                      std::vector<T> in,
                      std::function<void (T&, 
                                     const ExtraStruct*extra,
                                     rapidjson::Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    rapidjson::Value arr(rapidjson::kArrayType);
    rapidjson::Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    root.AddMember(path, arr, *alloc);
    //DEBUGOUT("", false);     
}

template <typename T>
void vec2jsonCallbackVal(rapidjson::Value& root,
                         AllocatorType* alloc,
                         const ExtraStruct *extra,
                         const StrRef &path,
                         std::vector<T> in,
                         std::function<void (T&, 
                                     const ExtraStruct*extra,
                                     rapidjson::Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    rapidjson::Value arr(rapidjson::kArrayType);
    rapidjson::Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    root.AddMember(path, arr, *alloc);
    //DEBUGOUT("", false);     
}



#ifdef LIMITJSON
struct LinprimaTask;
void AddDocument(LinprimaTask* task,
                 const StrRef &path, 
                 rapidjson::Document &root, 
                 rapidjson::Document &branch);
void PushDocument(LinprimaTask* task, 
                  AllocatorType &alloc,
                  rapidjson::Value &root, 
                  rapidjson::Document &branch);
#endif



#endif
