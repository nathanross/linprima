#line 1 "podt.cpp"
#include "podt.hpp"
#include "debug.hpp"

using namespace std;
using namespace rapidjson;
using namespace wojson;


fixedstr::FixedStr lstr(string in) {
    in.insert(wojson::LITERAL_HINT,0);
    return fixedstr::getFixedStr(in);
}

fixedstr::FixedStr fstr(string in) {
    return fixedstr::getFixedStr(in);
}

RegexHalf::RegexHalf() { 
#ifndef THROWABLE
        err = false;
#endif
        start = -1; 
        end = -1; 
}

#ifndef THROWABLE
AssertError::AssertError() {
    description = "";
}
void AssertError::toJson(WojsonMap* out, WojsonDocument* doc,
                         const ExtraStruct *extra) {
    out->moveAssign(text::_message,  lstr(description));
                  
    out->assign(text::_isAssert, true);
}
#endif

ExError::ExError() {
    description = "unknown error";
    index = 0;
    lineNumber = 0;
    column = 0;
}
void ExError::toJson(WojsonMap* out, WojsonDocument* doc,
                     const ExtraStruct *extra) {
    DEBUGIN("Error::toJSON", false);
    out->assign(text::_isError, true);
    out->moveAssign(text::_description, lstr(description));
    out->assign(text::_index, this->index);
    out->assign(text::_lineNumber, this->lineNumber);
    out->assign(text::_column, this->column);
    DEBUGOUT("Error::toJSON", false); 
}
void ExError::toJsonTolerant(WojsonMap* out, WojsonDocument* doc,
                             const ExtraStruct *extra) {
    DEBUGIN("Error::toJSON", false);
    out->moveAssign(text::_description,  lstr(description));
    out->assign(text::_index, this->index);
    out->assign(text::_lineNumber, this->lineNumber);
    out->assign(text::_column, this->column);
    DEBUGOUT("Error::toJSON", false); 
}

Loc::Loc(int lineNumber, int idx, int lineStart) : 
    startLine(lineNumber), startColumn(idx-lineStart),
             endLine(-1), endColumn(-1), hasSource(false),
             source("") {
    DEBUGIN("Loc()", true);
    DEBUGOUT("loc()", true);
}

void Loc::toJson(WojsonMap* out, WojsonDocument* doc) const { 
    //DEBUGIN(" locToJson(Loc l)", false);
    WojsonMap startjson = doc->getMap();
    startjson.assign(text::_line, startLine);
    startjson.assign(text::_column, startColumn);
    out->assignColl(text::_start, &startjson);
    if (this->endLine != -1) {
        WojsonMap endjson = doc->getMap();
        endjson.assign(text::_line, endLine);
        endjson.assign(text::_column, endColumn);
        out->assignColl(text::_end, &endjson);
    }
    if (this->hasSource) {
        out->moveAssign(text::_source,
                    lstr(this->source));
    }
    //DEBUGOUT("locToJson", false); 
}


Comment::Comment(int lineNumber, int idx, int lineStart) :
    type(&(text::_Null)),
    loc(lineNumber, idx, lineStart) {
    //DEBUGIN("Comment()", false);
    this->value = "";
    this->range[0] = -1;
    this->range[1] = -1;
    //DEBUGOUT("Comment()", false);
}

void Comment::toJson(WojsonMap* out, WojsonDocument* doc,
                     const ExtraStruct *extra) {
    DEBUGIN("Comment::toJson", false);
    out->scopedAssign(text::_type, *type);
    out->moveAssign(text::_value, lstr(value));
    if (extra->range) {
        WojsonArr rangearr = doc->getArr();
        rangearr.push(this->range[0]);
        rangearr.push(this->range[1]);
        out->assignColl(text::_range, &rangearr);
    }
    if (extra->loc) {
        WojsonMap locjson = doc->getMap();
        this->loc.toJson(&locjson, doc);
        out->assignColl(text::_loc, &locjson);
    }
    DEBUGOUT("comment::toJson", false);
}

TokenStruct::TokenStruct() :    
    isNull(true), lineNumber(-1), lineStart(-1),
    startLineNumber(-1), startLineStart(-1), start(-1),
    end(-1),
    loc(-1,-1,-1) {
}

