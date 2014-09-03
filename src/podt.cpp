

#ifndef THROWABLE
AssertError::AssertError() {
    description = "";
}
void AssertError::toJson(const ExtraStruct *extra,
                         Value& out, AllocatorType* alloc) {
    out.AddMember(text::_message, 
                  Value(description.data(),
                        description.length(),
                        *alloc).Move(),
                  *alloc); 
                  
                  
    out.AddMember(text::_isAssert, true, *alloc);
}
#endif

ExError::ExError() {
    description = "unknown error";
    index = 0;
    lineNumber = 0;
    column = 0;
}
void ExError::toJson(const ExtraStruct *extra,
                     Value& out, AllocatorType* alloc) {
    DEBUGIN("Error::toJSON", false);
    out.AddMember(text::_isError, true, *alloc);
    out.AddMember(text::_description, 
                  Value(description.data(),
                        description.length(),
                        *alloc).Move(),
                  *alloc);
    out.AddMember(text::_index, this->index, *alloc);
    out.AddMember(text::_lineNumber, this->lineNumber, *alloc);
    out.AddMember(text::_column, this->column, *alloc);
    DEBUGOUT("Error::toJSON", false); 
}
void ExError::toJsonTolerant(const ExtraStruct *extra,
                             Value& out, AllocatorType* alloc) {
    DEBUGIN("Error::toJSON", false);
    out.AddMember(text::_description, 
                  Value(description.data(),
                        description.length(),
                        *alloc).Move(),
                  *alloc);
    out.AddMember(text::_index, this->index, *alloc);
    out.AddMember(text::_lineNumber, this->lineNumber, *alloc);
    out.AddMember(text::_column, this->column, *alloc);
    DEBUGOUT("Error::toJSON", false); 
}

Loc::Loc(int lineNumber, int idx, int lineStart) : 
    startLine(lineNumber), startColumn(idx-lineStart),
             endLine(-1), endColumn(-1), hasSource(false),
             source("") {
    DEBUGIN("Loc()", true);
    DEBUGOUT("loc()", true);
}

void Loc::toJson(Value& out, AllocatorType* alloc) const { 
    //DEBUGIN(" locToJson(Loc l)", false);
    Value startjson(kObjectType);
    startjson.AddMember(text::_line, startLine, *alloc);
    startjson.AddMember(text::_column, startColumn, *alloc);
    out.AddMember(text::_start, startjson, *alloc);
    if (this->endLine != -1) {        
        Value endjson(kObjectType);
        endjson.AddMember(text::_line, endLine, *alloc);
        endjson.AddMember(text::_column, endColumn, *alloc);
        out.AddMember(text::_end, endjson, *alloc);
    }
    if (this->hasSource) {
        out.AddMember(text::_source,
                      Value(this->source.data(),
                            this->source.length(), 
                            *alloc).Move(),
                      *alloc);
    }
    //DEBUGOUT("locToJson", false); 
}


Comment::Comment(int lineNumber, int idx, int lineStart) :
    loc(lineNumber, idx, lineStart) {
    //DEBUGIN("Comment()", false);
    this->type = (&(text::_Null));
    this->value = "";
    this->range[0] = -1;
    this->range[1] = -1;
    //DEBUGOUT("Comment()", false);
}

void Comment::toJson(const ExtraStruct *extra,
                     Value& out, AllocatorType* alloc) {
    DEBUGIN("Comment::toJson", false);
    out.AddMember(text::_type, *(this->type), *alloc);
    out.AddMember(text::_value, 
                  Value(value.data(),
                        value.length(),
                        *alloc).Move(),
                  *alloc);
    if (extra->range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        out.AddMember(text::_range, rangearr, *alloc);
    }
    if (extra->loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        out.AddMember(text::_loc, locjson, *alloc);
    }
    DEBUGOUT("comment::toJson", false);
}


TokenStruct::TokenStruct(int lineNumber, int idx, int lineStart) :    
    lineNumber(-1), lineStart(-1),
    startLineNumber(-1), startLineStart(-1), start(-1),
    end(-1),
    loc(lineNumber, idx, lineStart) {
    DEBUGIN("TokenStruct()", true);
#ifndef THROWABLE
    err = false;
#endif
    isNull = false;
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

void TokenRecord::toJson(const ExtraStruct *extra,
                         Value& out, AllocatorType* alloc) {
    DEBUGIN(" TokenRecord::toJson", false);    
    Value typev(*(TokenName[this->type]), *alloc); 
    out.AddMember(text::_type, typev.Move(), *alloc);
    out.AddMember(text::_value, 
                      Value(valuestring.data(),
                                valuestring.length(),
                                *alloc).Move(),
                      *alloc);
    if (extra->range) {
        Value rangearr(kArrayType);
        rangearr.PushBack(this->range[0], *alloc);
        rangearr.PushBack(this->range[1], *alloc);
        out.AddMember(text::_range, rangearr, *alloc);
    }
    if (extra->loc) {
        Value locjson(kObjectType);
        this->loc.toJson(locjson, alloc);
        out.AddMember(text::_loc, locjson, *alloc);
    }
    DEBUGOUT("TokenRecord::toJson()", false);
}
