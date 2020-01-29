#include <string>

#ifndef CODE_GENERATION_VARIABLE
#define CODE_GENERATION_VARIABLE 1

struct Variable {
    // 变量类型(int, char, 类...); 基本变量类型数量统计最大值： 0-BASIC_TYPE_COUNT 基本变量， BASIC_TYPE_COUNT-正无穷 非基本变量
    int _type;
    static const int BASIC_TYPE_COUNT = 126;

    std::string _name;
    union {
        int _reference;
        std::string _immediate;
    };

    Variable(int type, const std::string& name, int reference) : _type(type), _name(name) {
        this->_reference = reference;
    }

    Variable(int type, const std::string& name, const std::string& immediate) : _type(type), _name(name) {
        this->_immediate = immediate;
    }

    virtual ~Variable() = default;
};

#endif