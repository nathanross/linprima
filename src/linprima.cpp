#line 1 "linprima.cpp"
#include "linprima.hpp"
#include "fixedstr.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

using namespace std;
using namespace rapidjson;
using namespace wojson;

typedef Document::AllocatorType AllocatorType;

SFixedStr tokenizeRetString(const u16string code, 
                              OptionsStruct options){

    Tokenizer tknr(code, options);
    return tknr.tokenize(true);
}

//# return json as string.
SFixedStr parseRetString(const u16string code, 
                           OptionsStruct options) {
    ParseTools pt(code, options);
    return pt.parse(true);   
}


char * outchars = new char[1];

//# not the prettiest solution, but it works for now.
//# maybe use smart pointer to free mem once returned?
//# it'd have to be compatible with c usage as regular pointer though.
//# fine if you have to increment it in a subcall.
char* strToChar(string in) {
    delete[] outchars;
    outchars = new char[in.size()+1];
    strcpy(outchars, in.c_str());
    return outchars;
}


extern "C" {

    char* tokenizeExtern(const char *code, const char* options) {
        return fixedstr::data(tokenizeRetString(string(code), 
                                          OptionsStruct(options)).f);

    }
    char* parseExtern(const char *code,
                      const char* options) {
        return fixedstr::data(parseRetString(string(code),
                                       OptionsStruct(
                                         options)).f);

    }

    // #emscripten will sometimes, on receipt of certain unicode chars,
    // append options any second char arg array to code. 
    // because this only happens sometimes, you can't 
    // just subtract length of second arg
    // available workarounds pending any emcc fixes
    // are compare ending text of code and assume
    // no one would deliberately pass code which ended with
    // the value of the options array
    // or simply pass a length arg in limprima-wrap in ASM

    // node that because js stores strings in UCS-2 or UTF-16
    // with some complications, we have to convert to UTF-16
    // before using substring, because the length of the char*
    // argument here is going to be byte length, not number of
    // characters as represented in javascript strings (ucs)
    char* tokenizeASMJS(const char *code, int codelen,
                        const char* options) {
        //printf("received string: =%s=\n", code);

        return fixedstr::data(tokenizeRetString(
                         toU16string(string(code)).substr(0,codelen), 
                                    OptionsStruct(options)).f);

    }
    char* parseASMJS(const char *code, int codelen, 
                      const char* options) {
        return fixedstr::data(parseRetString(
                    toU16string(string(code)).substr(0, codelen), 
                                       OptionsStruct(options)).f);

    }

}


#ifdef HASMAIN
#include <fstream>

int main() {
    string result, allopt;
    unsigned int resultlength = 0;
    
    string finput;
    string finputOpt;
    ifstream ifs("/home/n/coding/esp3/bench/cases/active/mootools.js");
    //ifstream ifs("/home/n/coding/esp7/test/codetotest.js");
    ifstream optifs("/home/n/coding/esp7/test/opttotest");

    finput.assign( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );    
    finputOpt.assign( (std::istreambuf_iterator<char>(optifs) ),
                    (std::istreambuf_iterator<char>()    ) );    

    vector<string> codeSamples = { finput };
    
    //vector<string> codeSamples = { 
    //   "var x = { null: 42 }" 
            //};
   
    //allopt = finputOpt;
    allopt = "{ \"range\":true, \"loc\": true }";
    //allopt = "{ \"loc\":true, \"range\":true, \"tokens\":true }";
    //    ProfilerStart("/tmp/profile2");

    //system_clock::time_point begin = system_clock::now();
    int reps = 1;
    for (int j = 0; j<reps; j++) {
        for (unsigned int i=0; i<codeSamples.size(); i++){ 
           //result = string(tokenizeRetString(
           result = string(parseRetString(
                                           toU16string(codeSamples[i]),
                                           OptionsStruct(allopt.data())));
            resultlength += result.length() % 6;
        }
    }
#ifdef LOWMEM
    free (asmRetVal);
#endif
    codeSamples.clear();
    finput = "";
    
    //system_clock::time_point end = system_clock::now();
    //auto timediff = end - begin;
    //int millis = std::chrono::duration_cast<std::chrono::milliseconds>(timediff).count();
    //printf("milliseconds: %i\n", (int) ((double) millis / (double) reps));
    //    ProfilerStop();
    printf("total length %u\n", resultlength);

    printf("text::_Identifier %s\n", text::_Identifier.s);
    
    ofstream lastResult;
    lastResult.open("/tmp/lastresult.txt");
    lastResult << result;
    lastResult.close();
    

    //printf("last result %s\n", result.data());
}
#endif
