#include "./bitset.h"

int BitSet::CalcCapacity(int setSize) {
    return ((unsigned int)(setSize + 7)) >> 3;
}

unsigned char* BitSet::AllocSetElems(int size) {
    auto setElems = new unsigned char[size];
    memset(setElems, 0, size);
    return setElems;
}

void BitSet::ExpandCapacity(int expandedCap) {
    auto setElems = AllocSetElems(expandedCap);
    memcpy(setElems, this->_setElems, this->_capacity);
    
    delete[] this->_setElems;
    this->_setElems = setElems;
    this->_capacity = expandedCap;
}

int BitSet::Cmp(const BitSet& set1, const BitSet& set2) {
    for (int i = 0; i < set2._capacity; i++) {
        int cmpRes = set1._setElems[i] - set2._setElems[i];
        if (cmpRes != 0) {
            return cmpRes;
        } 
    }
    for (int i = set2._size; i < set1._capacity; i++) {
        if (set1._setElems[i] != 0) {
            return set1._setElems[i];
        }
    }
    return 0;
}

BitSet::BitSet(int setSize) {
    this->_capacity = CalcCapacity(setSize);
    this->_setElems = AllocSetElems(this->_capacity);
    this->_size = setSize;
}

BitSet::BitSet() {
    this->_size = 32;
    this->_capacity = 4;
    this->_setElems = AllocSetElems(this->_capacity);
}

BitSet::~BitSet() {
    delete[] this->_setElems;
}

BitSet::BitSet(const BitSet& set) {
    this->_capacity = set._capacity;
    this->_size = set._size;
    this->_setElems = AllocSetElems(this->_capacity);
    memcpy(this->_setElems, set._setElems, this->_capacity);
}

BitSet& BitSet::operator=(const BitSet& set) {
    this->_size = set._size;
    if (this->_capacity < set._capacity) {
        this->_capacity = set._capacity;
        delete[] this->_setElems;
        this->_setElems = AllocSetElems(this->_capacity);
    } else {
        memset(this->_setElems, 0, this->_capacity);
    }
    memcpy(this->_setElems, set._setElems, set._capacity);
    return *this;
}

void BitSet::Set(int pos) {
    int index = ((unsigned int)pos) >> 3;
    if (index >= this->_capacity) {
        ExpandCapacity(CalcCapacity(pos + 1) << 1);
    }
    if (pos >= this->_size) {
        this->_size = pos + 1;
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
    if (this->_capacity >= bitSet._capacity) {
        return Cmp(*this, bitSet) == 0;
    } else {
        return Cmp(bitSet, *this) == 0;
    }
}

void BitSet::Union(const BitSet& bitSet) {
    if (bitSet._capacity > this->_capacity) {
        // 并操作以最大的集合为标准
        ExpandCapacity(bitSet._capacity);
        this->_size = bitSet._size;
    }
    for (int i = 0; i < bitSet._capacity; i++) {
        this->_setElems[i] = this->_setElems[i] | bitSet._setElems[i];
    }
}

void BitSet::Intersect(const BitSet& bitSet) {
    for (int i = 0; i < this->_capacity && i < bitSet._capacity; i++) {
        this->_setElems[i] = this->_setElems[i] & bitSet._setElems[i];
    }
    if (bitSet._capacity < this->_capacity) {
        for (int i = bitSet._capacity; i < this->_capacity; i++) {
            this->_setElems[i] = 0;
        }
    }
}

void BitSet::Except(const BitSet& bitSet) {
    for (int i = 0; i < this->_capacity && i < bitSet._capacity; i++) {
        this->_setElems[i] = this->_setElems[i] & (~bitSet._setElems[i]);
    }
}

bool BitSet::Exist(int pos) const {
    int index = ((unsigned int)pos) >> 3;
    if (index >= this->_capacity) {
        return false;
    }
    return ((((unsigned char)0x80) >> (pos & 0x00000007)) & this->_setElems[index]) != 0; 
}

std::string BitSet::ToString() const {
    std::string str;
    for (int i = 0; i < this->_capacity; i++) {
        for (int j = 0; j < 8; j++) {
            str += ((((unsigned char)0x80) >> j) & this->_setElems[i]) == 0 ? '0' : '1';
        }
        if (i < this->_capacity - 1) {
            str += ' ';
        }
    }
    return str;
}

bool BitSet::Empty() const {
    for (int i = 0; i < this->_capacity; i++) {
        if (this->_setElems[i] != 0) {
            return false;
        }
    }
    return true;
}

int BitSet::GetSize() const {
    return this->_size;
}