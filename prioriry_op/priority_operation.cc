#include "./priority_operation.h"

template<typename T, typename K>
r_e::PriorityOperation<T, K>::PriorityOperation(
    std::unordered_map<K, r_e::SymbolContent<T>, std::hash<K>, std::equal_to<K>>&& precedenceMap) {
    this->_precedenceMap = std::move(precedenceMap);
}

template<typename T, typename K>
void r_e::PriorityOperation<T, K>::calc(const K& symbol) {
    const r_e::SymbolContent<T>& symbolContent = this->_precedenceMap[symbol];
    this->_symbols
}