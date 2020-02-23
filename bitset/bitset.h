#include <string>
#include <cstring>

#ifndef BIT_SET
#define BIT_SET

class BitSet {
private:
    unsigned char *_setElems;
    // size表示setElems的个数，集合大小为size * 8
    int _size;

    int CalcSize(int setSize);

    unsigned char* AllocSetElems(int size);

    void ExpandCapacity(int expandedSize);

    // 约定： set1的size大于或等于set2的size
    static int Cmp(const BitSet& set1, const BitSet& set2);
public:
    BitSet(int setSize);

    BitSet();

    virtual ~BitSet();

    void Set(int pos);

    void Clear(int pos);

    bool operator== (const BitSet&) const;

    bool Exist(int pos) const;

    std::string ToString() const;

    void Union(const BitSet&);

    void Intersect(const BitSet&);

    void Except(const BitSet&);

};

#endif
