#include "./buffer.h"
#include <cstring>

#ifndef RE_BUFFER
#define RE_BUFFER

std::string Buffer::GetNextStringSplitByBlank() {
    std::string str;
    while (_curPos < _bufSize) {
        while ((_buf[_curPos] == ' ' || _buf[_curPos] == '\n' 
            || _buf[_curPos] == '\t') && _curPos < _bufSize) {
            _curPos++;
        }
        if (_curPos == _bufSize) {
            break;
        }
        while (_buf[_curPos] != ' ' && _buf[_curPos] != '\n' 
            && _buf[_curPos] != '\t' && _curPos < _bufSize) {
            str += _buf[_curPos++];
        }
        break;
    }
    return str;
}

void Buffer::AppendToBuffer(const char* str, int length) {
    memcpy(_buf + _curPos, str, length);
    _curPos += length;
}

#endif