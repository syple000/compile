#include <string>
#include <map>

#include "./variable_type.h"

#ifndef CODE_GENERATION_VARIABLE
#define CODE_GENERATION_VARIABLE 1

struct Variable {
    VariableType* _type;
    std::string _name;
    std::string _value;
    // openness 含义与variable type的解释一致
    int _openness;
    // life cycle： 0 仅当前作用域内; 1 全局 
    int _lifeCycle;

    Variable(VariableType* type, const std::string& name, const std::string& value, int openness, int lifeCycle) 
        : _type(type), _name(name), _value(value), _openness(openness), _lifeCycle(lifeCycle) {}

    virtual ~Variable() {};
};

#endif