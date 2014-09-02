#line 1 "Tokenizer.cpp"

const char16_t NULL_CHAR16 = u'N';
    vector< vector <unsigned int> > nonasciiIdentifierstart = { {170,181,186,192,216,248,710,736,748,750,880,886,887,890,895,902,904,908,910,931,1015,1162,1329,1369,1377,1488,1520,1568,1646,1647,1649,1749,1765,1766,1774,1775,1786,1791,1808,1810,1869,1969,1994,2036,2037,2042,2048,2074,2084,2088,2112,2208,2308,2365,2384,2392,2417,2437,2447,2448,2451,2474,2482,2486,2493,2510,2524,2525,2527,2544,2545,2565,2575,2576,2579,2602,2610,2611,2613,2614,2616,2617,2649,2654,2674,2693,2703,2707,2730,2738,2739,2741,2749,2768,2784,2785,2821,2831,2832,2835,2858,2866,2867,2869,2877,2908,2909,2911,2929,2947,2949,2958,2962,2969,2970,2972,2974,2975,2979,2980,2984,2990,3024,3077,3086,3090,3114,3133,3160,3161,3168,3169,3205,3214,3218,3242,3253,3261,3294,3296,3297,3313,3314,3333,3342,3346,3389,3406,3424,3425,3450,3461,3482,3507,3517,3520,3585,3634,3635,3648,3713,3714,3716,3719,3720,3722,3725,3732,3737,3745,3749,3751,3754,3755,3757,3762,3763,3773,3776,3782,3804,3840,3904,3913,3976,4096,4159,4176,4186,4193,4197,4198,4206,4213,4238,4256,4295,4301,4304,4348,4682,4688,4696,4698,4704,4746,4752,4786,4792,4800,4802,4808,4824,4882,4888,4992,5024,5121,5743,5761,5792,5870,5888,5902,5920,5952,5984,5998,6016,6103,6108,6176,6272,6314,6320,6400,6480,6512,6528,6593,6656,6688,6823,6917,6981,7043,7086,7087,7098,7168,7245,7258,7401,7406,7413,7414,7424,7680,7960,7968,8008,8016,8025,8027,8029,8031,8064,8118,8126,8130,8134,8144,8150,8160,8178,8182,8305,8319,8336,8450,8455,8458,8469,8473,8484,8486,8488,8490,8495,8508,8517,8526,8544,11264,11312,11360,11499,11506,11507,11520,11559,11565,11568,11631,11648,11680,11688,11696,11704,11712,11720,11728,11736,11823,12293,12321,12337,12344,12353,12445,12449,12540,12549,12593,12704,12784,13312,19968,40960,42192,42240,42512,42538,42539,42560,42623,42656,42775,42786,42891,42896,42928,42929,42999,43011,43015,43020,43072,43138,43250,43259,43274,43312,43360,43396,43471,43488,43494,43514,43520,43584,43588,43616,43642,43646,43697,43701,43702,43705,43712,43714,43739,43744,43762,43777,43785,43793,43808,43816,43824,43868,43876,43877,43968,44032,55216,55243,63744,64112,64256,64275,64285,64287,64298,64312,64318,64320,64321,64323,64324,64326,64467,64848,64914,65008,65136,65142,65313,65345,65382,65474,65482,65490,65498}, {170,181,186,214,246,705,721,740,748,750,884,886,887,893,895,902,906,908,929,1013,1153,1327,1366,1369,1415,1514,1522,1610,1646,1647,1747,1749,1765,1766,1774,1775,1788,1791,1808,1839,1957,1969,2026,2036,2037,2042,2069,2074,2084,2088,2136,2226,2361,2365,2384,2401,2432,2444,2447,2448,2472,2480,2482,2489,2493,2510,2524,2525,2529,2544,2545,2570,2575,2576,2600,2608,2610,2611,2613,2614,2616,2617,2652,2654,2676,2701,2705,2728,2736,2738,2739,2745,2749,2768,2784,2785,2828,2831,2832,2856,2864,2866,2867,2873,2877,2908,2909,2913,2929,2947,2954,2960,2965,2969,2970,2972,2974,2975,2979,2980,2986,3001,3024,3084,3088,3112,3129,3133,3160,3161,3168,3169,3212,3216,3240,3251,3257,3261,3294,3296,3297,3313,3314,3340,3344,3386,3389,3406,3424,3425,3455,3478,3505,3515,3517,3526,3632,3634,3635,3654,3713,3714,3716,3719,3720,3722,3725,3735,3743,3747,3749,3751,3754,3755,3760,3762,3763,3773,3780,3782,3807,3840,3911,3948,3980,4138,4159,4181,4189,4193,4197,4198,4208,4225,4238,4293,4295,4301,4346,4680,4685,4694,4696,4701,4744,4749,4784,4789,4798,4800,4805,4822,4880,4885,4954,5007,5108,5740,5759,5786,5866,5880,5900,5905,5937,5969,5996,6000,6067,6103,6108,6263,6312,6314,6389,6430,6509,6516,6571,6599,6678,6740,6823,6963,6987,7072,7086,7087,7141,7203,7247,7293,7404,7409,7413,7414,7615,7957,7965,8005,8013,8023,8025,8027,8029,8061,8116,8124,8126,8132,8140,8147,8155,8172,8180,8188,8305,8319,8348,8450,8455,8467,8469,8477,8484,8486,8488,8493,8505,8511,8521,8526,8584,11310,11358,11492,11502,11506,11507,11557,11559,11565,11623,11631,11670,11686,11694,11702,11710,11718,11726,11734,11742,11823,12295,12329,12341,12348,12438,12447,12538,12543,12589,12686,12730,12799,19893,40908,42124,42237,42508,42527,42538,42539,42606,42653,42735,42783,42888,42894,42925,42928,42929,43009,43013,43018,43042,43123,43187,43255,43259,43301,43334,43388,43442,43471,43492,43503,43518,43560,43586,43595,43638,43642,43695,43697,43701,43702,43709,43712,43714,43741,43754,43764,43782,43790,43798,43814,43822,43866,43871,43876,43877,44002,55203,55238,55291,64109,64217,64262,64279,64285,64296,64310,64316,64318,64320,64321,64323,64324,64433,64829,64911,64967,65019,65140,65276,65338,65370,65470,65479,65487,65495,65500} };
