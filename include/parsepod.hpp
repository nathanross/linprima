#ifndef PARSEPOD_HPP
#define PARSEPOD_HPP


struct ParseParamsOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    ptrTkn firstRestricted;
    ptrTkn stricted;
    unordered_set<string> paramSet;
    string message;
    ParseParamsOptions();
};

ParseParamsOptions::ParseParamsOptions() {
    firstRestricted = 0x0; 
    stricted = 0x0; 
}



struct ParseParamsOut {
#ifndef THROWABLE
    bool err;
#endif
    ptrTkn firstRestricted;
    ptrTkn stricted;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
    ParseParamsOut();
};

ParseParamsOut::ParseParamsOut() {
#ifndef THROWABLE
    err = false;
#endif
    message="";
    firstRestricted = 0x0; 
    stricted = 0x0; 
}


struct ReinterpretOptions {
    vector< Node* > params;
    int defaultCount;
    vector< Node* > defaults;
    Node *firstRestricted;
    Node *stricted;
    unordered_set<string> paramSet;
    string message;
    ReinterpretOptions();
};

ReinterpretOptions::ReinterpretOptions() {
    firstRestricted = NULLNODE; //?
    stricted = NULLNODE; //? nec.? had it only in reinOut before.
    //? not sure if context will make it different.
}

struct ReinterpretOut {
    Node *firstRestricted;
    Node *stricted;
    bool isNull;
    string message;
    vector< Node* > params;
    vector< Node* > defaults;
#ifndef THROWABLE
    bool err;
#endif
    void* rest; //seems to be a dummy var?
    ReinterpretOut();
};
ReinterpretOut::ReinterpretOut() {
#ifndef THROWABLE
    err = false;
#endif
    isNull=false;
    firstRestricted = NULLNODE;
    stricted = NULLNODE;
}
#endif
