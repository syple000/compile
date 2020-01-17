#include <string>
#include <cstring>

#ifndef RE_BUFFER
#define RE_BUFFER 1

struct Buffer {
    char* _buf;
    int _curPos;
    int _contentSize;
    int _bufSize;

    Buffer(int bufSize) {
        _buf = new char[bufSize];
        _curPos = 0;
        _contentSize = 0;
        _bufSize = bufSize;
    }

    ~Buffer() {
        delete[] _buf;
    }

    std::string GetNextStringSplitByBlank() {
        std::string str;
        if (this->_curPos < this->_contentSize) {
            while ((this->_buf[this->_curPos] == ' ' || this->_buf[this->_curPos] == '\n' 
                || this->_buf[this->_curPos] == '\t') && this->_curPos < this->_contentSize) {
                this->_curPos++;
            }
            while (this->_buf[this->_curPos] != ' ' && this->_buf[this->_curPos] != '\n' 
                && this->_buf[this->_curPos] != '\t' && this->_curPos < this->_contentSize) {
                str += this->_buf[this->_curPos++];
            }
        }
        return str;
    }

    std::string GetNextLine() {
        // skip blank line
        std::string str;
        if (this->_curPos < this->_contentSize) {
            while (this->_buf[this->_curPos] == '\n' && this->_curPos < this->_contentSize) {
                this->_curPos++;
            }
            while (this->_buf[this->_curPos] != '\n' && this->_curPos < this->_contentSize) {
                str += this->_buf[this->_curPos++];
            }
        }
        return str;
    }

    void AppendToBuffer(const char* str, int length) {
        if (this->_curPos + length > this->_bufSize) {
            char* oldBuf = this->_buf;
            this->_bufSize = (this->_curPos + length) < 1 << 30 ? (this->_curPos + length) << 1 : 1 << 31;
            this->_buf = new char[this->_bufSize];
            memcpy(this->_buf, oldBuf, this->_contentSize);
            delete[] oldBuf;
        }
        int copyLength = length < this->_bufSize - this->_curPos ? length : this->_bufSize - this->_curPos;
        memcpy(this->_buf + this->_curPos, str, copyLength);
        this->_curPos += copyLength;
        if (this->_curPos > this->_contentSize) {
            this->_contentSize = this->_curPos;
        }
    }

};

#endif