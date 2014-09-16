#ifndef JSONUTILS_HPP
#define JSONUTILS_HPP

#line 4 "jsonutils.hpp"
#include "podt.hpp"
#include "t52types.hpp"
#include "debug.hpp"
#include "wojson.hpp"
#include "fixedstr.hpp"
#include <vector>
#include <functional>

template <typename T>
void vec2jsonCallback(wojson::WojsonMap *root,
                      wojson::WojsonDocument *doc,                      
                      const ExtraStruct *extra,
                      const fixedstr::SFixedStr &path,
                      std::vector<T> in,
                      std::function<void (T&, 
                                          wojson::WojsonMap *el,
                                          wojson::WojsonDocument *doc,
                                          const ExtraStruct*extra)> const& f) {
    //DEBUGIN("vec2JsonCallback", false);
    wojson::WojsonArr arr = doc->getArr();
    for (int i=0; i<in.size(); i++) {
        wojson::WojsonMap el = doc->getMap(); 
        f(in[i], &el, doc, extra);
        arr.pushColl(&el);
    }
    root->assignColl(path, &arr);
    //DEBUGOUT("", false);     
}

#endif
