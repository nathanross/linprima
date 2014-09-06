#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#line 4 "stringutils.hpp"
#include "debug.hpp"
#include "t52types.hpp"
#include <vector>
#include <map>
#include <string>
#include <unordered_set>
#include <math.h>
#include <algorithm>

#define reqinline inline //save tweaking this for last. Talking maybe 10ms system / 20 ms asm in one rep of Chart.js (huge file), so pretty low return on optimization.

#ifndef LIBSTDC
#include <locale>
#include <codecvt>
#endif

#ifdef LIBSTDC

//assume everything is ascii.    
std::string toU8(const std::u16string& input) { 
    char * outtmp = new char[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char) ((int) input[i]);
    } 
    std::string out = std::string(outtmp, input.length());
    free(outtmp);
    return out;
}

std::string toU8(const char16_t* input) {
    return toU8(std::u16string(input));
}

std::u16string toU16string(const std::string& input){ 
    char16_t * outtmp = new char16_t[input.length()+1];
    for (unsigned int i=0; i<input.length(); i++) {
        outtmp[i] = (char16_t) ((int) input[i]);
    } 
    std::u16string out = std::u16string(outtmp, input.length());
    free(outtmp);
    return out;
}
#endif
#ifndef LIBSTDC

std::string toU8(const std::u16string input);

reqinline
std::string toU8(const char16_t* input) {
    return toU8(std::u16string(input));
}

reqinline
std::wstring toWstring(const std::string input) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>> myconv;
  return myconv.from_bytes(input);
}

reqinline
std::u16string toU16string(const std::string input){ 
    //#TODO moving this outside of the func causes a test failure? look into.
    std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu16;
    return convu16.from_bytes(input);
}
#endif

//non-parallel functions
// example: has<int>(3, {1,2,3,4,5}) // would return true
reqinline
bool hasSet(const std::u16string needle, 
            const std::unordered_set<std::u16string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}
reqinline
bool hasSet(const std::string needle, 
            const std::unordered_set<std::string>& haystack) {
    return (haystack.find(needle) != haystack.end());
}

reqinline 
bool has(const std::u16string needle, 
         const std::vector<std::u16string>& haystack) {
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const std::string needle, 
         const std::vector<std::string>& haystack) {
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
reqinline 
bool has(const int needle, const std::vector<int>& haystack) {
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
//inline bool has(const int needle, const int haystack[], const int length) {
//    return find(haystack, haystack+length, needle) != haystack+length;
//}

template<typename T> bool hasStringKey(const std::string needle, 
                                       const std::map<std::string,T> haystack) {
    auto result = haystack.find(needle);
    return (result != haystack.end());
}


//#define slice(a, b, c) u16string( ((const char16_t *) a+b), c-b)
reqinline
std::u16string slice(const char16_t *arr, int start, int end) {
    //start inclusive, end exclusive, just like js
    const char16_t * startptr = arr + start;    
    return std::u16string(startptr, (end-start));
}


//#define appendChar(a, b) a.append(u16string({b}))
reqinline
void appendChar(std::u16string &base, char16_t tail) { 
    base.append(std::u16string({tail})); 
    //? switch to u16stringstream? but there's nothing like that
    // on SO someone said append only handles certain input types right,
    //not sure if that's true for u16string.
}

reqinline
int parseInt(std::u16string in_u16, int radix) {  // !!!
    const int zero = 48;
    const int upperA = 65;
    const int lowerA = 97;

    std::string in = toU8(in_u16);
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

reqinline
double sciNoteToDouble(std::string in) {
    DEBUGIN("sciNoteToDouble", false);
    char current;
    std::string factor = "0";
    std::string exp = "0";
    bool esignPassed = false; //, dotPassed = false;
    for (unsigned int i=0;i<in.length();i++) {
        current = in[i];
        if (current == u'.') {
            //dotPassed = true;
            factor.append(std::string({current}));
        } else if (current == u'E' || current == u'e' ) {
            esignPassed = true; 
        } else if (esignPassed) {
            if (exp == "0") { exp = ""; }
            exp.append(std::string({current}));
        } else {
            factor.append(std::string({current}));               
        }
    }

    DEBUGOUT("sciNoteToDouble", false);
    return stod(factor) * pow(10,stod(exp));
}
#endif