vector< vector < unsigned int> > nonasciiIdentifierpart = { {170,181,186,192,216,248,710,736,748,750,768,886,887,890,895,902,904,908,910,931,1015,1155,1162,1329,1369,1377,1425,1471,1473,1474,1476,1477,1479,1488,1520,1552,1568,1646,1749,1759,1770,1791,1808,1869,1984,2042,2048,2112,2208,2276,2406,2417,2437,2447,2448,2451,2474,2482,2486,2492,2503,2504,2507,2519,2524,2525,2527,2534,2561,2565,2575,2576,2579,2602,2610,2611,2613,2614,2616,2617,2620,2622,2631,2632,2635,2641,2649,2654,2662,2689,2693,2703,2707,2730,2738,2739,2741,2748,2759,2763,2768,2784,2790,2817,2821,2831,2832,2835,2858,2866,2867,2869,2876,2887,2888,2891,2902,2903,2908,2909,2911,2918,2929,2946,2947,2949,2958,2962,2969,2970,2972,2974,2975,2979,2980,2984,2990,3006,3014,3018,3024,3031,3046,3072,3077,3086,3090,3114,3133,3142,3146,3157,3158,3160,3161,3168,3174,3201,3205,3214,3218,3242,3253,3260,3270,3274,3285,3286,3294,3296,3302,3313,3314,3329,3333,3342,3346,3389,3398,3402,3415,3424,3430,3450,3458,3459,3461,3482,3507,3517,3520,3530,3535,3542,3544,3558,3570,3571,3585,3648,3664,3713,3714,3716,3719,3720,3722,3725,3732,3737,3745,3749,3751,3754,3755,3757,3771,3776,3782,3784,3792,3804,3840,3864,3865,3872,3893,3895,3897,3902,3913,3953,3974,3993,4038,4096,4176,4256,4295,4301,4304,4348,4682,4688,4696,4698,4704,4746,4752,4786,4792,4800,4802,4808,4824,4882,4888,4957,4992,5024,5121,5743,5761,5792,5870,5888,5902,5920,5952,5984,5998,6002,6003,6016,6103,6108,6109,6112,6155,6160,6176,6272,6320,6400,6432,6448,6470,6512,6528,6576,6608,6656,6688,6752,6783,6800,6823,6832,6912,6992,7019,7040,7168,7232,7245,7376,7380,7416,7417,7424,7676,7960,7968,8008,8016,8025,8027,8029,8031,8064,8118,8126,8130,8134,8144,8150,8160,8178,8182,8204,8205,8255,8256,8276,8305,8319,8336,8400,8417,8421,8450,8455,8458,8469,8473,8484,8486,8488,8490,8495,8508,8517,8526,8544,11264,11312,11360,11499,11520,11559,11565,11568,11631,11647,11680,11688,11696,11704,11712,11720,11728,11736,11744,11823,12293,12321,12337,12344,12353,12441,12442,12445,12449,12540,12549,12593,12704,12784,13312,19968,40960,42192,42240,42512,42560,42612,42623,42655,42775,42786,42891,42896,42928,42929,42999,43072,43136,43216,43232,43259,43264,43312,43360,43392,43471,43488,43520,43584,43600,43616,43642,43739,43744,43762,43777,43785,43793,43808,43816,43824,43868,43876,43877,43968,44012,44013,44016,44032,55216,55243,63744,64112,64256,64275,64285,64298,64312,64318,64320,64321,64323,64324,64326,64467,64848,64914,65008,65024,65056,65075,65076,65101,65136,65142,65296,65313,65343,65345,65382,65474,65482,65490,65498}, {170,181,186,214,246,705,721,740,748,750,884,886,887,893,895,902,906,908,929,1013,1153,1159,1327,1366,1369,1415,1469,1471,1473,1474,1476,1477,1479,1514,1522,1562,1641,1747,1756,1768,1788,1791,1866,1969,2037,2042,2093,2139,2226,2403,2415,2435,2444,2447,2448,2472,2480,2482,2489,2500,2503,2504,2510,2519,2524,2525,2531,2545,2563,2570,2575,2576,2600,2608,2610,2611,2613,2614,2616,2617,2620,2626,2631,2632,2637,2641,2652,2654,2677,2691,2701,2705,2728,2736,2738,2739,2745,2757,2761,2765,2768,2787,2799,2819,2828,2831,2832,2856,2864,2866,2867,2873,2884,2887,2888,2893,2902,2903,2908,2909,2915,2927,2929,2946,2947,2954,2960,2965,2969,2970,2972,2974,2975,2979,2980,2986,3001,3010,3016,3021,3024,3031,3055,3075,3084,3088,3112,3129,3140,3144,3149,3157,3158,3160,3161,3171,3183,3203,3212,3216,3240,3251,3257,3268,3272,3277,3285,3286,3294,3299,3311,3313,3314,3331,3340,3344,3386,3396,3400,3406,3415,3427,3439,3455,3458,3459,3478,3505,3515,3517,3526,3530,3540,3542,3551,3567,3570,3571,3642,3662,3673,3713,3714,3716,3719,3720,3722,3725,3735,3743,3747,3749,3751,3754,3755,3769,3773,3780,3782,3789,3801,3807,3840,3864,3865,3881,3893,3895,3897,3911,3948,3972,3991,4028,4038,4169,4253,4293,4295,4301,4346,4680,4685,4694,4696,4701,4744,4749,4784,4789,4798,4800,4805,4822,4880,4885,4954,4959,5007,5108,5740,5759,5786,5866,5880,5900,5908,5940,5971,5996,6000,6002,6003,6099,6103,6108,6109,6121,6157,6169,6263,6314,6389,6430,6443,6459,6509,6516,6571,6601,6617,6683,6750,6780,6793,6809,6823,6845,6987,7001,7027,7155,7223,7241,7293,7378,7414,7416,7417,7669,7957,7965,8005,8013,8023,8025,8027,8029,8061,8116,8124,8126,8132,8140,8147,8155,8172,8180,8188,8204,8205,8255,8256,8276,8305,8319,8348,8412,8417,8432,8450,8455,8467,8469,8477,8484,8486,8488,8493,8505,8511,8521,8526,8584,11310,11358,11492,11507,11557,11559,11565,11623,11631,11670,11686,11694,11702,11710,11718,11726,11734,11742,11775,11823,12295,12335,12341,12348,12438,12441,12442,12447,12538,12543,12589,12686,12730,12799,19893,40908,42124,42237,42508,42539,42607,42621,42653,42737,42783,42888,42894,42925,42928,42929,43047,43123,43204,43225,43255,43259,43309,43347,43388,43456,43481,43518,43574,43597,43609,43638,43714,43741,43759,43766,43782,43790,43798,43814,43822,43866,43871,43876,43877,44010,44012,44013,44025,55203,55238,55291,64109,64217,64262,64279,64296,64310,64316,64318,64320,64321,64323,64324,64433,64829,64911,64967,65019,65039,65069,65075,65076,65103,65140,65276,65305,65338,65343,65370,65470,65479,65487,65495,65500} };


vector< string > FnExprTokens = {
    // A function following one of those tokens is an expression.
    "(", "{", "[", "in", "typeof", "instanceof", "new",
    "return", "case", "delete", "throw", "void",
    // assignment operators                                      
    "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", ">>>=",
    "&=", "|=", "^=", ",",
    // binary/unary operators            
    "+", "-", "*", "/", "%", "++", "--", "<<", ">>", ">>>", "&",
    "|", "^", "!", "~", "&&", "||", "?", ":", "===", "==", ">=",
    "<=", "<", ">", "!=", "!=="
};

