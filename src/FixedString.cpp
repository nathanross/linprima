#include "FixedString.hpp"

namespace fixedstring {
    JsonObj::JsonObj : vector<FixedString>() {
        this->push_back(LEFTBRACKET);
    }
    JsonObj::movePush(FixedString f){
        this->push_back(f);
        this->toDelete->push_back(f);
    }
    JsonObj::addMember(FixedString key, FixedString value) {
        this->push_back(SEMICOLON);
        this->push_back()
    }
    FixedString JsonObj::toString() {
    }
    JsonObj::~JsonObj() {
    }
    
}
