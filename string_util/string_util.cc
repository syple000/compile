
#include "./string_util.h"

std::pair<int, int> StringUtil::RegMatch(RegExprEngine& regExprEngine, const std::string& str, int startPos) {
    int curState = 0;
    int matchStartPos = startPos;
    int latelyMatchedPos = -1;
    int matchCurPos = matchStartPos;

    while (matchStartPos < str.size()) {
        curState = matchCurPos < str.size() ? regExprEngine.TransferState(curState, str[matchCurPos++]) : -1;
        if (curState == -1) {
            if (latelyMatchedPos == -1) {
                matchStartPos++;
                matchCurPos = matchStartPos;
                curState = 0;
            } else {
                return std::pair<int, int>(matchStartPos, latelyMatchedPos);
            }
        } else if (regExprEngine.IsTerminalState(curState)) {
            latelyMatchedPos = matchCurPos;
        }
    }
    return std::pair<int, int>(-1, -1);
}

std::vector<std::string> StringUtil::Split(const std::string& str, const std::string& regExpr) {
    std::vector<std::string> splitedStrs;
    RegExprEngine regExprEngine(regExpr);
    if (!regExprEngine.InitSuccess()) {
        return splitedStrs;
    }
    int startPos = 0;
    while (startPos < str.size()) {
        std::pair<int, int> matchInfo = RegMatch(regExprEngine, str, startPos);
        if (matchInfo.first < 0) {
            break;
        } else {
            if (matchInfo.first > startPos) {
                splitedStrs.push_back(str.substr(startPos, matchInfo.first - startPos));
            }
            startPos = matchInfo.second;
        }
    }
    if (str.size() > startPos) {
        splitedStrs.push_back(str.substr(startPos, str.size() - startPos));
    }
    return splitedStrs;
}

std::string StringUtil::Replace(const std::string& str, const std::string& regExpr, const std::string& replaceStr) {
    std::string resStr;
    RegExprEngine regExprEngine(regExpr);
    if (!regExprEngine.InitSuccess()) {
        return resStr;
    }
    int startPos = 0;
    while (startPos < str.size()) {
        std::pair<int, int> matchInfo = RegMatch(regExprEngine, str, startPos);
        if (matchInfo.first < 0) {
            break;
        } else {
            if (matchInfo.first > startPos) {
                resStr += str.substr(startPos, matchInfo.first - startPos);
            }
            resStr += replaceStr;
            startPos = matchInfo.second;
        }
    }
    if (str.size() > startPos) {
        resStr += str.substr(startPos, str.size() - startPos);
    }
    return resStr;
}

std::string StringUtil::Trim(const std::string& str) {
    int startIndex = 0, endIndex = str.size();
    for (; startIndex < str.size(); startIndex++) {
        if (str[startIndex] != ' ' && str[startIndex] != '\n') {
            break;
        }
    }
    for (; endIndex > 0; endIndex--) {
        if (str[endIndex - 1] != ' ' && str[endIndex - 1] != '\n') {
            break;
        }
    }
    if (endIndex > startIndex) {
        return str.substr(startIndex, endIndex);
    } else {
        return "";
    }
}