#include <string>

struct Buffer {
    char* _buf;
    int _curPos;
    int _bufSize;

    Buffer(int bufSize) {
        _buf = new char[bufSize];
        _curPos = 0;
        _bufSize = bufSize;
    }

    ~Buffer() {
        delete _buf;
    }

    std::string GetNextStringSplitByBlank();

    void AppendToBuffer(const char* str, int length);

};