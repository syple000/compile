#include <set>

#ifndef CMP
#define CMP 1

template<typename T>
struct SetCmp {
    bool operator() (const std::set<T>* set1, const std::set<T>* set2) const {
        return *set1 < *set2;
    }
};

template<typename T>
struct PointerObjectCmp {
    bool operator() (const T* t1, const T* t2) const {
        return *t1 < *t2;
    }
};

#endif