#include "../tree_node/tree_node.h"

struct TypeAttribute : public CfTreeNode::Attribute {
    std::string _typeName;
    int _typeSize;

    TypeAttribute(const std::string& attrName, const std::string& typeName, int typeSize) {
        this->_name = attrName;
        this->_typeName = typeName;
        this->_typeSize = typeSize;
    }

    virtual ~TypeAttribute() {}
};

struct QualifierAttribute : public CfTreeNode::Attribute {
    bool _isStatic;

    QualifierAttribute(const std::string& attrName, bool isStatic) {
        this->_name = attrName;
        this->_isStatic = isStatic;
    }

    virtual ~QualifierAttribute() {}
};