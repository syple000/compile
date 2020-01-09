#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include "../buffer/buffer.h"

#ifndef MATRIX_IO
#define MATRIX_IO 1

template<typename T>
class MatrixIo {
private:
    std::string _filePath;
    T (*_transFromString)(std::string);
    std::string (*_transToString)(T);
public:
    MatrixIo(const std::string& filePath, T (*transFromString)(std::string), std::string (*transToString)(T));
    ~MatrixIo() = default;
    std::vector<std::vector<T>> ReadFile();
    int WriteFile(const std::vector<std::vector<T>>&, std::ios::openmode);
};

template<typename T>
MatrixIo<T>::MatrixIo(const std::string& filePath, T (*transFromString)(std::string), std::string (*transToString)(T))
: _filePath(filePath), _transFromString(transFromString), _transToString(transToString){}

template<typename T>
std::vector<std::vector<T>> MatrixIo<T>::ReadFile() {
    std::ifstream in = std::ifstream(_filePath, std::ios::binary | std::ios::in);
    if (!in.is_open()) {
        return std::vector<std::vector<T>>();
    }
    in.seekg(0, std::ios::end);
    int bufSize = in.tellg();
    in.seekg(0, std::ios::beg);
    Buffer buf(bufSize);
    in.read(buf._buf, bufSize);
    
    int rowCount = atoi(buf.GetNextStringSplitByBlank().c_str());
    int colCount = atoi(buf.GetNextStringSplitByBlank().c_str());
    std::vector<std::vector<T>> matrix(rowCount, std::vector<T>(colCount));

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            matrix[i][j] = _transFromString(buf.GetNextStringSplitByBlank());
        }
    }

    in.close();
    return matrix;
}

template<typename T>
int MatrixIo<T>::WriteFile(const std::vector<std::vector<T>>& matrix, std::ios::openmode ioMode) {
    if (matrix.size() == 0 || matrix[0].size() == 0) {
        return -2;
    }
    std::ofstream out = std::ofstream(_filePath, std::ios::binary | ioMode);
    if (!out.is_open()) {
        return -1;
    }
    int rowCount = matrix.size(), colCount = matrix[0].size();
    Buffer buf(rowCount * colCount * 11 + 22);

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
    out.write(buf._buf, buf._curPos);
    out.close();

    return 0;
}

#endif