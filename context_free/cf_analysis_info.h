#include <string>
#include <unordered_map>
#include <iostream>

#ifndef CF_INFO
#define CF_INFO

struct CfInfo {

    // 属性都必须继承该类
    struct Attribute {
        std::string _name;
        Attribute() {}
        Attribute(const std::string& name) : _name(name) {}
        virtual ~Attribute() {}
    };

    // 作为调试信息
    std::string _key;
    std::string _value;
    std::unordered_map<std::string, Attribute*> _attributes;

    // 叶节点
    CfInfo(const std::string& key, const std::string& value) 
        : _key(key), _value(value) {}

    // 归约节点
    CfInfo(const std::string& key) : _key(key) {}

    CfInfo(CfInfo&& cfInfo) = default;

    virtual ~CfInfo() {
        for (auto itr : this->_attributes) {
            delete itr.second;
        }
    }

    Attribute* GetAttribute(const std::string& name) {
        auto itr = this->_attributes.find(name);
        if (itr == this->_attributes.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    bool AddAttribute(Attribute* attribute) {
        if (this->_attributes.find(attribute->_name) != this->_attributes.end()) {
            return false;
        }
        this->_attributes.insert(std::pair<std::string, Attribute*>(attribute->_name, attribute));
        return true;
    }

    bool RemoveAttribute(const std::string& name) {
        auto itr = this->_attributes.find(name);
        if (itr == this->_attributes.end()) {
            return false;
        } else {
            this->_attributes.erase(itr);
            return true;
        }
    }

    static bool MoveAttribute(CfInfo& src, CfInfo& dest, const std::string& attrName) {
        auto attr = src.GetAttribute(attrName);
        if (attr == nullptr) {
            return false;
        } else {
            src._attributes.erase(attrName);
            return dest.AddAttribute(attr);
        }
    }

    static void MoveAttributes(CfInfo& src, CfInfo& dest) {
        for (auto itr : src._attributes) {
            dest.AddAttribute(itr.second);
        }
        src._attributes.clear();
    }

};

#endif