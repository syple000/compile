#include <string>

#ifndef CODE_GENERATION_VARIABLE_TYPE
#define CODE_GENERATION_VARIABLE_TYPE

struct VariableType {
    std::string _name;
    void* _scope;
    // openness 0: all, 1: adjent nodes, 2: owner node
    int _openness;

    VariableType(const std::string& name, void* scope, int openness) 
        : _name(name), _scope(scope), _openness(openness) {}
};

#endif