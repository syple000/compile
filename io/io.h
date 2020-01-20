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
    T (*_transFromString)(const std::string&);
    std::string (*_transToString)(const T&);
public:
    IO(T (*transFromString)(const std::string&), std::string (*transToString)(const T&)) 
    : _transFromString(transFromString), _transToString(transToString) {}
    virtual ~IO() {}
    std::vector<std::vector<T>> GetMatrixFromBuf(Buffer& buf);
    std::vector<T> GetVecFromBuf(Buffer& buf);
    void AppendMatrixToBuf(Buffer& buf, const std::vector<std::vector<T>>& matrix);
    void AppendVecToBuf(Buffer& buf, const std::vector<T>& vec);
    int ReadFile(Buffer& buf, const std::string& filePath);
    int WriteFile(Buffer& buf, const std::string& filePath, std::ios::openmode);
};

template<typename T>
int IO<T>::ReadFile(Buffer& buf, const std::string& filePath) {
    std::ifstream in = std::ifstream(filePath, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        return -1;
    }
    in.seekg(0, std::ios::end);
    int bufSize = in.tellg();
    in.seekg(0, std::ios::beg);
    
    if (buf._bufSize < bufSize) {
        delete[] buf._buf;
        buf._buf = new char[bufSize];
        buf._bufSize = bufSize;
    }

    buf._curPos = 0;
    buf._contentSize = bufSize;
    in.read(buf._buf, bufSize);
    in.close();
    return 0;
}

template<typename T>
std::vector<std::vector<T>> IO<T>::GetMatrixFromBuf(Buffer& buf) {
    int rowCount = atoi(buf.GetNextStringSplitByBlank().c_str());
    int colCount = atoi(buf.GetNextStringSplitByBlank().c_str());
    std::vector<std::vector<T>> matrix(rowCount, std::vector<T>(colCount));

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            matrix[i][j] = _transFromString(buf.GetNextStringSplitByBlank());
        }
    }
    return matrix;
}

template<typename T>
std::vector<T> IO<T>::GetVecFromBuf(Buffer& buf) {
    int size = atoi(buf.GetNextStringSplitByBlank().c_str());
    std::vector<int> vec = std::vector<int>(size);

    for (int i = 0; i < size; i++) {
        vec[i] = _transFromString(buf.GetNextStringSplitByBlank());
    }
    return vec;
}

template<typename T>
void IO<T>::AppendMatrixToBuf(Buffer& buf, const std::vector<std::vector<T>>& matrix) {
    if (matrix.size() == 0 || matrix[0].size() == 0) {
        return;
    }
    int rowCount = matrix.size(), colCount = matrix[0].size();
    std::string numStr = std::to_string(rowCount);
    std::string space = " ";
    std::string lineBreak = "\n";
    buf.AppendToBuffer(numStr.c_str(), numStr.size());
    buf.AppendToBuffer(space.c_str(), space.size());
    numStr = std::to_string(colCount);
    buf.AppendToBuffer(numStr.c_str(), numStr.size());
    buf.AppendToBuffer(lineBreak.c_str(), lineBreak.size());

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            std::string str = _transToString(matrix[i][j]);
            buf.AppendToBuffer(str.c_str(), str.size());
            buf.AppendToBuffer(space.c_str(), space.size());
        }
        buf.AppendToBuffer(lineBreak.c_str(), lineBreak.size());
    }
}

template<typename T>
void IO<T>::AppendVecToBuf(Buffer& buf, const std::vector<T>& vec) {
    if (vec.size() == 0) {
        return;
    }
    int size = vec.size();
    std::string numStr = std::to_string(size);
    std::string space = " ";
    std::string lineBreak = "\n";
    buf.AppendToBuffer(numStr.c_str(), numStr.size());
    buf.AppendToBuffer(lineBreak.c_str(), lineBreak.size());

    for (int i = 0; i < size; i++) {
        std::string str = _transToString(vec[i]);
        buf.AppendToBuffer(str.c_str(), str.size());
        buf.AppendToBuffer(space.c_str(), space.size());
    }
    buf.AppendToBuffer(lineBreak.c_str(), lineBreak.size());
}


template<typename T>
int IO<T>::WriteFile(Buffer& buf, const std::string& filePath, std::ios::openmode ioMode) {
    std::ofstream out = std::ofstream(filePath, std::ios::binary | ioMode);
    if (!out.is_open()) {
        return -1;
    }
    out.write(buf._buf, buf._contentSize);
    out.close();

    return 0;
}

#endif