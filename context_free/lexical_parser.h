
#include "./cf_expr.h"
#include "../regular_expr/regexpr_engine.h"

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS 1

class LexicalParser {
    struct RegExprState {
        RegExprEngine* _regExprEngine;
        int _curState;

        RegExprState(RegExprEngine* regExprEngine, int curState)
         : _regExprEngine(regExprEngine), _curState(curState) {}

        RegExprState(RegExprState&&) = default;

        bool operator< (const RegExprState& regExprState) const {
            if (this->_regExprEngine == regExprState._regExprEngine) {
                return this->_curState < regExprState._curState;
            } else {
                return this->_regExprEngine < regExprState._regExprEngine;
            }
        }
    };

    struct LexicalParserState {
        int _number;
        std::set<RegExprState> _regExprStateSet;
        std::string _matchingValue;

        LexicalParserState(int number) : _number(number) {}

        LexicalParserState(LexicalParserState&&) = default;

        LexicalParserState(const LexicalParserState&) = default;

        bool operator< (const LexicalParserState& lexicalParserState) {
            return this->_regExprStateSet < lexicalParserState._regExprStateSet;
        }
    };

private:
    std::vector<std::vector<int>> _transTable;
    std::vector<LexicalParserState> _stateVec;
    std::map<LexicalParserState, int> _stateMap;
    std::vector<RegExprEngine*> _regExprEngineVec;

public:
    LexicalParser(const std::map<std::string, std::string>& keyRegExpMap) {
        LexicalParserState parserState(this->_stateVec.size());
        for (auto itr : keyRegExpMap) {
            RegExprEngine* regExprEngine = new RegExprEngine(itr.second, true);
            if (regExprEngine->IsTerminalState(0)) {
                parserState._matchingValue = itr.first;
            }
            this->_regExprEngineVec.push_back(regExprEngine);
            parserState._regExprStateSet.insert(RegExprState(regExprEngine, 0));
        }
        InsertLexicalParserState(parserState);
        int oldSize = 0;
        while (this->_stateVec.size() != oldSize) {
            int size = this->_stateVec.size();
            for (; oldSize < size; oldSize++) {
                LexicalParserState& state = this->_stateVec[oldSize];

            }
        }
    }

    virtual ~LexicalParser() {}

    void GenStatesByCurState(const LexicalParserState& state) {
        unsigned char ch = 0;
        for (int i = 0; i < 256; i++, ch++) {
            LexicalParserState nextLexState(this->_stateVec.size());
            for (auto& regExprState : state._regExprStateSet) {
                int nextRegState = regExprState._regExprEngine->TransferState(regExprState._curState, ch);
                if (nextRegState != -1) {
                    nextLexState._regExprStateSet.insert(RegExprState(regExprState._regExprEngine, nextRegState));
                    if (regExprState._regExprEngine->IsTerminalState(nextRegState)) {
                        nextLexState._matchingValue = 
                    }
                }
            }
            LexicalParserState nextLexState(this->_stateVec.size());
            if (this->_stateMap.find(nextLexState) != this->_stateMap.end()) {

            }
        }
    }

    bool InsertLexicalParserState(LexicalParserState& state) {
        if (this->_stateMap.find(state) == this->_stateMap.end()) {
            this->_stateMap.insert(std::pair<LexicalParserState, int>(std::move(state), this->_stateVec.size()));
            this->_stateVec.push_back(state);
            return true;
        }
        return false;
    }
};

#endif