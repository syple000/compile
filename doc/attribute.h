#include "../context_free/cf_analysis_info.h"
#include "./symbol_table.h"

struct TypeAttribute : public CfInfo::Attribute {
    std::string _typeName;
    int _typeSize;

    TypeAttribute(const std::string& attrName, const std::string& typeName, int typeSize) 
        : Attribute(attrName), _typeName(typeName), _typeSize(typeSize) {}

    virtual ~TypeAttribute() {}
};

struct QualifierAttribute : public CfInfo::Attribute {
    bool _isStatic;

    QualifierAttribute(const std::string& attrName, bool isStatic) 
        : Attribute(attrName), _isStatic(isStatic) {}

    virtual ~QualifierAttribute() {}
};

struct PtrAttribute : public CfInfo::Attribute {
    Pointer* _ptr;

    PtrAttribute(const std::string& attrName) : Attribute(attrName), _ptr(nullptr) {}

    void AddNestPtr() {
        this->_ptr = new Pointer(this->_ptr);
    }

    virtual ~PtrAttribute() {
        Pointer::DestroyPtr(this->_ptr);
    }
};

struct VarAttribute : public CfInfo::Attribute {
    const std::string _varName;

    VarAttribute(const std::string& attrName, const std::string& varName) : Attribute(attrName), _varName(varName) {}

    virtual ~VarAttribute() {}
};

struct ArrAttribute : public CfInfo::Attribute {
    Array* _array;

    ArrAttribute(const std::string& attrName) : Attribute(attrName), _array(nullptr) {}

    void AddNestArr(int varSize, int count) {
        if (this->_array == nullptr) {
            this->_array = new Array(varSize, count);
        } else {
            this->_array = new Array(this->_array, count);
        }
    }

    virtual ~ArrAttribute() {
        Array::DestroyArray(this->_array);
    }
};