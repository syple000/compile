#include <string>
#include <vector>

#include "../regular_expr/regexpr_engine.h"

#ifndef STRING_UTIL
#define STRING_UTIL 1

class StringUtil {
private:
    static std::pair<int, int> RegMatch(RegExprEngine& engine, const std::string& str, int startPos);
public:
    static std::vector<std::string> Split(const std::string& str, const std::string& regExpr);
    static std::string Trim(const std::string& str);
    static std::string Replace(const std::string& str, const std::string& regExpr, const std::string& replaceStr);
};

#endif