TokenStruct::TokenStruct(int lineNumber, int idx, int lineStart) :    
    isNull(false), lineNumber(-1), lineStart(-1),
    startLineNumber(-1), startLineStart(-1), start(-1),
    end(-1),
    loc(lineNumber, idx, lineStart) {
    DEBUGIN("TokenStruct()", true);
#ifndef THROWABLE
    err = false;
#endif
    type = TknType::None;
    octal=false; 
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenStruct()", true);
}

StateStruct::StateStruct() { 
    allowIn = true;
    inFunctionBody = false;
    inIteration = false;
    inSwitch = false;
    lastCommentStart = -1;
    parenthesisCount = 0; //? parse only?
}



OptionsStruct::OptionsStruct() {
    DEBUGIN("OptionsStruct()", true);
    range = false;
    loc = false;
    comment = false;
    tolerant = false;
    attachComment = false;
    tokens = false;
    tokenize = false;
    hasSource = false;
    DEBUGOUT("OptionsStruct()", true);
}
bool OptionsStruct::json_getbool(Value& jsonMap, 
                                 const string key, 
                                 const bool defaultVal) {
    Value::ConstMemberIterator itr = jsonMap.FindMember(key.data());
    if (itr != jsonMap.MemberEnd()) {        
        
        if (itr->value.IsBool()) {
            bool result = itr->value.GetBool();
            return result;
        } 
    }
    return defaultVal;
}
OptionsStruct::OptionsStruct(const char *in_o) {
    DEBUGIN("OptionsStruct(char*)", true);
    Document d;
    d.Parse(in_o);
    tokenize = false;
    if (d.HasParseError()) {
        //#I don't think this will ever come up outside of manual
        //# debugging unless there's some serious encoding error.
        DEBUGIN("failed to parse options string provided", false);
        range = false;
        loc = false;
        comment = false;
        tolerant = false;
        attachComment = false;
        tokens = false;
        hasSource = false;
    } else { 
        range = json_getbool(d, "range", false);
        loc = json_getbool(d, "loc", false);
        attachComment = json_getbool(d, "attachComment", false);
        comment = json_getbool(d, "comment", false);
        tolerant = json_getbool(d, "tolerant", false);
        tokens = json_getbool(d, "tokens", false);
        Value::ConstMemberIterator itr = d.FindMember("source");
        hasSource = (itr != d.MemberEnd() 
                     && itr->value.IsString());
        if (hasSource) {
            source = itr->value.GetString();            
        }
    }
    DEBUGOUT("OptionsStruct(char*)", true);
}




ExtraStruct::ExtraStruct(OptionsStruct opt) {
    tokenize = opt.tokenize;
    tokenTracking = opt.tokens;
    range = opt.range;    
    loc = opt.loc;
    errorTolerant = opt.tolerant;
    commentTracking = opt.comment;
    attachComment = opt.attachComment;
    if (loc && opt.hasSource) {
        hasSource = true;
        source = opt.source;
    } else { hasSource = false; }
    if (attachComment) {
        range = true;
        commentTracking = true;
    }

    this->clear();
    //openParenToken = -1;
    //openCurlyToken = -1;
}
void ExtraStruct::clear() {
    tokenRecords.clear();
    comments.clear();
    errors.clear();
    leadingComments.clear();
    trailingComments.clear();
}


TokenRecord::TokenRecord(Loc locArg) : loc(locArg) {
    DEBUGIN("TokenRecord()", true);
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenRecord()", true);
}
TokenRecord::TokenRecord(int lineNumber, int idx, int lineStart) :
    loc(lineNumber, idx, lineStart) {
    DEBUGIN("TokenRecord()", true);
    range[0] = -1;
    range[1] = -1;
    DEBUGOUT("TokenRecord()", true);
}

void TokenRecord::toJson(WojsonMap* out, WojsonDocument* doc,
                         const ExtraStruct *extra) {
    DEBUGIN(" TokenRecord::toJson", false);    
    out->scopedAssign(text::_type, *(TokenName[this->type]));
    out->moveAssign(text::_value, lstr(valuestring));
    if (extra->range) {
        WojsonArr rangearr = doc->getArr();
        rangearr.push(this->range[0]);
        rangearr.push(this->range[1]);
        out->assignColl(text::_range, &rangearr);
    }
    if (extra->loc) {
        WojsonMap locjson = doc->getMap();
        this->loc.toJson(&locjson, doc);
        out->assignColl(text::_loc, &locjson);
    }
    DEBUGOUT("TokenRecord::toJson()", false);
}
