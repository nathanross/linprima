#ifndef WOJSON_HPP
#define WOJSON_HPP

#include "fixedstr.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#define reqinline __attribute__((always_inline)) inline

namespace wojson {
    extern const fixedstr::SFixedStr TRUESTR;
    extern const fixedstr::SFixedStr FALSESTR;
    extern const fixedstr::SFixedStr NULLSTR;

    extern const char LITERAL_HINT;

    class WojsonDocument;

    class WojsonColl  {
    public:
        WojsonColl(WojsonDocument *doc);
        virtual ~WojsonColl();

        virtual void getBookends(char &start, char &end) = 0;

        fixedstr::FixedStr toCompressedString();
        //add coll's toString val to the collregistry.
        //then return it's addr in the collregistry, encoded.

        //you can put in placeholder values if you want to finish a child node after the parent,
        //after its position in parent has been assigend, etc.
        WojsonDocument *doc;
    protected:
        std::vector<const void *> segments; //<const FixedString>, compiler throws error if you use const with typedef.
        std::vector<fixedstr::FixedStr> segmentsToDelete;
        bool first;
    };

    class WojsonArr : public WojsonColl {
    public:  
        WojsonArr(WojsonDocument *doc);
        ~WojsonArr();

        reqinline 
        virtual void getBookends(char &start, char &end) { 
            start = '[';
            end = ']';
        }

        //string is inserted directly w/o quotes.
        void movePushRaw(fixedstr::FixedStr val);
        void movePush(fixedstr::FixedStr val);
        //scoped additions:
        //for when the strings' lifetime exceeds 
        //that of this WojsonColl
        void scopedPushRaw(const fixedstr::SFixedStr &val);
        void scopedPush(const fixedstr::SFixedStr &val);
        //value is copied in push values below.

        reqinline
        void pushNull() 
        { scopedPushRaw(NULLSTR); }

        reqinline
        void push(bool val) 
        { scopedPushRaw((val)? TRUESTR: FALSESTR); }

        reqinline
        void push(int val) 
        { movePushRaw(fixedstr::getFixedStr(std::to_string(val))); }

        reqinline
        void push(unsigned int val) 
        { movePushRaw(fixedstr::getFixedStr(std::to_string(val))); }

        reqinline
        void push(double val) 
        { movePushRaw(fixedstr::getFixedStr(std::to_string(val))); }

        void pushColl(WojsonColl * val);

        size_t pushReserve();
    private:

    };

    class WojsonMap : public WojsonColl {
    public:
        WojsonMap(WojsonDocument *doc);
        ~WojsonMap();
        
        inline virtual void getBookends(char &start, char &end) { 
            start = '{';
            end = '}';
        }

        //string is inserted directly w/o quotes.
        void moveAssignRaw(const fixedstr::SFixedStr &key, 
                           fixedstr::FixedStr val);
        void moveAssign(const fixedstr::SFixedStr &key, 
                        fixedstr::FixedStr val);

        //scoped additions:
        //for when the strings' lifetime exceeds 
        //that of this WojsonColl
        void scopedAssignRaw(const fixedstr::SFixedStr &key, 
                             const fixedstr::SFixedStr &val);
        void scopedAssign(const fixedstr::SFixedStr &key, 
                          const fixedstr::SFixedStr &val);
        //value is copied in push values below.

        reqinline
        void assignNull(const fixedstr::SFixedStr &key) 
        { scopedAssignRaw(key, NULLSTR); }

        reqinline
        void assign(const fixedstr::SFixedStr &key, 
                           bool val) 
        { scopedAssignRaw(key, (val)? TRUESTR: FALSESTR); }

        reqinline
        void assign(const fixedstr::SFixedStr &key, 
                           int val) 
        { moveAssignRaw(key, fixedstr::getFixedStr(std::to_string(val))); }

        reqinline
        void assign(const fixedstr::SFixedStr &key, 
                       unsigned int val) 
        { moveAssignRaw(key, fixedstr::getFixedStr(std::to_string(val))); }

        reqinline
        void assign(const fixedstr::SFixedStr &key, 
                           double val) { 
            moveAssignRaw(key, 
                          fixedstr::getFixedStr(std::to_string(val))); 
        }

        size_t assignReserve(const fixedstr::SFixedStr &key);

