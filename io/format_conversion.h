#include <string>
#include <cstring>

std::string String2String(const std::string& str) {
    return str;
}

std::string Int2String(const int& num) {
    return std::to_string(num);
}

int String2Int(const std::string& str) {
    return std::stoi(str);
}