#line 1 "jsonutils.h"

//convenience function for using l-value (stack/non-temporary) as
//json keys when using AddMember
inline void MemberAdd(Value& out,
                      const string& key, 
                      Value& val, 
                      AllocatorType& alloc) {
    out.AddMember(Value(key.data(), key.length(), alloc).Move(),
                  val,
                  alloc);
}
inline void MemberAdd(Value& out,
                      const GenericStringRef<char> key, 
                      Value& val, 
                      AllocatorType& alloc) {
    out.AddMember(key,
                  val,
                  alloc);
}

//adds string via copy semantics.
void jsonAddString(Value& out, AllocatorType* alloc, 
                   const string key, string val) {
    MemberAdd(out, key, 
                  Value(val.data(), 
                        val.length(), 
                        *alloc).Move(),
                  *alloc);
}
//adds string via copy semantics.
void jsonAddStringRef(Value& out, AllocatorType* alloc, 
                   const GenericStringRef<char> key, string val) {
    out.AddMember(key, 
                  Value(val.data(), 
                        val.length(), 
                        *alloc).Move(),
                  *alloc);
}

template <typename T>
void vec2jsonCallback(Document& root,
                      AllocatorType* alloc,
                      const ExtraStruct *extra,
                      const StrRef &path,
                      vector<T> in,
                      function<void (T&, 
                                     const ExtraStruct*extra,
                                     Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    Value arr(kArrayType);
    Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    root.AddMember(path, arr, *alloc);
    //DEBUGOUT("", false);     
}

template <typename T>
void vec2jsonCallbackVal(Value& root,
                         AllocatorType* alloc,
                         const ExtraStruct *extra,
                         const StrRef &path,
                         vector<T> in,
                         function<void (T&, 
                                     const ExtraStruct*extra,
                                     Value& el, 
                                     AllocatorType* alloc)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    Value arr(kArrayType);
    Value el;
    for (int i=0; i<in.size(); i++) {
        el.SetObject();        
        f(in[i], extra, el, alloc);
        arr.PushBack(el, *alloc);
    }
    root.AddMember(path, arr, *alloc);
    //DEBUGOUT("", false);     
}