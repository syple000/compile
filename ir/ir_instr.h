#include <string>
#include <vector>
#include <cstdarg>

#ifndef IR_INSTR
#define IR_INSTR 1

struct Instruction {
    Instruction* _pre, *_next;
    std::string _label;
    std::vector<std::string> _components;

    Instruction(Instruction* pre, const std::string& label, int argCount, ...) : _pre(pre), _label(label) {
        va_list args;
        va_start(args, argCount);
        for (int i = 0; i < argCount; i++) {
            this->_components.push_back(va_arg(args, char*));
        }
    }
};

#endif