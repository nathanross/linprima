#ifndef PARSEPOD_HPP
#define PARSEPOD_HPP

#line 1 "parsepod.hpp"
#include "strref.hpp"
#include "Node.hpp"
#include "t52types.hpp"
#include <unordered_set>
#include <vector>
#include <string>

struct ParseParamsOptions {
    std::vector< Node* > params;
    int defaultCount;
    std::vector< Node* > defaults;
    ptrTkn firstRestricted;
    ptrTkn stricted;
    std::unordered_set<std::string> paramSet;
    std::string message;
    ParseParamsOptions();
};

struct ParseParamsOut {
#ifndef THROWABLE
    bool err;
#endif
    ptrTkn firstRestricted;
    ptrTkn stricted;
    std::string message;
    std::vector< Node* > params;
    std::vector< Node* > defaults;
    ParseParamsOut();
};

struct ReinterpretOptions {
    std::vector< Node* > params;
    int defaultCount;
    std::vector< Node* > defaults;
    Node *firstRestricted;
    Node *stricted;
    std::unordered_set<std::string> paramSet;
    std::string message;
    ReinterpretOptions();
};

struct ReinterpretOut {
    Node *firstRestricted;
    Node *stricted;
    bool isNull;
    std::string message;
    std::vector< Node* > params;
    std::vector< Node* > defaults;
#ifndef THROWABLE
    bool err;
#endif
    void* rest; //seems to be a dummy var?
    ReinterpretOut();
};

#endif
