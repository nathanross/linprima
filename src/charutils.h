reqinline
bool isDecimalDigit(const char16_t& ch) {
    DEBUGIN("   isDecimalDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch >= 0x30 && ch <= 0x39); //0..9
}

reqinline
bool isHexDigit(const char16_t& ch) {
    DEBUGIN("   isHexDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (u16string({u"0123456789abcdefABCDEF"}).find_first_of(ch) 
           != std::string::npos);    
}

reqinline
bool isOctalDigit(const char16_t& ch) {
    DEBUGIN("   isOctalDigit(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (u16string({u"01234567"}).find_first_of(ch) 
            != std::string::npos);    
}

char16_t toLowercaseHex(const char16_t& ch) { //used in scanHexEscape
    //assumes isHexDigit(ch) evals to true
    u16string hexletters = u"abcdefABCDEF";
    int pos = hexletters.find_first_of(ch);
    char16_t out = ch;
    if (pos != std::string::npos && pos >= 6) {
        pos = pos - 6;
        out = hexletters[pos];
    }
    return out;
}

//7.2 White Space
reqinline
bool isWhiteSpace(const char16_t& ch) {
    DEBUGIN("   isWhiteSpace(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch == 0x20) || (ch == 0x09) || (ch == 0x0B) || (ch == 0x0C) || (ch == 0xA0) || ch==0xFEFF || (ch >= 0x1680 && ch <= 0x3000 
                             && has(ch, {0x1680, 0x180E, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x202F, 0x205F, 0x3000, 0xFEFF}));
}

// 7.3 Line Terminators
reqinline
bool isLineTerminator(const char16_t& ch) {
    DEBUGIN("   isLineTerminator(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch == 0x0A) || (ch == 0x0D) || (ch == 0x2028) || (ch == 0x2029);
}
