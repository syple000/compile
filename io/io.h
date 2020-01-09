#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include "../buffer/buffer.h"

#ifndef MATRIX_IO
#define MATRIX_IO 1

template<typename T>
class IO {
private:
    T (*_transFromString)(std::string);
    std::string (*_transToString)(T);
    Buffer* _buf;
public:
    IO(T (*transFromString)(std::string), std::string (*transToString)(T)) 
    : _transFromString(transFromString), _transToString(transToString), _buf(nullptr) {}
    ~IO() {
        delete this->_buf;
    }
    std::vector<std::vector<T>> GetMatrixFromBuf();
    std::vector<T> GetVecFromBuf();
    void AppendMatrixToBuf(const std::vector<std::vector<T>>& matrix);
    void AppendVecToBuf(const std::vector<T>& vec);
    int SetIndexOfBuf(int index);
    int ReadFile(const std::string& filePath);
    int WriteFile(const std::string& filePath, std::ios::openmode);
};

template<typename T>
int IO<T>::SetIndexOfBuf(int index) {
    if (this->_buf != nullptr) {
        this->_buf->_curPos = index;
        return index;
    }
    return -1;
}

template<typename T>
int IO<T>::ReadFile(const std::string& filePath) {
    std::ifstream in = std::ifstream(filePath, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        return -1;
    }
    in.seekg(0, std::ios::end);
    int bufSize = in.tellg();
    in.seekg(0, std::ios::beg);
    if (this->_buf == nullptr || this->_buf->_bufSize < bufSize) {
        delete this->_buf;
        this->_buf = new Buffer(bufSize);
    }
    this->_buf->_curPos = 0;
    in.read(this->_buf->_buf, bufSize);
    in.close();
    return 0;
}

template<typename T>
std::vector<std::vector<T>> IO<T>::GetMatrixFromBuf() {
    if (this->_buf == nullptr) {
        return std::vector<std::vector<int>>();
    } 
    int rowCount = atoi(this->_buf->GetNextStringSplitByBlank().c_str());
    int colCount = atoi(this->_buf->GetNextStringSplitByBlank().c_str());
    std::vector<std::vector<T>> matrix(rowCount, std::vector<T>(colCount));

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            matrix[i][j] = _transFromString(this->_buf->GetNextStringSplitByBlank());
        }
    }
    return matrix;
}

template<typename T>
std::vector<T> IO<T>::GetVecFromBuf() {
    if (this->_buf == nullptr) {
        return std::vector<int>();
    }
    int size = atoi(this->_buf->GetNextStringSplitByBlank().c_str());
    std::vector<int> vec = std::vector<int>(size);

    for (int i = 0; i < size; i++) {
        vec[i] = _transFromString(this->_buf->GetNextStringSplitByBlank());
    }
    return vec;
}

template<typename T>
void IO<T>::AppendMatrixToBuf(const std::vector<std::vector<T>>& matrix) {
    if (matrix.size() == 0 || matrix[0].size() == 0) {
        return;
    }
    int rowCount = matrix.size(), colCount = matrix[0].size();
    if (this->_buf == nullptr) {
        this->_buf = new Buffer((rowCount * colCount * (sizeof(T) + 2) + 22) << 1);
    }
    std::string numStr = std::to_string(rowCount);
    std::string space = " ";
    std::string lineBreak = "\n";
    this->_buf->AppendToBuffer(numStr.c_str(), numStr.size());
    this->_buf->AppendToBuffer(space.c_str(), space.size());
    numStr = std::to_string(colCount);
    this->_buf->AppendToBuffer(numStr.c_str(), numStr.size());
    this->_buf->AppendToBuffer(lineBreak.c_str(), lineBreak.size());

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            std::string str = _transToString(matrix[i][j]);
            this->_buf->AppendToBuffer(str.c_str(), str.size());
            this->_buf->AppendToBuffer(space.c_str(), space.size());
        }
        this->_buf->AppendToBuffer(lineBreak.c_str(), lineBreak.size());
    }
}

template<typename T>
void IO<T>::AppendVecToBuf(const std::vector<T>& vec) {
    if (vec.size() == 0) {
        return;
    }
    if (this->_buf == nullptr) {
        this->_buf = new Buffer((vec.size() * (sizeof(T) + 2) + 22) << 1); 
    }
    int size = vec.size();
    std::string numStr = std::to_string(size);
    std::string space = " ";
    std::string lineBreak = "\n";
    this->_buf->AppendToBuffer(numStr.c_str(), numStr.size());
    this->_buf->AppendToBuffer(lineBreak.c_str(), lineBreak.size());

    for (int i = 0; i < size; i++) {
        std::string str = _transToString(vec[i]);
        this->_buf->AppendToBuffer(str.c_str(), str.size());
        this->_buf->AppendToBuffer(space.c_str(), space.size());
    }
    this->_buf->AppendToBuffer(lineBreak.c_str(), lineBreak.size());
}


template<typename T>
int IO<T>::WriteFile(const std::string& filePath, std::ios::openmode ioMode) {
    if (this->_buf == nullptr) {
        return -2;
    }
    std::ofstream out = std::ofstream(filePath, std::ios::binary | ioMode);
    if (!out.is_open()) {
        return -1;
    }
    out.write(this->_buf->_buf, this->_buf->_curPos);
    out.close();

    return 0;
}

#endif