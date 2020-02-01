#include <string>

#ifndef CODE_GENERATION_VARIABLE_TYPE
#define CODE_GENERATION_VARIABLE_TYPE

struct VariableType {
    std::string _name;
    void* _scope;
    
    // 自定义类型的开放程度： 0 全部可见, 1 相邻可见, 2 仅拥有者可见
    int _openness;

    VariableType(const std::string& name, void* scope, int openness) 
        : _name(name), _scope(scope), _openness(openness) {}

    bool operator==(const VariableType& type) const {
        return this->_name == type._name && this->_scope == type._scope;
    }
};

#endif