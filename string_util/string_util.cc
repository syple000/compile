
#include "./string_util.h"

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& regExpr) {
    std::vector<std::string> splitedStrs;
    RegExprEngine regExprEngine(regExpr);
    if (!regExprEngine.InitSuccess()) {
        return splitedStrs;
    }
    int startPos = 0;
    int curState = 0;
    int matchStartPos = 0;
    int latelyMatchedPos = -1;
    int matchCurPos = 0;
    while (matchStartPos < str.size()) {
        curState = matchCurPos < str.size() ? regExprEngine.TransferState(curState, str[matchCurPos++]) : -1;
        if (curState == -1) {
            if (latelyMatchedPos == -1) {
                matchStartPos++;
                matchCurPos = matchStartPos;
                curState = 0;
            } else {
                if (matchStartPos > startPos) {
                    splitedStrs.push_back(str.substr(startPos, matchStartPos - startPos));
                }
                startPos = latelyMatchedPos;
                matchStartPos = startPos;
                matchCurPos = matchStartPos;
                curState = 0;
                latelyMatchedPos = -1;
                continue;
            }
        } else if (regExprEngine.IsTerminalState(curState)) {
            latelyMatchedPos = matchCurPos;
        }
    }
    if (matchStartPos > startPos) {
        splitedStrs.push_back(str.substr(startPos, matchStartPos - startPos));
    }
    return splitedStrs;
}

std::string StringUtil::trim(const std::string& str) {
    int startIndex = 0, endIndex = str.size();
    for (; startIndex < str.size(); startIndex++) {
        if (str[startIndex] != ' ') {
            break;
        }
    }
    for (; endIndex > 0; endIndex--) {
        if (str[endIndex - 1] != ' ') {
            break;
        }
    }
    if (endIndex > startIndex) {
        return str.substr(startIndex, endIndex);
    } else {
        return "";
    }
}