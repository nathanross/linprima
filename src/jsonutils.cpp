#line 1 "jsonutils.cpp"
#include "jsonutils.hpp"
#include "fixedstr.hpp"
#include "LinprimaTask.hpp"
#include "debug.hpp"
#include <stdio.h>
#include <rapidjson/writer.h>
using namespace std;
using namespace rapidjson;



fixedstr::FixedStr lstr(string in) {    
    //DEBUGIN("lstr", false);
    rapidjson::StringBuffer rjbuffer;
    rapidjson::Document d;
    //printf("llstr src ==%s== \n", in.data());
    rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
    d.SetObject();
    const char * dummykey = "dummykey";
    d.AddMember(rapidjson::StringRef(dummykey), 
                rapidjson::Value(in.data(),
                                 in.length(),
                                 alloc).Move(), 
                alloc);
    rjbuffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> rjwriter(rjbuffer);
    d.Accept(rjwriter);
    string out = rjbuffer.GetString();
    out = out.substr(13, out.length()-15);
    //printf("llstr out ==%s== \n", out.data());
    out.insert(out.begin(), wojson::LITERAL_HINT);
    return fixedstr::getFixedStr(out);
}
