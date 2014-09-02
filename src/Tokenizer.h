#line 1 "Tokenizer.h"

class Tokenizer {
public:
    Tokenizer(u16string code, OptionsStruct options);
    Tokenizer(shared_ptr<LinprimaTask> task);
    static bool isIdentifierName(const TknType type);
    static bool isIdentifierStart(const char16_t ch);
    static bool isIdentifierPart(const char16_t ch);
    static bool isFutureReservedWord(const string id);
    static bool isStrictModeReservedWord(const string id);
    static bool isRestrictedWord(const string id);
    bool isKeyword(const string id);

    //#throw_begin
    void skipComment();
    ptrTkn scanRegExp();
    ptrTkn collectRegex();
    ptrTkn lex();
    void peek();
    void readTokens(vector<TokenRecord> &tokens);
    //#throw_end
    void tokenize(Document& out, 
#ifdef LIMITJSON
                  vector<string> &completedObjectsOut,
#endif
                  const bool retErrAsJson);
    void filterTokenLocation();
    ptrTkn makeToken();
private:
    shared_ptr<LinprimaTask> task;
    const char16_t * sourceRaw;
    const int length;

    int& idx;
    int& lineNumber;
    int& lineStart;
    ExtraStruct &extra;
    StateStruct &state;
    ptrTkn &lookahead;

    char16_t source(long pos);


    static bool intervalarr_contains(unsigned int key, vector< vector< unsigned int > > * arr);

    void addComment(const StrRef * type, const string& value, 
                const int start, const int end, const Loc& loc);
    int skipSingleLineComment(int idxtmp, const int offset);
    //#throw_begin
    int skipMultiLineComment(int idxtmp);
    //#throw_end
    char16_t scanHexEscape(const char16_t prefix);
    //#throw_begin
    u16string scanUnicodeCodePointEscape();
    string getEscapedIdentifier();
    string getIdentifier();
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
