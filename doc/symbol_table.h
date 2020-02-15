#include <string>
#include <unordered_map>
#include <vector>

// 不直接参与编译，代码供expr_file使用
// 文法约束不能完全，需要分析辅助查找代码错误
// 一般地，文法约束越严格， 分析辅助复杂度越低

// 函数名与函数类型均以 $ 开始

// 默认数组类型，可强制转换
struct Array {
    int _size;
    int _count;

    Array* _subArray;

    Array(int varSize, int count) : _count(count), _size(varSize * count), _subArray(nullptr) {}

    Array(Array* subArray, int count) : _count(count), _subArray(subArray), _size(subArray->_size * count) {}

    static void DestroyArray(Array* arr) {
        if (arr != nullptr) {
            DestroyArray(arr->_subArray);
            delete arr;
        }
    }

    virtual ~Array() {}
};

#define PTR_SIZE 4
// 默认指针类型，可强制转换
struct Pointer {
    Pointer* _subPtr;

    Pointer() : _subPtr(nullptr) {}

    Pointer(Pointer* ptr) : _subPtr(ptr) {}

    static void DestroyPtr(Pointer* ptr) {
        if (ptr != nullptr) {
            DestroyPtr(ptr->_subPtr);
            delete ptr;
        }
    }
    
    virtual ~Pointer() {}
};

// 变量类型与变量内存大小
struct VarType {
    std::string _baseType;
    int _baseSize;
    Pointer* _ptrInfo;
    Array* _arrayInfo;

    VarType(const std::string& baseType, int baseSize) : _baseType(baseType), _baseSize(_baseSize), _arrayInfo(nullptr), _ptrInfo(nullptr) {}

    VarType(const std::string& baseType, int baseSize, Pointer* ptrInfo, Array* arrayInfo) 
        : _baseType(baseType), _baseSize(baseSize), _arrayInfo(arrayInfo), _ptrInfo(ptrInfo) {}

    int GetSize() const {
        if (this->_arrayInfo == nullptr) {
            if (this->_ptrInfo == nullptr) {
                return this->_baseSize;
            } else {
                return PTR_SIZE;
            }
        } else {
            return this->_arrayInfo->_size + PTR_SIZE;
        }
    }

    virtual ~VarType() {
        Pointer::DestroyPtr(this->_ptrInfo);
        Array::DestroyArray(this->_arrayInfo);
    }
};

struct Var {
    std::string _name;
    VarType* _type;
    int _size;
    // offset = -1 表示静态变量或函数
    int _offset;
    // 该参数给出静态位置（仅offset参数负数时生效）; 该参数不为负数，即从地址空间0处开始计算位置
    int _staticOff;

    Var(const std::string& name, VarType* type, int offset, bool isStatic) 
        : _name(name), _type(type), _size(type->GetSize()) {
        if (isStatic) {
            this->_staticOff = offset;
            this->_offset = -1;
        } else {
            this->_offset = offset;
            this->_staticOff = -1;
        }
    }

    virtual ~Var() {
        delete this->_type;
    }
};

struct SymbolTable {
    std::string _name;
    std::unordered_map<std::string, Var*> _vars;
    // 普通符号表：0 类型：1 函数：2 
    int _type;
    int _size;

    SymbolTable* _ptable;
    std::unordered_map<std::string, SymbolTable*> _ctables;

    SymbolTable(const std::string& name, int type, SymbolTable* ptable, int initSize) 
        : _name(name), _type(type), _ptable(ptable) {
        if (this->_ptable != nullptr) {
            this->_ptable->_ctables.insert(std::pair<std::string, SymbolTable*>(this->_name, this));
        }
        this->_size = initSize;
    }

    // offset 在该函数更新
    bool AddOrdinaryVar(const std::string& name, VarType* type) {
        if (this->_vars.find(name) != this->_vars.end()) {
            return false;
        }
        Var* var = new Var(name, type, this->_size, false);
        this->_vars.insert(std::pair<std::string, Var*>(name, var));
        this->_size += type->GetSize();
        return true;
    }

