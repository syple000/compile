#include <string>

#ifndef RE_BUFFER
#define RE_BUFFER 1

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
        delete[] _buf;
    }

    std::string GetNextStringSplitByBlank() {
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

    void AppendToBuffer(const char* str, int length) {
        if (this->_curPos + length > this->_bufSize) {
            char* oldBuf = this->_buf;
            int oldBufSize = this->_bufSize;
            this->_bufSize = (this->_curPos + length) < 1 << 30 ? (this->_curPos + length) << 1 : 1 << 31;
            this->_buf = new char[this->_bufSize];
            memcpy(this->_buf, oldBuf, oldBufSize);
            delete[] oldBuf;
        }
        int copyLength = length < this->_bufSize - this->_curPos ? length : this->_bufSize - this->_curPos;
        memcpy(_buf + _curPos, str, copyLength);
        _curPos += copyLength;
    }

};

#endif