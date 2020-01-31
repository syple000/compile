#include <string>

#include "./variable_type.h"

#ifndef CODE_GENERATION_VARIABLE
#define CODE_GENERATION_VARIABLE 1

struct Variable {
    VariableType* _type;
    std::string _name;
    int _reference;
    std::string _immediate;
    // openness 0: all, 1: adjent nodes, 2: owner node   
    // life cycle 0: 当前作用域内; 1: 全局 
    int _openness;
    int _lifeCycle;

    Variable(VariableType* type, const std::string& name, int reference, int openness, int lifeCycle) 
        : _type(type), _name(name), _openness(openness), _lifeCycle(lifeCycle) {
        this->_reference = reference;
    }

    Variable(VariableType* type, const std::string& name, const std::string& immediate, int openness, int lifeCycle) 
        : _type(type), _name(name), _openness(openness), _lifeCycle(lifeCycle) {
        this->_immediate = immediate;
        this->_reference = -1;
    }

    virtual ~Variable() {};
};

#endif