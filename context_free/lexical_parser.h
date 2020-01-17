
#include "./cf_expr.h"
#include "../regular_expr/regexpr_engine.h"
#include "../buffer/buffer.h"

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS 1

class LexicalParser {
    struct RegExprState {
        RegExprEngine* _regExprEngine;
        std::string _key;
        int _priority;
        int _curState;

        RegExprState(RegExprEngine* regExprEngine, const std::string& key, int priority, int curState)
         : _regExprEngine(regExprEngine), _key(key), _priority(priority), _curState(curState) {}

        bool operator< (const RegExprState& regExprState) const {
            if (this->_regExprEngine == regExprState._regExprEngine) {
                return this->_curState < regExprState._curState;
            } else {
                return this->_regExprEngine < regExprState._regExprEngine;
            }
        }
    };

    struct LexicalParserState {
        std::set<RegExprState> _regExprStateSet;
        std::string _matchingValue;
        int _priority = -1;

        bool operator< (const LexicalParserState& lexicalParserState) {
            return this->_regExprStateSet < lexicalParserState._regExprStateSet;
        }
    };

private:
    std::vector<std::vector<int>> _transTable;
    std::vector<LexicalParserState> _stateVec;
    std::map<LexicalParserState, int> _stateMap;
    std::vector<RegExprEngine*> _regExprEngineVec;
    int _maxPriority;

public:
    LexicalParser(const std::map<std::string, std::pair<std::string, int>>& keyRegExpMap) {
        LexicalParserState parserState;
        for (auto itr : keyRegExpMap) {
            RegExprEngine* regExprEngine = new RegExprEngine(itr.second.first, true);
            if (regExprEngine->IsTerminalState(0) && itr.second.second > parserState._priority) {
                parserState._matchingValue = itr.first;
                parserState._priority = itr.second.second;
            }
            if (itr.second.second > this->_maxPriority) {
                this->_maxPriority = itr.second.second;
            }
            this->_regExprEngineVec.push_back(regExprEngine);
            parserState._regExprStateSet.insert(RegExprState(regExprEngine, itr.first, itr.second.second, 0));
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

    virtual ~LexicalParser() {}

    void GenStatesByCurState(const LexicalParserState& state, int number);

    bool InsertLexicalParserState(LexicalParserState& state);

    // 空格/换行是默认分割符
    std::string GetNextWord(Buffer& buffer);
};

#endif