#include <unordered_map>
#include <stack>
#include <string>
#include <vector>

#ifndef PRIORITY_OPERATION
#define PRIORITY_OPERATION

#define NONEXISTENT_SYMBOL nonexistent_symbol

namespace r_e {

    template<typename T>
    struct SymbolContent {
        int _priority;  // if the value is larger, priority is higher
        int _paramCount;
        T _func(std::vector<T>);
    };

    template<typename T, typename K>
    class PriorityOperation {
    private:
        std::unordered_map<K, SymbolContent<T>, std::hash<K>, std::equal_to<K>> _precedenceMap;
        std::stack<K> _symbols;
        std::stack<T> _elems;

        void calc(const K& symbol);
        bool 
    public:
        PriorityOperation(std::unordered_map<K, SymbolContent<T>, std::hash<K>, std::equal_to<K>>&&);
        virtual ~PriorityOperation() {};
        T run();
    };
};

#endif
