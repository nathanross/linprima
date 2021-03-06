#ifndef JSON_DECOMPRESSOR_HPP
#define JSON_DECOMPRESSOR_HPP

#line 4 "JsonDecompressor.hpp"
#include "FixedString.hpp"
#include "t52types.hpp"
#include <rapidjson/document.h>
#include <vector>
#include <string>

/* 

JsonDecompressor.decompress is a function that takes a json
and for each key, and string value that does not follow 
particular keys, uses a dictionary (implemented as an 
array) to replace that key or string value with an 
original. We can afford to do this because in the js ast,
the circumstances under which a string value may be 
arbitrary or not worth compressing 

1. don't have overlap with those good for compression.
ie. 
  a. for string values which include arbitrary user input 
  such as string literals, there aren't many strings which 
  predictably show up at high frequency those that do are 
  not long enough and/or predictable enough to justify 
  compression.
  b. operators which, almost never would experience savings 
  via compression have their own distinct key they 
  correspond to, so they take up a small amount of space 
  while we can use their own characters to represent a larger
  number of vars.

2. inicate themselves clearly, from previous characters at 
  set relative positions (in a maximally compressed json) 
  so you can tell if a string value exists in such a position.

*/

#ifdef LOWMEM

class JsonDecompressor {
public:
    typedef char Ch;


    JsonDecompressor(
#ifdef LIMITJSON
                     std::vector<fixedstring::FixedString> * completeObjs, 
#endif
                     long len);
    void Put(Ch c);
    void Flush() { };
    void decompress(char *&out, long &lenOut);
    static std::string encodeObjId(size_t in);
    
    //dummy functions
    Ch Peek() const { assert(false); return '\0'; }
    Ch Take() { assert(false); return '\0'; }
    size_t Tell() const { return 0; }
    Ch* PutBegin() { assert(false); return 0; }
    size_t PutEnd(Ch*) { assert(false); return 0; }

private:
    const long MAX_BLOCK_SIZE = 200000;
    long len;
    long blockSize;

    long i;
    char * out;
#ifndef LIMITJSON
    char * current;
    std::vector<char *> blocks;
#endif
#ifdef LIMITJSON
    int objExpandSeq=0;
    std::vector<fixedstring::FixedString> * completeObjects;
    char lastChar;
    int firstDecodeIdx;
    int decodeIdx;
    int ignoreNextStrval;
    const char * decodedPtr;

    //needs to be at least as long as the longest
    //text expansion + marker length.

    size_t nextRealloc;

    //use vector to avoid another include.
    std::vector<const char*> putStack;
    std::vector<int> putStackLen;
    std::vector<int> putStackPos;
    std::vector<void *> putStackFixedStr;
    size_t addr;
    //    OStreamWrapper(const OStreamWrapper&);
    //OStreamWrapper& operator=(const OStreamWrapper&);    
#endif

    static int getDecodeIdx(char in);
};

#endif
#endif