    // 静态变量: offset 在外围对象更新; 函数: offset表示函数代码所在位置
    bool AddStaticVar(const std::string& name, VarType* type, int offset) {
        if (this->_vars.find(name) != this->_vars.end()) {
            return false;
        }
        Var* var = new Var(name, type, offset, true);
        this->_vars.insert(std::pair<std::string, Var*>(name, var));
        return true;
    }

    Var* GetVar(const std::string& name) {
        auto itr = this->_vars.find(name);
        if (itr != this->_vars.end()) {
            return itr->second;
        }
        return nullptr;
    }

    virtual ~SymbolTable() {
        for (auto itr : this->_vars) {
            delete itr.second;
        }
    }

};

struct SymbolTableManager {
    int _staticOff;
    SymbolTable* _rootTable;
    SymbolTable* _curTable;

    SymbolTableManager(int initStaticOff) : _staticOff(initStaticOff) {
        this->_rootTable = new SymbolTable("", 0, nullptr, 0);
        this->_curTable = this->_rootTable;
        AddTable("float", 1, 4);
        AddTable("int", 1, 4);
        AddTable("char", 1, 1);
    }

    bool AddTable(const std::string& name, int type, int initSize) {
        if (this->_curTable->_ctables.find(name) != this->_curTable->_ctables.end()) {
            return false;
        } else {
            auto table = new SymbolTable(name, type, this->_curTable, initSize);
            this->_curTable->_ctables.insert(std::pair<std::string, SymbolTable*>(name, table));
            return true;
        }
    }

    SymbolTable* GoBack(SymbolTable* table) {
        return table->_ptable;
    }

    bool RollBackCurTable() {
        auto ptable = GoBack(this->_curTable);
        if (ptable == nullptr) {
            return false;
        } else {
            this->_curTable = ptable;
            return true;
        }
    }

    SymbolTable* GoAhead(SymbolTable* table, const std::string& nextTableName) {
        auto itr = table->_ctables.find(nextTableName);
        if (itr == table->_ctables.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    bool ForwardCurTable(const std::string& nextTableName) {
        auto nextTable = GoAhead(this->_curTable, nextTableName);
        if (nextTable == nullptr) {
            return false;
        } else {
            this->_curTable = nextTable;
            return true;
        }
    }

    bool AddVar(VarType* type, std::string& name, bool isStatic) {
        if (isStatic) {
            if (this->_curTable->AddStaticVar(name, type, this->_staticOff)) {
                this->_staticOff += type->GetSize();
                return true;
            } else {
                return false;
            }
        } else {
            return this->_curTable->AddOrdinaryVar(name, type);
        }
    }

    bool AddFunc(VarType* type, std::string& name) {
        return this->_curTable->AddStaticVar(name, type, 0);
    }

    Var* GetVarFromTable(SymbolTable* table, const std::string& name) {
        while (table != nullptr) {
            auto var = table->GetVar(name);
            if (var != nullptr) {
                return var;
            }
            table = table->_ptable;
        }
        return nullptr;
    }

    Var* GetVar(const std::string& name) {
        return GetVarFromTable(this->_curTable, name);
    }

    SymbolTable* GetChidTable(SymbolTable* ptable, const std::string& tableName) {
        auto itr = ptable->_ctables.find(tableName);
        if (itr == ptable->_ctables.end()) {
            return nullptr;
        } else {
            return itr->second;
        }
    }

    SymbolTable* GetTable(const std::string& tableName) {
        auto curTable = this->_curTable;
        while (curTable != nullptr) {
            auto table = GetChidTable(curTable, tableName);
            if (table != nullptr) {
                return table;
            }
            curTable = curTable->_ptable;
        }
        return nullptr;
    }

    SymbolTable* GetTable(const std::vector<std::string>& tableNames) {
        SymbolTable* root = this->_rootTable;
        for (auto tableName : tableNames) {
            root = GetChidTable(root, tableName);
            if (root == nullptr) {
                break;
            }
        }
        return root;
    }

    static void DestroyManager(SymbolTable* root) {
        if (root == nullptr) {
            return;
        }
        for (auto itr : root->_ctables) {
            DestroyManager(itr.second);
        }
        delete root;
    }

    virtual ~SymbolTableManager() {
        DestroyManager(this->_rootTable);
    }
};