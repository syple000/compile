#include <set>

template<typename T>
struct SetCmp {
    bool operator() (const std::set<T>* set1, const std::set<T>* set2) const {
        return *set1 < *set2;
    }
};