#include "wojson.hpp"
#include "debug.hpp"

using namespace std;
using namespace fixedstr;

namespace wojson {

    const fixedstr::SFixedStr TRUESTR("true");
    const fixedstr::SFixedStr FALSESTR("false");
    const fixedstr::SFixedStr NULLSTR("null");
    const char LITERAL_HINT = '#';

    const char * MARKER = "@%";
    const int MARKERLEN = strlen(MARKER);
    const fixedstr::SFixedStr COMMA(",");
    const fixedstr::SFixedStr COMMAQUOTE(",\"");
    const fixedstr::SFixedStr QUOTE("\"");
    const fixedstr::SFixedStr KEYMID("\":\"");
    const fixedstr::SFixedStr KEYMID_RAW("\":");
    
    inline
    int getDecodeIdx(char in) {
        if (in >= '0' && in <= '9') {
            return in - '0';
        } else if (in >= 'A' && in <= 'Z') {
            return (in - 'A') + 10;
        } else if (in >= 'a' && in <= 'z') {
            return (in - 'a') + 36;
        } else {
            return -1;
        }
    }

    WojsonColl::WojsonColl(WojsonDocument *docArg) :
        doc(docArg),
        first(true) {
    }
    WojsonColl::~WojsonColl() {
        for (size_t i=0; i<segmentsToDelete.size();i++){ 
            free(segmentsToDelete[i]);
        }
    }

    fixedstr::FixedStr WojsonColl::toCompressedString() {
        //DEBUGIN("toCompressedString()", false);
        size_t len=2; //+2 bookends
        for (size_t s=0; s<segments.size(); s++) {
            len += fixedstr::length(segments[s]);
        }
        //+8 for long (len)
        //+2 for bookends
        //+1 for nullterm            
        fixedstr::FixedStr result = malloc(len+9);
        *((long *) result) = len;
            
        long outpos = 1;
        char * outchars = fixedstr::data(result);
        char collStart, collEnd;
        this->getBookends(collStart, collEnd);
        outchars[0] = collStart;
        char * segchars;
        long seglength;

        for (size_t s=0; s<segments.size(); s++) {
            segchars = fixedstr::data(segments[s]);
            seglength = fixedstr::length(segments[s]);
            memcpy(outchars+outpos, segchars, seglength);
            outpos+= seglength;
        }
        outchars[outpos] = collEnd;
        outchars[outpos+1] = '\0';


        return result;
    }

    WojsonArr::WojsonArr(WojsonDocument *docArg) :
        WojsonColl(docArg) {
    }
    WojsonArr::~WojsonArr() {
    }

