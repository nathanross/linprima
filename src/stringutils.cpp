#line 1 "stringutils.cpp"
#include "stringutils.hpp"

std::wstring_convert< std::codecvt_utf8_utf16<char16_t>, char16_t> convu8;


std::string toU8(const std::u16string input){ 
    return convu8.to_bytes(input);
}
