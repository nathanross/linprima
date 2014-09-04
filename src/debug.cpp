#line 1 "debug.cpp"
#include "debug.hpp"
#include <vector>
#include <functional>
using namespace std;
using namespace rapidjson;

#ifdef DO_DEBUG
 
int debuglevel = 1;
vector<string> stackfuncs;

bool DEBUG_ON= (bool) 1;
bool HIPRI_ONLY= (bool) 1;

static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static string colorHash(string text) {
    int num=0; 
    string code = "";
    for (int i=0; i<text.length() && i < 7; i++) {
        num += (int) text[i];
    }
    code.append("\033[1;");
    code.append(to_string(31 + (num % 7)));
    code.append("m");
    return code;
}

void DEBUGIN(string in, bool lowprio) {    
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    debuglevel++;
    string msg = "";

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }    
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(ltrim(in))); }
    msg.append(ltrim(in));
    msg.append(to_string(debuglevel));
    msg.append("|");
    msg.append(to_string(*debugidx));
    msg.append("\033[0m\n");
    printf("%s", msg.data());
    stackfuncs.push_back(ltrim(in));
}

void DEBUGOUT(string in, bool lowprio) {
    if (!DEBUG_ON) { return; }
    if (HIPRI_ONLY && lowprio) { return; }
    string msg = "";
    string realtext = "";
    if (stackfuncs.size() > 0) {
        realtext = stackfuncs.back();
    }

    for (int i=0;i<debuglevel;i++) {
        msg.append("  ");
    }
    if (lowprio) { msg.append("\033[1;30m"); } 
    else { msg.append(colorHash(realtext)); }
    msg.append("~");
    msg.append(ltrim(in));
    if (stackfuncs.size() > 0) {
         msg.append(stackfuncs.back());
         stackfuncs.pop_back();
         msg.append(to_string(debuglevel));
         debuglevel--;
     }  
     msg.append("|");
     msg.append(to_string(*debugidx));
     msg.append("\033[0m\n");
     printf("%s", msg.data());
 }

 //shows you which entry is unitialized when one of the 
 //items in the json is uninitialized (leading to a segfault)
 //walks json, printing each path before trying to access/print
 //value at that path.
void walkJson(string path, const Value& a) {
    string nextpath;
    if (a.IsObject()) {
        for (Value::ConstMemberIterator itr = a.MemberBegin();
             itr != a.MemberEnd(); ++itr)
            {
                nextpath = path;
                nextpath.append("/");
                nextpath.append(itr->name.GetString());
                printf("Path %s\n", nextpath.data());
                walkJson(nextpath, itr->value);
            }
    } else if (a.IsArray()) {
        int i=0;
        for (Value::ConstValueIterator itr = a.Begin(); 
             itr != a.End(); 
             ++itr) 
            {
                nextpath = path;
                nextpath.append("/");
                nextpath.append(to_string(i));
                printf("Path %s\n", nextpath.data());
                walkJson(nextpath, *itr);
                i++;
            }
    } else if (a.IsInt()) {
        printf("%i\n", a.GetInt());
    } else if (a.IsNull()) {
        printf("null\n");
    } else if (a.IsString()) {
        printf("%s\n", a.GetString());
    }
}

 
#endif
 