Tokenizer::Tokenizer(const u16string code,
                     OptionsStruct options) :
    task(make_shared<LinprimaTask>(code,
                                       //code.data(), 
                                       //code.length(), 
                                   options)),
    //    task(make_shared_linprima(code.data(), code.length(), options)),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead) {


}

Tokenizer::Tokenizer(shared_ptr<LinprimaTask> taskArg) :
         task(taskArg),
         sourceRaw(task->sourceRaw),
         length(task->length),
         idx(task->idx),
         lineNumber(task->lineNumber),
         lineStart(task->lineStart),
         extra(task->extra),
         state(task->state),
         lookahead(task->lookahead) {
    
}

ptrTkn Tokenizer::makeToken() {
    shared_ptr<TokenStruct> tmp (new TokenStruct(
                                                 lineNumber, 
                                                 idx, 
                                                 lineStart));
    return tmp;
}


// 7.6 Identifier Names and Identifiers
bool Tokenizer::intervalarr_contains(unsigned int key, vector< vector< unsigned int > > * arr) {
    //DEBUGIN("   intervalarr_contains ", false);
    vector< unsigned int > *range_starts = &(arr->at(0)),
        *range_ends = &(arr->at(1));
    unsigned int pos = lower_bound(range_starts->begin(), 
                          range_starts->end(), key) - range_starts->begin();
    if (range_starts->at(pos) > key) { 
        if (pos == 0) { return false; } pos--; 
    }
    return (key <= range_ends->at(pos) 
                   || (pos+1 < range_starts->size() 
                       && key == range_starts->at(pos+1)));
}

reqinline
bool Tokenizer::isIdentifierStart(const char16_t ch) {
    DEBUGIN("   isIdentifierStart(const char16_t ch)", false);
    DEBUGOUT("", false); 
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
        (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
        (ch >= 0x61 && ch <= 0x7A) ||         // a..z
        (ch == 0x5C) ||                      // \ (backslash)
        ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                              &nonasciiIdentifierstart));
} 

reqinline
bool Tokenizer::isIdentifierPart(const char16_t ch) {
     DEBUGIN("   isIdentifierPart(const char16_t ch)", false);
     DEBUGOUT("", false); 
     return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch >= 0x30 && ch <= 0x39) ||         // 0..9
         (ch == 0x5C) ||                      // \ (backslash)
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                               &nonasciiIdentifierpart));
 }

// 7.6.1.2 Future Reserved Words
reqinline
bool Tokenizer::isFutureReservedWord(const string id) {
     DEBUGIN("   isFutureReservedWord(const u16string id)", false);
     DEBUGOUT("", false); 
     return has(id, { //
             "class",
             "enum",
             "export",
             "extends",
             "import",
             "super"
             });
 }

reqinline
bool Tokenizer::isStrictModeReservedWord(const string id) {
     DEBUGIN("   isStrictModeReservedWord(const u16string id)", false);
     DEBUGOUT("", false); 
     return has(id, { 
             "implements",
             "interface",
             "package",
             "private",
             "protected",
             "public",
             "static",
             "yield",
             "let"
             });
 }

reqinline
bool Tokenizer::isRestrictedWord(const string id) {
 DEBUGIN("   isRestrictedWord(const u16string id)", false);
 DEBUGOUT("", false);
 return (id == "eval" || id == "arguments");
}

const vector<string> KEYWORDSET = 
    {"if", "in", "do", "var", "for", "new", "try", "let",
     "this", "else", "case", "void", "with", "enum",
     "while", "break", "catch", "throw", 
     "const", "yield", "class", "super",
     "return", "typeof", "delete",
     "switch", "export", "import", "default",
     "finally", "extends", "function", "continue", "debugger",
     "instanceof"};
 
// 7.6.1.1 Keywords
reqinline
bool Tokenizer::isKeyword(const string id) {
     DEBUGIN("   isKeyword(const u16string id)", false);
     if (task->strict && isStrictModeReservedWord(id)) { 
         DEBUGOUT("", false); 
         return true;
     }

     DEBUGOUT("",false);
     // 'const' is specialized as Keyword in V8.
     // 'yield' and 'let' are for compatiblity with SpiderMonkey and ES.next.
     // Some others are from future reserved words.
     return has(id, KEYWORDSET);

}

bool Tokenizer::isIdentifierName(const TknType tkntype) {
    DEBUGIN("   isIdentifierName(TokenStruct token)", false);
    DEBUGOUT("", false); 
    return tkntype == TknType::Identifier ||
        tkntype == TknType::Keyword ||
        tkntype == TknType::BooleanLiteral ||
        tkntype == TknType::NullLiteral;
}
char16_t Tokenizer::source(long pos) { return *(sourceRaw + pos); }

//# only called if extra.commentTracking
void Tokenizer::addComment(const StrRef * type, const string& value, 
                 const int start, const int end, const Loc& loc) {
    DEBUGIN(" addComment(u16string type, u16string value, int start, int end, Loc loc)", false);
    Comment comment(lineNumber, idx, lineStart);

     //assert(typeof start === 'number', 'Comment must have valid position');

     // Because the way the actual token is scanned, often the comments
     // (if any) are skipped twice during the lexical analysis.
     // Thus, we need to skip adding a comment if the comment array already
     // handled it.
     if (state.lastCommentStart >= start) {
         DEBUGOUT("", false); 
         return;
     }
     state.lastCommentStart = start;

     comment.type = type;
     comment.value = value; 
     if (extra.range) {
         comment.range[0] = start;
         comment.range[1] = end;
     }
     if (extra.loc) {
         comment.loc = loc;
     }
     extra.comments.push_back(comment);
     if (extra.attachComment) {
         extra.leadingComments.push_back(comment);
         extra.trailingComments.push_back(comment);
     }
     DEBUGOUT("", false);
}

int Tokenizer::skipSingleLineComment(int idxtmp, const int offset) {
    DEBUGIN(" skipSingleLineComment(const int offset)", false);
    int start;

    char16_t ch;
    string comment;

    Loc loc(lineNumber, idxtmp, lineStart);

    start = idxtmp - offset;
    loc.startColumn -= offset;

    while (idxtmp < length) {
        ch = source(idxtmp);
        ++idxtmp;
        switch (ch) {
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            if (extra.commentTracking) {
                comment = toU8(slice(sourceRaw, start + offset, idxtmp-1));
                loc.endLine = lineNumber;
                loc.endColumn = idxtmp - lineStart - 1;
                addComment((&(text::_Line)), comment, start, idxtmp - 1, loc);
            }
            if (ch == 13 && source(idxtmp) == 10) {
                ++idxtmp;
            }
            ++lineNumber;
            lineStart = idxtmp;
            DEBUGOUT("", false); 
            return idxtmp;
        default:
            break;
        }
    } 

    if (extra.commentTracking) {
        comment = toU8(slice(sourceRaw, start + offset, idxtmp)); 
        loc.endLine = lineNumber;
        loc.endColumn = idxtmp - lineStart;
        addComment((&(text::_Line)), comment, start, idxtmp, loc);
    }
    DEBUGOUT("", false);
    return idxtmp;
}