        void assignColl(const fixedstr::SFixedStr &key, 
                               WojsonColl * val);
        // final: delete Colls in the finishedCollRegistry as they 
        // are integrated into the decompressed string. Only do 
        // this if you are done with this map and all children 
        // of the map, and this map is not added to any parent 
        // nodes within the document. As a side note, if you're 
        // going to decompress a map separately, it's probably 
        // better to have it as its own separate document.

        // texpansions: 
        // texpansions are useful for when many of the strings 
        // in your json are going to be the same value over
        // and over again, and minimization of max heap is 
        // critical. whether quoted keys and values whose strings 
        // begin with an alphanumeric value are by default 
        // assumed to be encoded base-alphanumeric address 
        // of a text expansion (textpansion) to substitute 
        // that string with.

        // decoder: an array of fixed strings from which to look 
        // up this address and find the text to replace it with.
        
        // literalChar : strings which have this (non-alphanumeric)
        // char appended to the beginning (should also not match 
        // char that begins the Coll Marker sequence) are not treated 
        // as addresses for text expansion or coll expansion, but 
        // instead have the char removed and treated as a regular 
        // string. 
        fixedstr::SFixedStr toDecompressedString(bool final,
                                                const char ** decoder);

        inline fixedstr::SFixedStr toDecompressedString(bool final) {
            return toDecompressedString(final, nullptr);
        }
    };


      // instead of :
      // size_t i = arr.pushReserve()
      // auto m = doc.getMap();
      // ... (add to m)
      // doc.replaceCollContents(i, m);

      //reserved allows you to do:
      // auto m = doc.getReservedMap(arr);
      // ... (add to m)
      // m.complete();

    class ReservedWojsonMap : public WojsonMap {
    public:
        ReservedWojsonMap(WojsonDocument *doc,
                          size_t onCompleteAddrArg) :
            WojsonMap(doc),
            onCompleteAddr(onCompleteAddrArg) {
        }
        void complete();
    private:
        size_t onCompleteAddr;
    };

    class ReservedWojsonArr : public WojsonArr {
    public:  
        ReservedWojsonArr(WojsonDocument *doc,
                  size_t onCompleteAddrArg) : 
            WojsonArr(doc),             
            onCompleteAddr(onCompleteAddrArg) {
        }
        void complete();
    private:
        size_t onCompleteAddr;
    };

    class WojsonDocument {
    public:
        WojsonDocument(bool useTexpansionsArg);

        ~WojsonDocument();

        WojsonMap& getRootMap();

        reqinline 
        WojsonMap getMap() 
        { return WojsonMap(this); }

        reqinline 
        WojsonArr getArr() 
        { return WojsonArr(this); }

        reqinline 
        ReservedWojsonMap getReservedMap(WojsonArr *arr) {
            return ReservedWojsonMap(this,
                                     arr->pushReserve());
        }

        reqinline 
        ReservedWojsonMap getReservedMap(WojsonMap *map, 
                                 const fixedstr::SFixedStr &key) {
            return ReservedWojsonMap(this,
                                     map->assignReserve(key));
        }
        
        reqinline 
        ReservedWojsonArr getReservedArr(WojsonArr *arr) {
            return ReservedWojsonArr(this,
                                     arr->pushReserve());
        }

        reqinline 
        ReservedWojsonArr getReservedArr(WojsonMap *map, 
                                 const fixedstr::SFixedStr &key) {
            return ReservedWojsonArr(this,
                                     map->assignReserve(key));
        }

        fixedstr::SFixedStr toDecompressedString(WojsonMap *map,
                                         bool final,
                                         const char ** decoder);

        reqinline
        fixedstr::SFixedStr toDecompressedString(WojsonMap *map,
                                                       bool final) {
            return toDecompressedString(map, final, nullptr);
        }

        fixedstr::FixedStr regColl(size_t *retAddr, 
                                   fixedstr::FixedStr collCompressedString);
        
        reqinline
        void replaceCollContents(size_t addr, fixedstr::FixedStr in) {
            finishedCollRegistry[addr] = in;
        }

    private:
        std::vector<fixedstr::FixedStr> finishedCollRegistry;
        bool useTexpansions;
        WojsonMap rootmap;
    };

}

#undef reqinline
#endif
