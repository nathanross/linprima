#line 2 "debug.cpp"
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;
using namespace rapidjson;


int debuglevel = 1;
vector<string> stackfuncs;
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

string encodeObjId(size_t in) {
    string out;
    size_t intmp = in;
    char next;
    while (intmp > 0) {
        next = intmp % 62;
        if (next < 10) { next = '0' + next; }
        else if(next <36) { next = 'A' + next - 10; }
        else { next = 'a' + next - 36; }
        out.insert(out.begin(), next); //not most efficient,
                             //but most encodes won't be 
                             //more than 4 chars or so.
        intmp = intmp / 62;
    }
    //marker could be any arbitrary sequence. just chose this one.
    string outwithmarker = string("#`@$");
    outwithmarker.append(out);
    return outwithmarker;
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


#ifdef DO_DEBUG
#define DEBUGIN(A,B) DEBUG_IN(A,B)
#define DEBUGOUT(A,B) DEBUG_OUT(A,B)
#define DBGRET(A,B) DBG_RET(A,B)

int *debugidx;

bool DEBUG_ON= (bool) 1;

bool HIPRI_ONLY= (bool) 1;

string colorHash(string text) {
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

void DEBUG_IN(string in, bool lowprio) {    
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

void DEBUG_OUT(string in, bool lowprio) {
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

template<typename T> T DBG_RET(string a, T b) { 
    DEBUG_OUT(a, false); 
    return b; 
}

#endif
#ifndef DO_DEBUG
#define DEBUGIN(A,B) 
#define DEBUGOUT(A,B)
#define DBGRET(A,B) B 

#endif
