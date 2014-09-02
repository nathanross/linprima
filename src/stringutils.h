#line 2 "stringutils.cpp"
//#include <vector>
#include <map>
//#include <string>
#include <unordered_set>

#define reqinline inline //save tweaking this for last. Talking maybe 10ms system / 20 ms asm in one rep of Chart.js (huge file), so pretty low return on optimization.

#ifndef LIBSTDC
#include <locale>
#include <codecvt>
#endif

#ifndef THROWABLE

//todo use templated class and typedefs.
/*
template <class T> class ErrWrap {
public:
    bool err; T val;
    ErrWrap<T>() {
        err = false;
    }
    ErrWrap<T>(T val) {
        this->val = val;
        err = false;
    }
};
ErrWrap<int> noErr; */


class ErrWrapint {
public:
    bool err;
    int val;
    ErrWrapint() {
        err = false;
    }
    ErrWrapint(int in) {
        val = in;
        err = false;
    }
};
ErrWrapint noErr;

class ErrWrapbool {
public:
    bool err;
    bool val;
    ErrWrapbool() {
        err = false;
    }
    ErrWrapbool(bool in) {
        val = in;
        err = false;
    }
};

class ErrWrapstring {
public:
    bool err;
    string val;
    ErrWrapstring() {
        err = false;
    }
    ErrWrapstring(string in) {
        val = in;
        err = false;
    }
};

class ErrWrapu16string {
public:
    bool err;
    u16string val;
    ErrWrapu16string() {
        err = false;
    }
    ErrWrapu16string(u16string in) {
        val = in;
        err = false;
    }
};



#endif

#ifdef LIBSTDC

//assume everything is ascii.    
string toU8(const u16string& input) { 
    char * outtmp = new char[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char) ((int) input[i]);
    } 
    string out = string(outtmp, input.length());
    free(outtmp);
    return out;
}

string toU8(const char16_t* input) {
    return toU8(u16string(input));
}

u16string toU16string(const string& input){ 
    char16_t * outtmp = new char16_t[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char16_t) ((int) input[i]);
    } 
    u16string out = u16string(outtmp, input.length());
    free(outtmp);
    return out;
}
#endif
#ifndef LIBSTDC

std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu8;

reqinline 
string toU8(const u16string input){ 
    return convu8.to_bytes(input);
}

string toU8(const char16_t* input) {
  return toU8(u16string(input));
}

wstring toWstring(const string input) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(input);
}

reqinline
u16string toU16string(const string input){ 
    //#TODO moving this outside of the func causes a test failure? look into.
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu16;
    return convu16.from_bytes(input);
}
#endif

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
reqinline
bool hasSet(const u16string needle, const unordered_set<u16string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}
reqinline
bool hasSet(const string needle, const unordered_set<string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}

reqinline 
bool has(const u16string needle, const vector<u16string>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const string needle, const vector<string>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const int needle, const vector<int>& haystack) {
    return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
//inline bool has(const int needle, const int haystack[], const int length) {
//    return find(haystack, haystack+length, needle) != haystack+length;
//}

template<typename T> bool hasStringKey(const string needle, const map<string,T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}


//#define slice(a, b, c) u16string( ((const char16_t *) a+b), c-b)
reqinline
u16string slice(const char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
    const char16_t * startptr = arr + start;    
    return u16string(startptr, (end-start));
    }


//#define appendChar(a, b) a.append(u16string({b}))
reqinline
void appendChar(u16string &base, char16_t tail) { 
    base.append(u16string({tail})); 
    //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.

    }

int parseInt(u16string in_u16, int radix) {  // !!!
    const int zero = 48;
    const int upperA = 65;
    const int lowerA = 97;

    string in = toU8(in_u16);
    int length,
        cur,
        result = 0;
    length = in.length();
    for (int i=0; i<length; i++) {
        cur = (int) in[i];
        if (cur <= zero+9 &&  cur >= zero) {
            cur = cur - zero;
        } else if (cur <= upperA+5 && cur >= upperA) {
            cur = 10 + cur - upperA;
        } else if (cur <= lowerA+5 && cur >= lowerA) {
            cur = 10 + cur - lowerA;
        } else {
            return -1;
        }
        result += (cur * pow(radix,length-(i+1)));
    }
    return result; 
}

double sciNoteToDouble(string in) {
    DEBUGIN("sciNoteToDouble", false);
    char current;
    string factor = "0";
    string exp = "0";
    bool esignPassed = false; //, dotPassed = false;
    for (unsigned int i=0;i<in.length();i++) {
        current = in[i];
        if (current == u'.') {
            //dotPassed = true;
            factor.append(string({current}));
        } else if (current == u'E' || current == u'e' ) {
            esignPassed = true; 
        } else if (esignPassed) {
            if (exp == "0") { exp = ""; }
            exp.append(string({current}));
        } else {
            factor.append(string({current}));               
        }
    }

    DEBUGOUT("sciNoteToDouble", false);
    return stod(factor) * pow(10,stod(exp));
}