    //string is inserted directly w/o quotes.
    void WojsonArr::movePushRaw(fixedstr::FixedStr val) {
        if (!first) {
            segments.push_back(COMMA.f);
        } else { first = false; }
        segments.push_back(val);
        segmentsToDelete.push_back(val);
    }
    void WojsonArr::movePush(fixedstr::FixedStr val) {
        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else { 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(val);
        segments.push_back(QUOTE.f);
        segmentsToDelete.push_back(val);
    }
    //scoped additions:
    //for when the strings' lifetime exceeds 
    //that of this WojsonColl
    void WojsonArr::scopedPushRaw(const fixedstr::SFixedStr &val) {
        if (!first) {
            segments.push_back(COMMA.f);
        } else { first = false; }
        segments.push_back(val.f);
    }

    void WojsonArr::scopedPush(const fixedstr::SFixedStr &val) {
        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else {                 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(val.f);
        segments.push_back(QUOTE.f);
    }

    void WojsonArr::pushColl(WojsonColl * val) 
    { movePush(doc->regColl(nullptr, val->toCompressedString())); }

    size_t WojsonArr::pushReserve() {
        size_t retAddr;
        movePush(doc->regColl(&retAddr, nullptr));
        return retAddr;
    }

    WojsonMap::WojsonMap(WojsonDocument *doc) :                        
        WojsonColl(doc) {
    }

    WojsonMap::~WojsonMap() {
    }

    void WojsonMap::moveAssignRaw(const fixedstr::SFixedStr &key, 
                                  fixedstr::FixedStr val) {
        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else { 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(key.f);
        segments.push_back(KEYMID_RAW.f);
        segments.push_back(val);

        segmentsToDelete.push_back(val);
    }

    void WojsonMap::moveAssign(const fixedstr::SFixedStr &key, 
                               fixedstr::FixedStr val) {

        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else { 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(key.f);
        segments.push_back(KEYMID.f);
        segments.push_back(val);
        segments.push_back(QUOTE.f);

        segmentsToDelete.push_back(val);
    }

    void WojsonMap::scopedAssignRaw(const fixedstr::SFixedStr &key, 
                                    const fixedstr::SFixedStr &val) {
        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else { 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(key.f);
        segments.push_back(KEYMID_RAW.f);
        segments.push_back(val.f);
    }
    
    void WojsonMap::scopedAssign(const fixedstr::SFixedStr &key, 
                                 const fixedstr::SFixedStr &val) {

        if (!first) {
            segments.push_back(COMMAQUOTE.f);
        } else { 
            segments.push_back(QUOTE.f);
            first = false; 
        }
        segments.push_back(key.f);
        segments.push_back(KEYMID.f);
        segments.push_back(val.f);
        segments.push_back(QUOTE.f);
    }

    size_t WojsonMap::assignReserve(const fixedstr::SFixedStr &key) {
        size_t retAddr;
        moveAssign(key,
                   doc->regColl(&retAddr, nullptr));
        return retAddr;
    }

    void WojsonMap::assignColl(const fixedstr::SFixedStr &key, 
                               WojsonColl * val) 
    { moveAssign(key, doc->regColl(nullptr, val->toCompressedString())); }

    SFixedStr WojsonMap::toDecompressedString(bool final,
                                              const char ** decoder) {
        return doc->toDecompressedString(this, final, decoder);
    }

    void ReservedWojsonMap::complete() {
        doc->replaceCollContents(onCompleteAddr, toCompressedString());
    }
    void ReservedWojsonArr::complete() {
        doc->replaceCollContents(onCompleteAddr, toCompressedString());
    }

    WojsonDocument::WojsonDocument(bool useTexpansionsArg) :
            useTexpansions(useTexpansionsArg),
            rootmap(this) {       
    }
    WojsonMap& WojsonDocument::getRootMap() {
        return rootmap;
    }
    WojsonDocument::~WojsonDocument() {
        for (size_t i=0; i<finishedCollRegistry.size(); i++) {
            if (finishedCollRegistry[i] != nullptr) {
                free(finishedCollRegistry[i]);
            }
        }
    }

    SFixedStr WojsonDocument::toDecompressedString(WojsonMap *map,
                                                   bool final,
                                                   const char ** decoder) {
        //DEBUGIN("toDecompressedString()", false);
        std::vector<const char*> dataStack;
        std::vector<long> lenStack;
        std::vector<long> posStack;
        std::vector<size_t> addrStack;
        //printf("\n%c|",in);
        FixedStr rootstring = map->toCompressedString();
        //printf("root string %s length %li \n", data(rootstring),
        //       length(rootstring));
        dataStack.push_back(data(rootstring));
        lenStack.push_back(length(rootstring));
        posStack.push_back(0);
        addrStack.push_back(0);

        //review:
        //current: current block
        //i: current position in block
        //c: current character in source document or expanded string
        //    expanded string is read right after quotation following
        //    addr is read.
        //putStack: stack of expanded strings.
        long inpos=0;

        bool isRoot = (map == &rootmap);

        int REALLOC_SLACK = 200; 
        //should be longer than longest text expansion + 10.

        size_t blockSize = finishedCollRegistry.size() * 30;
        FixedStr fout = malloc(8+blockSize+REALLOC_SLACK);
        int FIXED_STR_HEADER_SIZE = 8;
        char *out = ((char *) fout) + FIXED_STR_HEADER_SIZE;
        size_t nextRealloc = blockSize;
        size_t i=0;

        const int OBJ_NONE = 0;
        const int OBJ_FOUND_INIT =1;
        const int OBJ_FOUND_QUOTE =2;
        const int OBJ_TEXPANSION =3;
        const int OBJ_FOUND_NODE_MARKER_BEGIN = 5;
        const int OBJ_GET_NODE = MARKERLEN + OBJ_FOUND_NODE_MARKER_BEGIN - 1;


        int objExpandSeq = 0;
        char c;
        //char lastChar;
        long texpAddr;
        long texpVal;
        //int ignoreNextStrval;
        const char * decodedPtr;
        size_t addr;
            
        while (! dataStack.empty()) {
            if (i >= nextRealloc) {            
                fout = realloc(fout, 
                               nextRealloc + REALLOC_SLACK + blockSize + 
                               FIXED_STR_HEADER_SIZE);
                out = ((char *) fout) + FIXED_STR_HEADER_SIZE;
                nextRealloc = (nextRealloc + blockSize);
            }
            if (inpos == lenStack.back()) {
                dataStack.pop_back();
                lenStack.pop_back();
                posStack.pop_back();
                if (dataStack.empty()) {
                    free(rootstring);
                    break;
                }
                if (isRoot && final) {
                    addr = addrStack.back();
                    free(finishedCollRegistry.at(addr));
                    finishedCollRegistry[addr] = nullptr;
                }
                addrStack.pop_back();
                inpos = posStack.back();
                //printf(" stackframe complete <<<<<< \n");
                //current[i] = '}';
                //++i;
                continue;
            }
            //printf("\n\n      depth %i pos %i out of len %i \n", (int) posStack.size(), (int) inpos, (int) lenStack.back());
            c = dataStack.back()[inpos];
            //printf("%c     -",c);

            //printf("seq %i ", (int) objExpandSeq);
            if (objExpandSeq == OBJ_NONE && 
                c != '[' &&
                c != ',' &&
                c != ':' &&
                c != '{') {

                out[i] = c;
                ++i;
                ++inpos;
                continue;
            } else if (objExpandSeq == OBJ_NONE) {            
                //  if (ignoreNextStrval > 0) {
                //    ignoreNextStrval--;
                //}
                //if (c != ':') {
                //    ignoreNextStrval = 0;
                //}
                //printf(" found init  ");    
                objExpandSeq = OBJ_FOUND_INIT;

            } else if (objExpandSeq == OBJ_FOUND_INIT) {
                if (c == '"') {
                    ++inpos;
                    //printf(" found quote  ");    
                    objExpandSeq = OBJ_FOUND_QUOTE;
                    continue;
                } else {
                    objExpandSeq = OBJ_NONE;
                    continue; //check for a new init here.
                }
            } else if (objExpandSeq == OBJ_FOUND_QUOTE) {
                if (c == MARKER[0]) {
                    //printf(" found node expand sequence  ");    
                    objExpandSeq = OBJ_FOUND_NODE_MARKER_BEGIN;// + 1;
                    ++inpos;
                    continue;
                }

                out[i] = '"'; //add back the held quote.
                ++i;
                if (! useTexpansions || c == LITERAL_HINT) {
                    objExpandSeq = OBJ_NONE;
                    if (useTexpansions) { ++inpos; }
                    continue;
                }
                //eating first char;
                texpAddr = getDecodeIdx(c);
                if (texpAddr >= 0) {
                    //it's a valid char.
                    objExpandSeq = OBJ_TEXPANSION;
                    ++inpos;
                    continue;
                } else {
                    //not part of a sequence or empty str. no big deal.
                    objExpandSeq = OBJ_NONE;
                    continue; //check for a new init here.
                }
            } else if (objExpandSeq == OBJ_TEXPANSION) {
                //looking at second char;
                texpVal = getDecodeIdx(c);
                //if returned decodeIdx is -1, gets the 
                //first ptr in the array 
                // (the one for a single character)
                if (texpVal != -1) {
                    texpAddr *= 62;
                    texpAddr += texpVal;
                    ++inpos;
                    continue;
                }
                decodedPtr = decoder[texpAddr];

                    
                //printf("expanding: %s ", decodedPtr);

                strcpy(out + i, decodedPtr);
                while (out[i] != 0) { 
                    // replacement texts should always 
                    // have null terminator at end and
                    // only at end.
                    ++i;
                }
                out[i] = c;
                ++i;
                ++inpos;
                objExpandSeq = OBJ_NONE;
                continue;
                    
            } else if (objExpandSeq >= OBJ_FOUND_NODE_MARKER_BEGIN
                       && objExpandSeq < OBJ_GET_NODE) {
                if (c == MARKER[ 1 + 
                                 objExpandSeq - OBJ_FOUND_NODE_MARKER_BEGIN]) {
                    //printf(" matches marker ");
                    ++objExpandSeq;
                    ++inpos;
                    //printf(" objExpand %i objGetNode %i ",
                    //       (int) objExpandSeq,
                    //       (int) OBJ_GET_NODE);
                    if (objExpandSeq == OBJ_GET_NODE) {
                        addr = 0;
                    }
                    continue;
                } else {
                    for (int m = 0; 
                         m <= objExpandSeq - OBJ_FOUND_NODE_MARKER_BEGIN; 
                         m++) {
                        out[i] = MARKER[m];
                        ++i;
                    }
                    objExpandSeq = OBJ_NONE;
                    continue;
                }
            } else if (objExpandSeq == OBJ_GET_NODE) {
                if (c != '"') {
                    addr *= 62;
                    addr += getDecodeIdx(c);
                    //printf(" getting addr... ");
                    ++(inpos);
                    continue;
                } else {
                    ++(inpos);
                    posStack.pop_back();
                    posStack.push_back(inpos);

                    //printf(" entering str at completeobj addr %i >>>>>> ", (int) addr);

                    //printf("at completeObjs of size %i \n", 
                    //              (int) completeObjects->size());
                    FixedStr fs = 
                        finishedCollRegistry[addr];
                    dataStack.push_back(data(fs));
                    lenStack.push_back(length(fs));
                    addrStack.push_back(addr);
                    posStack.push_back(0);
                    inpos = 0;
                    objExpandSeq = OBJ_NONE;
                    continue;
                }
            } else if (objExpandSeq > OBJ_NONE) {
                objExpandSeq = OBJ_NONE;
                continue; 
                //check for the init of a new sequence 
                // at this character.
            }


            out[i] = c;
            ++i;    
            ++(inpos);
        }
        out[i] = '\0';
        *((long *) fout) = i;
        return SFixedStr(std::move(fout));
    }

    /*WojsonDocument::WojsonDocument() :
        rootmap(&finishedCollRegistry,true) {
        }*/

    fixedstr::FixedStr WojsonDocument::regColl(size_t *retAddr, 
                                fixedstr::FixedStr collCompressedString) {
        //62**6 includes ulong_max, which is used as the limit for # of supported nodes.
        //DEBUGIN("regColl", false);
        char rev[6], subst[10]; //assumes marker not > 4 chars.
        finishedCollRegistry.push_back(collCompressedString);
        size_t pos = finishedCollRegistry.size()-1;
        if (retAddr != nullptr) { (*retAddr) = pos; }
        int next, rev_i=0, i;
        while (pos > 0 || rev_i == 0) {
            next = pos % 62;
            if (next < 10) { next = '0' + next; }
            else if(next <36) { next = 'A' + next - 10; }
            else { next = 'a' + next - 36; }
            rev[rev_i] = next;
            ++rev_i;
            //if (rev_i > 5) { throw runtimeerror("exceeded # of nodes supported"); }
            pos = pos / 62;
        }
        --rev_i;
        strcpy(subst, MARKER);
        i = MARKERLEN;
        while (rev_i >= 0) {
            subst[i] = rev[rev_i];
            ++i;
            --rev_i;
        }
        subst[i] = '\0';
        return fixedstr::getFixedStr(subst, i);
    }

}

#ifdef WOJ_DRIVER

using namespace wojson;
//driver
int main() {
    wojson::WojsonDocument * doc = new wojson::WojsonDocument(true);
    WojsonMap& root = doc->getRootMap();
    SFixedStr hello("0");
    const char * decoder[4] = {"hello", "world", "somearr", "reserved"};

    root.moveAssign(hello.f,
                    getFixedStr("1"));
    WojsonArr arr = doc->getArr();
    arr.push(3);
    arr.push(true);



    WojsonMap map = doc->getMap();
    SFixedStr pi("#pi");
    map.assign(pi.f, 3.14);
    arr.pushColl(&map);


    SFixedStr somearr("2");
    root.assignColl(somearr.f, &arr);

    SFixedStr reserved("3");
    auto m = doc->getReservedMap(&root, reserved.f);

    std::string nullkey = std::string({wojson::LITERAL_HINT});
    nullkey.append("null");
    SFixedStr nullkeyf(nullkey);
    m.assignNull(nullkeyf.f);
    m.complete();

 
    SFixedStr &&result = doc->toDecompressedString(&root, true, decoder);
    printf("%s\n", data(result.f));
    delete doc;
    return 0;
}
#endif

