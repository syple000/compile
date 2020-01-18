#include <string>
#include <cstring>
#include <vector>

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
        if (CurrentCharAvailable()) {
            char ch = GetCurrentChar();
            while (ch == ' ' || ch == '\n' || ch == '\t') {
                MoveOnByChar();
                if (CurrentCharAvailable()) {
                    ch = GetCurrentChar();
                } else {
                    break;
                }
            }
            while (ch != ' ' && ch != '\n' && ch != '\t') {
                str += ch;
                MoveOnByChar();
                if (CurrentCharAvailable()) {
                    ch = GetCurrentChar();
                } else {
                    break;
                }
            }
        }
        return str;
    }

    // 下一行有内容的行，跳过空行（当前行若有剩余元素，取出返回）
    std::string GetNextLine() {
        std::string str;
        if (CurrentCharAvailable()) {
            char ch = GetCurrentChar();
            while (ch == '\n') {
                MoveOnByChar();
                if (CurrentCharAvailable()) {
                    ch = GetCurrentChar();
                } else {
                    break;
                }
            }

            while (ch != '\n') {
                str += ch;
                MoveOnByChar();
                if (CurrentCharAvailable()) {
                    ch = GetCurrentChar();
                } else {
                    break;
                }
            }
        }
        return str;
    }

    // 不跳过空行(若当前行仍有元素,包含\n，返回)
    std::vector<std::string> GetStringsOfNextLine() {
        std::vector<std::string> strVec;
        if (CurrentCharAvailable()) {
            char ch = GetCurrentChar();
            while (CurrentCharAvailable() && ch != '\n') {
                while (ch == ' ') {
                    MoveOnByChar();
                    if (CurrentCharAvailable()) {
                        ch = GetCurrentChar();
                    } else {
                        break;
                    }
                }
                std::string str;
                while (ch != ' ' && ch != '\n') {
                    str += ch;
                    MoveOnByChar();
                    if (CurrentCharAvailable()) {
                        ch = GetCurrentChar();
                    } else {
                        break;
                    }
                }
                if (str.size() != 0) {
                    strVec.push_back(std::move(str));
                }
            }
        }
        if (CurrentCharAvailable()) {
            MoveOnByChar();
        }
        return strVec;
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

    char GetCurrentChar() {
        return this->_buf[this->_curPos];
    }

    bool CurrentCharAvailable() {
        return this->_curPos < this->_contentSize;
    }

    void MoveOnByChar() {
        this->_curPos++;
    }

};

#endif