inline //only called once.
//#throw_
int Tokenizer::skipMultiLineComment(int idxtmp) {
    DEBUGIN(" skipMultiLineComment()", false);
    int start=0; //satisfy g++ warning about potential unitialized use
    Loc loc(lineNumber, idxtmp, lineStart);
    char16_t ch;
    string comment;

    if (extra.commentTracking) {
        start = idxtmp - 2;
        loc.startColumn -= 2 ;
    }

    while (idxtmp < length) {
        ch = source(idxtmp);
        switch (ch) {
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            if (ch == 0x0D && source(idxtmp + 1) == 0x0A) {
                ++idxtmp;
            }
            ++lineNumber;
            ++idxtmp;
            lineStart = idxtmp;
            if (idxtmp >= length) {
                idx = idxtmp;
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            break;
        case 0x2A:
            // Block comment ends with ''.
            if (source(idxtmp + 1) == 0x2F) {
                ++idxtmp;
                ++idxtmp;
                if (extra.commentTracking) {
                    comment = toU8(slice(sourceRaw, start + 2, idxtmp - 2));
                    loc.endLine = lineNumber;
                    loc.endColumn = idxtmp - lineStart;
                    addComment((&(text::_Block)), comment, start, idxtmp, loc);
                }
                DEBUGOUT("", false); 
                idx = idxtmp; //#throw52 error idx.
                return idxtmp;
            }
            ++idxtmp;
            break;
        default:
            ++idxtmp;
            break;
        }
    }

    idx = idxtmp;
    task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    DEBUGOUT("", false);
    return -1; //#throw52
}

//throw_
void Tokenizer::skipComment() {
    DEBUGIN(" skipComment()", false);
    char16_t ch;
    bool start;
    start = (idx == 0);
    while (idx < length) {
        ch = source(idx);

        //alternative to switch we've explored,
        //make an array of all whitespace and other
        //cases below, if not present in array, exit,
        //and move all whitespace cases to the default:
        //clause. We're talking maybe a ms of difference though.
        //both approaches work relatively very well.
        //if (! binary_search(MAYBE_COMMENT, 
        //               MAYBE_COMMENT+29,
        //              ch)) { return; }

        switch(ch) {
        case 0x20: 
        case 0x09: 
        case 0x0B: 
        case 0x0C: 
        case 0xA0: 
        case 0xFEFF: 
        case 0x1680: 
        case 0x180E: 
        case 0x2000: 
        case 0x2001: 
        case 0x2002: 
        case 0x2003: 
        case 0x2004: 
        case 0x2005: 
        case 0x2006: 
        case 0x2007: 
        case 0x2008: 
        case 0x2009: 
        case 0x200A:
        case 0x202F: 
        case 0x205F: 
        case 0x3000: 
            ++idx;
            break;
        case 0x0A: //line terminators;
        case 0x0D:
        case 0x2028:
        case 0x2029:
            ++idx;
            if (ch == 0x0D && source(idx) == 0x0A) {
                ++idx;
            }
            ++lineNumber;
            lineStart = idx;
            start = true;
            break;
        case 0x2F: // U+002F is '/'
            ch = source(idx + 1);
            if (ch == 0x2F) {
                ++idx;
                ++idx;
                idx = skipSingleLineComment(idx, 2);
                start = true;
            } else if (ch == 0x2A) {  // U+002A is '*'
                ++idx;
                ++idx;
                idx = skipMultiLineComment(idx); 
            } else {
                DEBUGOUT("", false);
                return;
            }
            break;
        case 0x2D: // U+002D is '-'
            // U+003E is '>'
            if (start 
                && (source(idx + 1) == 0x2D) 
                && (source(idx + 2) == 0x3E)) {
                // '-->' is a single-line comment
                idx += 3;
                idx = skipSingleLineComment(idx, 3);
            } else {
                DEBUGOUT("", false);
                return;
            }
            break;
        case 0x3C: // U+003C is '<'
            if (slice(sourceRaw, idx + 1, idx + 4) == u"!--") {
                ++idx; // `<`
                ++idx; // `!`
                ++idx; // `-`
                ++idx; // `-`
                idx = skipSingleLineComment(idx, 4);
            } else {
                DEBUGOUT("", false);
                return; //whitespace
            }    
            break;        
        default:
            DEBUGOUT("", false);
            return;
        }
    }
    DEBUGOUT("", false);
    return; //throw52; 
}
char16_t Tokenizer::scanHexEscape(const char16_t prefix) {
    DEBUGIN("scanHexEscape(const char16_t prefix) {", false);
    int i, len;
    char16_t ch;
    int code = 0;

    len = (prefix == u'u') ? 4 : 2;
    for (i = 0; i < len; ++i) {
        if (idx < length && isHexDigit(source(idx))) {
            ch = source(idx++);
            code = code * 16 + u16string({u"0123456789abcdef"})
                .find_first_of(toLowercaseHex(ch));
        } else {
            DEBUGOUT("scanHexEscape", false); 
            return NULL_CHAR16; 
        }
    }
    DEBUGOUT("scanHexEscape", false); 
    return code;
}

//throw_
u16string Tokenizer::scanUnicodeCodePointEscape() {
    DEBUGIN("scanUnicodeCodePointEscape", false);
    char16_t ch;
    int code;
    char16_t cu[2];

    ch = source(idx);
    code = 0;

    // At least, one hex digit is required.
    if (ch == u'}') {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    while (idx < length) {
        ch = source(idx++);
        if (!isHexDigit(ch)) {
            break;
        }
        code = code * 16 + u16string({u"0123456789abcdef"})
            .find_first_of(toLowercaseHex(ch));
    }

    if (code > 0x10FFFF || ch != u'}') {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    // UTF-16 Encoding
    if (code <= 0xFFFF) {
        DEBUGOUT("", false);
        return u16string({(char16_t) code});
    }

    cu[0] = ((code - 0x10000) >> 10) + 0xD800; 
    cu[1] = ((code - 0x10000) & 1023) + 0xDC00;
    DEBUGOUT("scanUnicodeCodePointEscape", false); 
    return u16string({cu[0], cu[1]});
}

//throw_
string Tokenizer::getEscapedIdentifier() {
    DEBUGIN("getEscapedIdentifier", false);
    char16_t ch;
    u16string id;

    ch = source(idx++);
    id = u16string({ch});

    // '\u' (U+005C, U+0075) denotes an escaped character.
    if (ch == 0x5C) {
        if (source(idx) != 0x75) {
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
        ++idx;
        ch = scanHexEscape(u'u');
        if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierStart(ch)) { 
            task->throwError(NULLPTRTKN, 
                       Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
        id = u16string({ch});
    }

    while (idx < length) {
        ch = source(idx);
        if (!isIdentifierPart(ch)) {
            break;
        }
        ++idx;
        appendChar(id, ch);

        // '\u' (U+005C, U+0075) denotes an escaped character.
        if (ch == 0x5C) {
            id = id.substr(0, id.length() - 1);
            if (source(idx) != 0x75) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            ++idx;
            ch = scanHexEscape(u'u');
            if (ch == NULL_CHAR16 || ch == u'\\' || !isIdentifierPart(ch)) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
            }
            appendChar(id, ch);
        }
    }

    DEBUGOUT("getEscapedIdentifier", false); 
    return toU8(id);
}

//throw_
string Tokenizer::getIdentifier() {
    DEBUGIN("getIdentifier()", false);
    int start;
    char16_t ch;

    start = idx++;
    while (idx < length) {
        ch = source(idx);
        if (ch == 0x5C) {
            // Blackslash (U+005C) marks Unicode escape sequence.
            idx = start;
            return DBGRET("", getEscapedIdentifier());
        }
        // this is a hotpath (e.g. about 10th-15th in # calls)
        // force-inlining isIdentifierPart here and 
        // removing the 2x jumps per loop
        // saves about 95% of its cost.
        if ((ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch >= 0x30 && ch <= 0x39) ||         // 0..9
         ((ch >= 0x80) && intervalarr_contains((unsigned int) ch, 
                                    &nonasciiIdentifierpart))) {
            ++idx;
        } else {
            break;
        }
    }

    return DBGRET("getIdentifier", toU8(slice(sourceRaw, start, idx))); 
}

//throw_
ptrTkn Tokenizer::scanIdentifier() {
    DEBUGIN(" scanIdentifier()", false);
    ptrTkn t = makeToken();
    int start;
    TknType type;
    string id; 

    start = idx;

    // Backslash (U+005C) starts an escaped character.
    //#ternary operator interferes with throw52
    if (source(idx) == 0x5C) {
        id = getEscapedIdentifier();
    } else {
        id = getIdentifier();
    }
    // There is no keyword or literal with only one character.
    // Thus, it must be an identifier.
    if (id.length() == 1) {
        type = TknType::Identifier;
    } else if (isKeyword(id)) {
        type = TknType::Keyword;
    } else if (id == "null") {
        type = TknType::NullLiteral;
    } else if (id == "true" || id == "false") {
        type = TknType::BooleanLiteral;
    } else {
        type = TknType::Identifier;
    }

    t->type = type;
    t->strvalue = id;
    t->literaltype = LiteralType["String"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanIdentifier", false);
    return t;
}

u16string emccu16str;


 // 7.7 Punctuators
 //throw_
ptrTkn Tokenizer::scanPunctuator() {
    DEBUGIN(" scanPunctuator()", false);

    ptrTkn t = makeToken();
    int start = idx;

    char16_t ch1 = source(idx);
    string ch2, ch3, ch4;
    char16_t code[2];

    code[0] = source(idx);

    t->type = TknType::Punctuator;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->literaltype = LiteralType["String"];

    switch (code[0]) {
        // Check for most common single-character punctuators.
    case 0x2E:  // . dot
    case 0x28:  // ( open bracket
    case 0x29:  // ) close bracket
    case 0x3B:  // ; semicolon
    case 0x2C:  // , comma
    case 0x7B:  // { open curly brace
    case 0x7D:  // } close curly brace
    case 0x5B:  // [
    case 0x5D:  // ]
    case 0x3A:  // :
    case 0x3F:  // ?
    case 0x7E:  // ~
        ++idx;
        if (extra.tokenize) {
            if (code[0] == 0x28) {
                extra.openParenToken = extra.tokenRecords.size();
            } else if (code[0] == 0x7B) {
                extra.openCurlyToken = extra.tokenRecords.size();
            }
        }
        t->strvalue = toU8(u16string({ code[0] }));

        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    default:
        code[1] = source(idx + 1);

        // '=' (U+003D) marks an assignment or comparison operator.
        if (code[1] == 0x3D) {
            switch (code[0]) {
            case 0x2B:  // +
            case 0x2D:  // -
            case 0x2F:  // /
            case 0x3C:  // <
            case 0x3E:  // >
            case 0x5E:  // ^
            case 0x7C:  // |
            case 0x25:  // %
            case 0x26:  // &
            case 0x2A:  // *
                idx += 2;
                t->strvalue = toU8(u16string({code[0], code[1]}));      
                t->end = idx;
                DEBUGOUT("", false); 
                return t;
            case 0x21: // !
            case 0x3D: // =
                idx += 2;

                // !== and ===
                if (source(idx) == 0x3D) {
                    ++idx;
                }
                t->strvalue = toU8(slice(sourceRaw, start, idx));
                t->end = idx;
                DEBUGOUT("", false); 
                return t;
            }
        }
    } 


    // 4-character punctuator: >>>=

    ch4 = toU8(slice(sourceRaw, idx, idx+4)); 

    if (ch4 == ">>>=") {
        idx += 4;
        t->strvalue = ch4;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 3-character punctuators: === !== >>> <<= >>=

    ch3 = ch4.substr(0, 3); 

    if (ch3 == ">>>" || ch3 == "<<=" || ch3 == ">>=") {
        idx += 3;
        t->strvalue = ch3;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // Other 2-character punctuators: ++ -- << >> && ||
    ch2 = ch3.substr(0, 2); 

    if (((ch1 == (char16_t) ch2[1]) 
         && (u16string({u"+-<>&|"}).find_first_of(ch1)
             != std::string::npos))
        || ch2 == "=>") {
        idx += 2;
        t->strvalue = ch2;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    // 1-character punctuators: < > = ! + - * % & | ^ /

    if (u16string({u"<>=!+-*%&|^/"}) //? is it necessary/correct to have this as ({u""}) and not just (u"")
        .find_first_of(ch1) != std::string::npos) {
        ++idx;
        t->strvalue = toU8(u16string({ch1}));
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});

    DEBUGOUT("", false); 
    return t;
  //# return avoids compile warnings bcos clang doesn't look into task->throwError.
}
     // 7.8.3 Numeric Literals
//throw_
ptrTkn Tokenizer::scanHexLiteral(const int start) {
    DEBUGIN(" scanHexLiteral(const int start)", false);
    u16string number;
    ptrTkn t = makeToken();

    while (idx < length) {
        if (!isHexDigit(source(idx))) {
            break;
        }
        appendChar(number, source(idx++));
    }

    if (number.length() == 0) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    if (isIdentifierStart(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::NumericLiteral;
    t->intvalue = parseInt(number, 16);
    t->literaltype = LiteralType["Int"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanHexLiteral", false);
    return t;
}

//throw_
ptrTkn Tokenizer::scanOctalLiteral(const int start) {
    DEBUGIN(" scanOctalLiteral(const int start)", false);
    u16string number = u"0";

    ptrTkn t = makeToken();
    appendChar(number, source(idx++));

    while (idx < length) {
        if (!isOctalDigit(source(idx))) {
            break;
        }
        appendChar(number, source(idx++));
    }

    if (isIdentifierStart(source(idx)) || isDecimalDigit(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::NumericLiteral;
    t->intvalue = parseInt(number, 8);
    t->literaltype = LiteralType["Int"];
    t->octal = true;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanOctalLiteral", false);
    return t;
}

//#throw_
ptrTkn Tokenizer::scanNumericLiteral() {
    DEBUGIN(" scanNumericLiteral()", false);
    int start;
    char16_t ch;
    ptrTkn t = makeToken();
    u16string number;


    ch = source(idx);
    task->softAssert(isDecimalDigit(ch) || (ch == u'.'),
              "Numeric literal must start with a decimal digit or a decimal point");

    start = idx;
    number = u"";
    if (ch != u'.') {
        appendChar(number, source(idx++));
        ch = source(idx);

        // Hex number starts with '0x'.
        // Octal number starts with '0'.
        if (number == u"0") {
            if (ch == u'x' || ch == u'X') {
                ++idx;
                return DBGRET("scanNumericLiteral", scanHexLiteral(start));
            }
            if (isOctalDigit(ch)) {
                return DBGRET("scanNumericLiteral", scanOctalLiteral(start));
            }
            // decimal number starts with '0' such as '09' is illegal.
            if (ch && isDecimalDigit(ch)) {
                task->throwError(NULLPTRTKN, 
                           Messages[Mssg::UnexpectedToken], {"ILLEGAL"}); 
            }
        }

        while (isDecimalDigit(source(idx))) {
            appendChar(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'.') {
        //#JSON can't parse decimal numbers without
        //#a number preceding the decimal.
        if (start == idx) { appendChar(number, u'0'); }

        appendChar(number, source(idx++));
        while (isDecimalDigit(source(idx))) {
            //if (source(idx) != u'0') { hasDot = true; } //# js auto-casts dbls of negligible epsilon-to-int to int
            appendChar(number, source(idx++));
        }
        ch = source(idx);
    }

    if (ch == u'e' || ch == u'E') {
        appendChar(number, source(idx++));
        ch = source(idx);
        if (ch == u'+' || ch == u'-') {
            appendChar(number, source(idx++));
        }
        if (isDecimalDigit(source(idx))) {
            while (isDecimalDigit(source(idx))) {
                appendChar(number, source(idx++));
            }
        } else {
            task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
        }
    }

    if (isIdentifierStart(source(idx))) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }
    t->type = TknType::NumericLiteral;
    t->strvalue = toU8(number);
    //t->dblvalue = //# want to allow browsers to use full allowance of whatever their local max int size is.
    //               //#which is same as it would work in esprima.
    //(hasSciNote)?
    //sciNoteToDouble(toU8string(number)) : stod(toU8string(number));
    t->literaltype = LiteralType["Double"];
    t->lineNumber = lineNumber;
    t->lineStart = lineStart;
    t->start = start;
    t->end = idx;

    DEBUGOUT("scanNumericLiteral", false);
    return t; 
}


// 7.8.4 String Literals
//throw_
ptrTkn Tokenizer::scanStringLiteral() {
    DEBUGIN(" scanStringLiteral()", false);

    ptrTkn t = makeToken();
    bool octal = false;
    u16string str;
    int start, restore, code;
    char16_t quote, ch, unescaped;
    int startLineNumber = lineNumber;
    int startLineStart = lineStart;

    quote = source(idx);
    task->softAssert((quote == u'\'' || quote == u'"'),
               "String literal must starts with a quote");

    start = idx;
    ++idx;

    while (idx < length) {
        ch = source(idx++);
        if (ch == quote) {
            quote = NULL_CHAR16;
            break;
        } else if (ch == u'\\') {
            ch = source(idx++);
            if (!ch || !isLineTerminator(ch)) { // what does !ch mean in this context
                if (ch == u'u' || ch == u'x') {
                    if (source(idx) == u'{') {
                        ++idx;
#ifndef THROWABLE
                        u16string tmp = scanUnicodeCodePointEscape();
                        str.append(tmp);
#endif
#ifdef THROWABLE
                        str.append(scanUnicodeCodePointEscape());
#endif
                    } else {
                        restore = idx;
                        unescaped = scanHexEscape(ch);
                        if (unescaped != NULL_CHAR16) {
                            appendChar(str, unescaped);
                        } else {
                            idx = restore;
                            appendChar(str, ch);
                        }
                    }    
                } else if (ch == u'n') {
                    appendChar(str, u'\n');
                } else if (ch == u'r') {
                    appendChar(str, u'\r');
                } else if (ch == u't') {
                    appendChar(str, u'\t');
                } else if (ch == u'b') {
                    appendChar(str, u'\b');
                } else if (ch == u'f') {
                    appendChar(str, u'\f');
                } else if (ch == u'v') {
                    appendChar(str, u'\x0B');
                } else {
                    if (isOctalDigit(ch)) {
                        code = u16string({u"01234567"}).find_first_of(ch);

                        // \0 is not octal escape sequence
                        if (code != 0) {
                            octal = true;
                        }

                        if (idx < length && isOctalDigit(source(idx))) {
                            octal = true;
                            code = code * 8 + 
                                u16string({u"01234567"})
                                .find_first_of(source(idx++));

                            // 3 digits are only allowed when string starts
                            // with 0, 1, 2, 3
                            if (u16string({u"0123"})
                                .find_first_of(ch) != std::string::npos &&
                                idx < length &&
                                isOctalDigit(source(idx))) {
                                code = code * 8 + u16string({u"01234567"})
                                    .find_first_of(source(idx++));
                            }
                        }
                        appendChar(str, (char16_t) code);
                    } else {
                        str += ch;
                    }
                }
            } else {
                ++lineNumber;
                if (ch == u'\r' && source(idx) == u'\n') {
                    ++idx;
                }
                lineStart = idx;
            }
        } else if (isLineTerminator(ch)) {
            break;
        } else {
            appendChar(str, ch);
        }
    }

    if (quote != NULL_CHAR16) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnexpectedToken], {"ILLEGAL"});
    }

    t->type = TknType::StringLiteral;
    t->strvalue = toU8(str);
    t->literaltype = LiteralType["String"];            
    t->octal = octal;
    t->startLineNumber = startLineNumber;
    t->startLineStart = startLineStart;
    t->lineNumber = lineNumber;
    t->lineStart = lineStart,
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanStringLiteral", false);
    return t;
}

//throw_
RegexHalf Tokenizer::scanRegExpBody() {
    DEBUGIN("scanRegExpBody()", false);
    char16_t ch;
    u16string str = u"", body;
    bool classMarker, terminated;
    RegexHalf rh;

    ch = source(idx);
    task->softAssert(ch == u'/',
               "Regular expression literal must start with a slash");
    appendChar(str, source(idx++)); 

    classMarker = false;
    terminated = false;
    while (idx < length) {
        ch = source(idx++);
        appendChar(str, ch);
        if (ch == u'\\') {
            ch = source(idx++);
            // ECMA-262 7.8.5
            if (isLineTerminator(ch)) {
                task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
            }
            appendChar(str, ch);
        } else if (isLineTerminator(ch)) {
            task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
        } else if (classMarker) {
            if (ch == u']') {
                classMarker = false;
            }
        } else {
            if (ch == u'/') {
                terminated = true;
                break;
            } else if (ch == u'[') {
                classMarker = true;
            }
        }
    }

    if (!terminated) {
        task->throwError(NULLPTRTKN, Messages[Mssg::UnterminatedRegExp], {});
    }

    // Exclude leading and trailing slash.
    body = str.substr(1, str.length() - 2);
    rh.value = toU8(body);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpBody", false);
    return rh;
}

//throw_
RegexHalf Tokenizer::scanRegExpFlags() {
    DEBUGIN("scanRegExpFlags()", false);
    char16_t ch;
    u16string str, flags;
    int restore;
    RegexHalf rh;

    str = u"";
    flags = u"";
    while (idx < length) {
        ch = source(idx);
        if (!isIdentifierPart(ch)) {
            break;
        }

        ++idx;
        if (ch == u'\\' && idx < length) {
            ch = source(idx);
            if (ch == u'u') {
                ++idx;
                restore = idx;
                ch = scanHexEscape(u'u');
                if (ch) {
                    appendChar(flags, ch);
                    for (str.append(u16string({u'\\', u'u'})); 
                         restore < idx; ++restore) {                         
                        appendChar(str, source(restore));
                    }
                } else {
                    idx = restore;
                    appendChar(flags, u'u');
                    str.append(u"\\u");
                }

                task->throwErrorTolerant(NULLPTRTKN, 
                                   Messages[Mssg::UnexpectedToken], 
                                   {"ILLEGAL"});
            } else {
                appendChar(str, u'\\');
                task->throwErrorTolerant(NULLPTRTKN, 
                                   Messages[Mssg::UnexpectedToken],
                                   {"ILLEGAL"});
            }
        } else {
            appendChar(flags, ch);
            appendChar(str, ch);
        }
    }

    rh.value = toU8(flags);
    rh.literal = toU8(str);
    DEBUGOUT("scanRegExpFlags", false);
    return rh;
}

//throw_
ptrTkn Tokenizer::scanRegExp() {
    DEBUGIN(" scanRegExp()", false);
    int start;
    RegexHalf body; 
    RegexHalf flags; 
    ptrTkn t = makeToken();

    //? value is int? to think on. 
    //testRegExp is I think supposed to normally return a regex object.

    lookahead = makeToken();
    lookahead->isNull = true;
    skipComment(); 
    start = idx;

    body = scanRegExpBody(); //eregexhalf
    flags = scanRegExpFlags(); //eregexhalf

    //value = testRegExp(body.value, flags.value);

    if (extra.tokenize) {
        t->type = TknType::RegularExpression;
        t->strvalue = "regexDummy"; //?
        t->lineNumber = lineNumber;
        t->lineStart = lineStart;
        t->start = start;
        t->end = idx;
        DEBUGOUT("", false); 
        return t; //not polymorphic right now. not going to work... :!
    }

    t->literal = body.literal; 
    t->literal.append(flags.literal);
    t->literaltype = LiteralType["Regexp"];
    t->strvalue = body.value;
    t->flags = flags.value;
    t->start = start;
    t->end = idx;
    DEBUGOUT("scanRegExp", false);
    return t;
}

//throw_
ptrTkn Tokenizer::collectRegex() {
    DEBUGIN(" collectRegex()", false);
    int pos;

    ptrTkn regex;
    string tokval;

    skipComment();
    Loc loc(lineNumber, idx, lineStart);

    pos = idx;

    regex = scanRegExp(); //etkns
    loc.endLine = lineNumber;
    loc.endColumn = idx - lineStart;

    if (!extra.tokenize) {
        TokenRecord tr(loc);
        // Pop the previous token, which is likely '/' or '/='
        if (extra.tokenRecords.size() > 0) {
            const TokenRecord * token = 
                &(extra.tokenRecords[extra.tokenRecords.size() - 1]);
            if (token->range[0] == pos 
                && token->type == TknType::Punctuator) {
                
                tokval = token->valuestring; 
                if (tokval == "/" || tokval == "/=") {
                    extra.tokenRecords.pop_back();
                }
            }
        }

        tr.type = TknType::RegularExpression;
        tr.valuestring = regex->literal;
        tr.range[0] = pos;
        tr.range[1] = idx;
        tr.loc = loc;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectRegex", false);
    return regex;
}

//throw_
ptrTkn Tokenizer::advanceSlash() {
    DEBUGIN(" advanceSlash()", false);
    //# only gets called if extra.tokenize == true

    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    if (extra.tokenRecords.size() == 0) {
        // Nothing before that: it cannot be a division.
        return DBGRET("advSlash1", collectRegex()); 
    }    
    const TokenRecord *prevToken = 
        &(extra.tokenRecords[extra.tokenRecords.size() - 1]);

    if (prevToken->type == TknType::Punctuator) { 
        if (prevToken->valuestring == "]") { 
            return DBGRET("advSlash2", scanPunctuator());
        }
        const TokenRecord *checkToken;
        if (prevToken->valuestring == ")") {
            //checkToken && 
            //# instead of checking for existence, we add
            //# the openParenToken value check to the condition above.
            //# remember exta.tokens() is already size > 0 bcos 
            //# check at top of func.

            if (extra.openParenToken > 0
                && extra.tokenRecords.size() > (extra.openParenToken - 1)) { 
                checkToken = 
                    &(extra.tokenRecords[extra.openParenToken - 1]);
                if (checkToken->type == TknType::Keyword && 
                    has(checkToken->valuestring, 
                        {"if", "while", "for", "with"})) {
                    return DBGRET("advSlash3", collectRegex()); 
                }
            }
            return DBGRET("advSlash4", scanPunctuator());
        }
        if (prevToken->valuestring == "}") {
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            if (extra.openCurlyToken >= 3 &&
                extra.tokenRecords.size() > (extra.openCurlyToken -3) &&
                extra.tokenRecords[extra.openCurlyToken - 3].type 
                == TknType::Keyword) { 
                // Anonymous function.

                if (extra.openCurlyToken > 3
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 4)) {
                    checkToken = 
                        &(extra.tokenRecords[extra.openCurlyToken -4]);
                } else { 
                    return DBGRET("advSlash5", scanPunctuator());
                }
            } else if (extra.openCurlyToken >= 4 
                       && extra.tokenRecords.size()
                       > (extra.openCurlyToken -4) 
                       && extra.tokenRecords[extra.openCurlyToken - 4].type
                       == TknType::Keyword) {
                // again previously had checked type against string in this cond.
                // Named function.
                if (extra.openCurlyToken > 4
                    && extra.tokenRecords.size() > 
                    (extra.openCurlyToken - 5)) {
                    checkToken = 
                        &(extra.tokenRecords[extra.openCurlyToken -5]);
                } else { 
                    return DBGRET("advSlash6", collectRegex());
                }
            } else {
                return DBGRET("advSlash7", scanPunctuator());
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (has(checkToken->valuestring, FnExprTokens)) {
                // It is an expression.
                return DBGRET("advSlash8", scanPunctuator());
            }
            // It is a declaration.
            return DBGRET("advSlash9", collectRegex()); 
        }
        return DBGRET("advSlash10", collectRegex());
    }
    if (prevToken->type == TknType::Keyword) { 
        return DBGRET("advSlash11", collectRegex()); 
    }
    return DBGRET("advSlash12", scanPunctuator());
}

//throw_
ptrTkn Tokenizer::advance() {
    DEBUGIN(" advance()", false);
    char16_t ch;

    skipComment(); 

    if (idx >= length) {
        ptrTkn t = makeToken();
        t->type = TknType::EOFF;
        t->lineNumber = lineNumber;
        t->lineStart = lineStart;
        t->start = idx;
        t->end = idx;
        DEBUGOUT("", false); 
        return t;
    }

    ch = source(idx);
    if (isIdentifierStart(ch)) {
        return DBGRET("adv", scanIdentifier());
    }

    // Very common: ( and ) and ;
    if (ch == 0x28 || ch == 0x29 || ch == 0x3B) {
        return DBGRET("adv", scanPunctuator());
    }

    // String literal starts with single quote (U+0027) or double quote (U+0022).
    if (ch == 0x27 || ch == 0x22) {
        return DBGRET("adv", scanStringLiteral());
    }

    // Dot (.) U+002E can also start a floating-point number, hence the need
    // to check the next character.
    if (ch == 0x2E) {
        if (isDecimalDigit(source(idx + 1))) {
            return DBGRET("adv", scanNumericLiteral());
        }
        return DBGRET("adv", scanPunctuator());
    }

    if (isDecimalDigit(ch)) {
        return DBGRET("adv", scanNumericLiteral());
    }

    // Slash (/) U+002F can also start a regex.
    if (extra.tokenize && ch == 0x2F) {
        return DBGRET("adv", advanceSlash());
    }

    return DBGRET("adv", scanPunctuator());
}

//throw_
ptrTkn Tokenizer::collectToken() {
    DEBUGIN(" collectToken()", false);
    ptrTkn token;
    u16string value;

    skipComment(); //ev
    Loc loc(lineNumber, idx, lineStart);

    token = advance(); //etkns
    loc.endLine = lineNumber;
    loc.endColumn = idx - lineStart;

    if (token->type != TknType::EOFF) { 
        //this didn't check against string. is fine.
        TokenRecord tr(loc);
        tr.valuestring = toU8(slice(sourceRaw, 
                                    token->start, 
                                    token->end));
        tr.type = token->type;
        tr.range[0] = token->start;
        tr.range[1] = token->end;
        extra.tokenRecords.push_back(tr);
    }

    DEBUGOUT("collectToken", false);
    return token;
}

//throw_
ptrTkn Tokenizer::lex() {
    DEBUGIN(" lex()", false);
    ptrTkn token;

    token = lookahead;
    idx = token->end;
    lineNumber = token->lineNumber;
    lineStart = token->lineStart;

    //#ternary operators not supported by throw52
    if (extra.tokenTracking) {
        lookahead = collectToken();
    } else { 
        lookahead = advance(); 
    }

    idx = token->end;
    lineNumber = token->lineNumber;
    lineStart = token->lineStart;

    DEBUGOUT("lex", false);
    return token;
}

//throw_
void Tokenizer::peek() {
    DEBUGIN(" peek()", false);
    int pos, line, start;
    pos = idx;
    line = lineNumber;
    start = lineStart;

    //#ternary operators not supported by throw52
    if (extra.tokenTracking) {
        lookahead = collectToken();
    } else { 
        lookahead = advance(); 
    }

    idx = pos;
    lineNumber = line;
    lineStart = start;
    DEBUGOUT("peek", false);
    return; //#throw52
}

//# tokenize
//# returns a map containing under
//# (optional) 'comments' - list of comments as per esprima.
//# (optional) 'errors' - list of errors as per esprima
//# 'tokenlist' - list of tokens as per esprima

//# differences between this and esprima tokenize results:
//# -1. json hierarchy in esprima the tokenlist is the noderoot
//# -2. no js regex validation unless passed through a js environment 
//#    afterwards for validation with a tool like linprima-wrapfuncs.js


void Tokenizer::tokenize(Document &outJson, 
#ifdef LIMITJSON
                         vector<string> &completeObjectsOut,
#endif
                         const bool retErrorsAsJson) {
#ifdef LIMITJSON
    task->completeObjects = &completeObjectsOut;
#endif
    outJson.SetObject();
    AllocatorType& alloc = outJson.GetAllocator();
    extra.tokenize = true;    
    vector<TokenRecord> tokens;

    //range, loc, comment->CommentTracking and tolerant->errorTolerant
    //are the relevant/used options.

#ifndef THROWABLE
    ErrWrapint tmp = peek();
    if (tmp.err) {
        if (!extra.errorTolerant) {
            //json_object_put(outJson);
            if (task->errorType == 0) {
                task->retError.toJson(&extra, outJson, &alloc);
                return;
            }
            task->retAssertError.toJson(&extra, outJson, &alloc);
            return;
        }
    }
#endif
#ifdef THROWABLE
    peek();
#endif

    if (lookahead->type == TknType::EOFF) {
        vec2jsonCallback<TokenRecord>(outJson, &alloc, &extra,
                                      text::_tokens,
                                      extra.tokenRecords, 
                                      &TokenRecord::toJson);
         return;
    }

    lex();
    while (lookahead->type != TknType::EOFF) {
#ifndef THROWABLE
        ErrWrapptrTkn out = lex();
        if (out.err) { 
            if (extra.errorTolerant) {
                extra.errors.push_back(task->retError); 
                break;
            } else {
                if (task->errorType == 0) {
                    task->retError.toJson(&extra, outJson, &alloc);
                    return;
                }
                task->retAssertError.toJson(&extra, outJson, &alloc);
                return;
            }
        }
#endif
#ifdef THROWABLE
        try {
            lex();
        } catch (ExError& e) { 
            if (extra.errorTolerant) {
                extra.errors.push_back(e); 
                break;
            } else {
                if (retErrorsAsJson) {
                    //json_object_put(outJson);
                    e.toJson(&extra, outJson, &alloc);
                    return; 
                }
                throw e;
            }
        }
#endif
    }

    filterTokenLocation();
    vec2jsonCallback<TokenRecord>(outJson, &alloc, &extra,
                                  text::_tokens,
                                  extra.tokenRecords, 
                                  &TokenRecord::toJson); 
    if (extra.commentTracking) {
        vec2jsonCallback<Comment>(outJson, &alloc, &extra,
                                  text::_comments,
                                  extra.comments,
                                  &Comment::toJson);
    }
    if (extra.errorTolerant) {
        vec2jsonCallback<ExError>(outJson, &alloc, &extra,
                                  text::_errors,
                                  extra.errors,
                                  &ExError::toJsonTolerant);
    }
    extra.clear();
    return;
}
