#include "./lexical_parser.h"

LexicalParser::LexicalParser(const std::map<std::string, std::pair<std::string, int>>& keyRegExpMap) {
    LexicalParserState* parserState = new LexicalParserState();
    for (auto itr : keyRegExpMap) {
        RegExprEngine* regExprEngine = new RegExprEngine(itr.second.first, true);
        if (regExprEngine->IsTerminalState(0) && itr.second.second > parserState->_priority) {
            parserState->_matchingValue = itr.first;
            parserState->_priority = itr.second.second;
        }
        if (itr.second.second > this->_maxPriority) {
            this->_maxPriority = itr.second.second;
        }
        this->_regExprEngineVec.push_back(regExprEngine);
        parserState->_regExprStateSet.insert(RegExprState(regExprEngine, itr.first, itr.second.second, 0));
    }
    InsertLexicalParserState(parserState);
    int oldSize = 0;
    while (this->_stateVec.size() != oldSize) {
        int size = this->_stateVec.size();
        for (; oldSize < size; oldSize++) {
            GenStatesByCurState(this->_stateVec[oldSize], oldSize);
        }
    }
}

LexicalParser::~LexicalParser() {
    for (auto regExprEngine : this->_regExprEngineVec) {
        delete regExprEngine;
    }
    for (int i = 0; i < this->_stateVec.size(); i++) {
        delete this->_stateVec[i];
    }
}

void LexicalParser::GenStatesByCurState(LexicalParserState* state, int number) {
    unsigned char ch = 0;
    for (int i = 0; i < 256; i++, ch++) {
        LexicalParserState* nextLexState = new LexicalParserState();
        for (auto& regExprState : state->_regExprStateSet) {
            int nextRegState = regExprState._regExprEngine->TransferState(regExprState._curState, ch);
            if (nextRegState != -1) {
                nextLexState->_regExprStateSet.insert(RegExprState(regExprState._regExprEngine, regExprState._key, 
                    regExprState._priority, nextRegState));
                if (regExprState._regExprEngine->IsTerminalState(nextRegState) && regExprState._priority > nextLexState->_priority) {
                    nextLexState->_matchingValue = regExprState._key;
                    nextLexState->_priority = regExprState._priority;
                }
            }
        }
        if (nextLexState->_regExprStateSet.size() != 0) {
            auto itr = this->_stateMap.find(nextLexState);
            if (itr == this->_stateMap.end()) {
                InsertLexicalParserState(nextLexState);
                this->_transTable[number][i] = this->_stateVec.size() -1;
            } else {
                delete nextLexState;
                this->_transTable[number][i] = itr->second;
            }
        } else {
            delete nextLexState;
        }
    }
}

bool LexicalParser::InsertLexicalParserState(LexicalParserState* state) {
    this->_stateMap.insert(std::pair<LexicalParserState*, int>(state, this->_stateVec.size()));
    this->_stateVec.push_back(state);
    this->_transTable.push_back(std::vector<int>(256, -1));
    return true;
}

// 空格/换行是默认分割符
std::string LexicalParser::GetNextWord(Buffer& buffer) {
    std::string word;
    int qualifiedPos = -1;
    int curState = 0;
    int priority = this->_stateVec[0]->_priority;
    while (buffer.CurrentCharAvailable()) {
        char ch = buffer.GetCurrentChar();
        if (ch == ' ' || ch == '\n') {
            buffer.MoveOnByChar();
            continue;
        } else {
            break;
        }
    }

    while (buffer.CurrentCharAvailable()) {
        unsigned char ch = buffer.GetCurrentChar();
        if (ch == ' ' || ch == '\n') {
            break;
        }
        curState = this->_transTable[curState][(int)ch];
        if (curState == -1) {
            break;
        } else if (this->_stateVec[curState]->_priority > priority) {
            priority = this->_stateVec[curState]->_priority;
            word = this->_stateVec[curState]->_matchingValue;
            qualifiedPos = buffer._curPos;
            if (priority == this->_maxPriority) {
                break;
            }
        }
        buffer.MoveOnByChar();
    }
    if (qualifiedPos != -1) {
        buffer._curPos = qualifiedPos + 1;
    }
    return word;
}