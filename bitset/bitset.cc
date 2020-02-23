#include "./bitset.h"

int BitSet::CalcSize(int setSize) {
    return ((unsigned int)(setSize + 7)) >> 3;
}

unsigned char* BitSet::AllocSetElems(int size) {
    auto setElems = new unsigned char[size];
    memset(setElems, 0, size);
    return setElems;
}

void BitSet::ExpandCapacity(int expandedSize) {
    auto setElems = AllocSetElems(expandedSize);
    memcpy(setElems, this->_setElems, this->_size);
    
    delete[] this->_setElems;
    this->_setElems = setElems;
    this->_size = expandedSize;
}

int BitSet::Cmp(const BitSet& set1, const BitSet& set2) {
    for (int i = 0; i < set2._size; i++) {
        int cmpRes = set1._setElems[i] - set2._setElems[i];
        if (cmpRes != 0) {
            return cmpRes;
        } 
    }
    for (int i = set2._size; i < set1._size; i++) {
        if (set1._setElems[i] != 0) {
            return set1._setElems[i];
        }
    }
    return 0;
}

BitSet::BitSet(int setSize) {
    this->_size = CalcSize(setSize);
    this->_setElems = AllocSetElems(this->_size);
}

BitSet::BitSet() {
    this->_size = 4;
    this->_setElems = AllocSetElems(this->_size);
}

BitSet::~BitSet() {
    delete[] this->_setElems;
}

void BitSet::Set(int pos) {
    int index = ((unsigned int)pos) >> 3;
    if (index >= this->_size) {
        ExpandCapacity(CalcSize(pos + 1) << 1);
    }
    unsigned char newBit = ((unsigned char)0x80) >> (pos & 0x00000007);
    this->_setElems[index] = this->_setElems[index] | newBit;
}

void BitSet::Clear(int pos) {
    int index = ((unsigned int)pos) >> 3;
    if (index >= this->_size) {
        // 超过界限清空位不需要操作
        return;
    }
    unsigned char newBit = ((unsigned char)0x80) >> (pos & 0x00000007) ^ ((unsigned char)0xFF);
    this->_setElems[index] = this->_setElems[index] & newBit;
}

bool BitSet::operator==(const BitSet& bitSet) const {
    if (this->_size >= bitSet._size) {
        return Cmp(*this, bitSet) == 0;
    } else {
        return Cmp(bitSet, *this) == 0;
    }
}

void BitSet::Union(const BitSet& bitSet) {
    if (bitSet._size > this->_size) {
        // 并操作以最大的集合size为标准
        ExpandCapacity(bitSet._size);
    }
    for (int i = 0; i < bitSet._size; i++) {
        this->_setElems[i] = this->_setElems[i] | bitSet._setElems[i];
    }
}

void BitSet::Intersect(const BitSet& bitSet) {
    for (int i = 0; i < this->_size && i < bitSet._size; i++) {
        this->_setElems[i] = this->_setElems[i] & bitSet._setElems[i];
    }
    if (bitSet._size < this->_size) {
        for (int i = bitSet._size; i < this->_size; i++) {
            this->_setElems[i] = 0;
        }
    }
}

void BitSet::Except(const BitSet& bitSet) {
    for (int i = 0; i < this->_size && i < bitSet._size; i++) {
        this->_setElems[i] = this->_setElems[i] & (~bitSet._setElems[i]);
    }
}

bool BitSet::Exist(int pos) const {
    int index = ((unsigned int)pos) >> 3;
    if (index >= this->_size) {
        return false;
    }
    return ((((unsigned char)0x80) >> (pos & 0x00000007)) & this->_setElems[index]) != 0; 
}

std::string BitSet::ToString() const {
    std::string str;
    for (int i = 0; i < this->_size; i++) {
        for (int j = 0; j < 8; j++) {
            str += ((((unsigned char)0x80) >> j) & this->_setElems[i]) == 0 ? '0' : '1';
        }
        if (i < this->_size - 1) {
            str += ' ';
        }
    }
    return str;
}