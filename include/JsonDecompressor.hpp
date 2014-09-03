#ifndef JSON_DECOMPRESSOR_HPP
#define JSON_DECOMPRESSOR_HPP

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
                     vector<string> * completeObjs, 
#endif
                     long len);
    void Put(Ch c);
    void Flush() { };
    void decompress(char *&out, long &lenOut);
    
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
    vector<char*> blocks;
    char * current;

#ifdef LIMITJSON
    int objExpandSeq=0;
    vector<string> * completeObjects;

    const int OBJ_NONE = 0;
    const char OBJ_MARKER_BEGIN=1;
    const char OBJ_MARKER_END=5;
    const int OBJ_GET_ADDR =7;
    const char *MARKER = "\"#`@$";

    //use vector to avoid another include.
    vector<const char*> putStack;
    vector<int> putStackLen;
    vector<int> putStackPos;
    int addr;
    //    OStreamWrapper(const OStreamWrapper&);
    //OStreamWrapper& operator=(const OStreamWrapper&);    
#endif

    static int getDecodeIdx(char in);
};

#endif
#endif
