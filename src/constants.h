
TokenStruct NULLTOKEN(-1,-1,-1);
ptrTkn NULLPTRTKN;
Node * NULLNODE = 0x0;
const char * emptystr = "";
const StrRef EMPTY_STRREF = StringRef(emptystr, 0);

map<string, unique_ptr<Node>> PlaceHolders { 
};
 //map<string, int> 

unique_ptr<Node> make_unique_Node() {
    unique_ptr<Node> a(new Node(false, false, 0x0, 0x0, 0x0));
    return a;
}

void initglobals() {
    DEBUGIN(" initglobals()", true);    
    PlaceHolders["ArrowParameterPlaceHolder"] = make_unique_Node(); 
    PlaceHolders["ArrowParameterPlaceHolder"]->type = (&(text::_ArrowParameterPlaceholder));
    NULLTOKEN.isNull = true;
    NULLPTRTKN = make_shared<TokenStruct>(-1,-1,-1);
    (*NULLPTRTKN).isNull = true;

    DEBUGOUT("", true);
}
