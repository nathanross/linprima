#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#line 4 "Tokenizer.hpp"
#include "LinprimaTask.hpp"
#include "podt.hpp"
#include "strref.hpp"
#include "t52types.hpp"
#include <string>
#include <memory>
#include <rapidjson/document.h>


class Tokenizer {
public:
    static ptrTkn NULLPTRTKN;
    Tokenizer(std::u16string code, OptionsStruct options);
    Tokenizer(std::shared_ptr<LinprimaTask> task);
    static bool isIdentifierName(const TknType type);
    static bool isIdentifierStart(const char16_t ch);
    static bool isIdentifierPart(const char16_t ch);
    static bool isFutureReservedWord(const std::string id);
    static bool isStrictModeReservedWord(const std::string id);
    static bool isRestrictedWord(const std::string id);
    bool isKeyword(const std::string id);

    //#throw_begin
    void skipComment();
    ptrTkn scanRegExp();
    ptrTkn collectRegex();
    ptrTkn lex();
    void peek();
    void readTokens(std::vector<TokenRecord> &tokens);
    //#throw_end
    void tokenize(rapidjson::Document& out, 
#ifdef LIMITJSON
                  std::vector<std::string> &completedObjectsOut,
#endif
                  const bool retErrAsJson);
    void filterTokenLocation();
    ptrTkn makeToken();
private:
    std::shared_ptr<LinprimaTask> task;
    const char16_t * sourceRaw;
    const int length;

    int& idx;
    int& lineNumber;
    int& lineStart;
    ExtraStruct &extra;
    StateStruct &state;
    ptrTkn &lookahead;

    char16_t source(long pos);


    static bool intervalarr_contains(unsigned int key, 
                                     std::vector< std::vector< unsigned int > > * arr);

    void addComment(const StrRef * type, const std::string& value, 
                const int start, const int end, const Loc& loc);
    int skipSingleLineComment(int idxtmp, const int offset);
    //#throw_begin
    int skipMultiLineComment(int idxtmp);
    //#throw_end
    char16_t scanHexEscape(const char16_t prefix);
    //#throw_begin
    std::u16string scanUnicodeCodePointEscape();
    std::string getEscapedIdentifier();
    std::string getIdentifier();
    ptrTkn scanIdentifier();
    ptrTkn scanPunctuator();
    ptrTkn scanHexLiteral(const int start);
    ptrTkn scanOctalLiteral(const int start);
    ptrTkn scanNumericLiteral();
    ptrTkn scanStringLiteral();
    RegexHalf scanRegExpBody();
    RegexHalf scanRegExpFlags();
    ptrTkn advanceSlash();
    ptrTkn advance();
    ptrTkn collectToken();
    //#throw_end
};
#endif
