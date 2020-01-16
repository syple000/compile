
#include "./cf_expr.h"
#include "../regular_expr/regexpr_engine.h"

#ifndef LEXICAL_ANALYSIS
#define LEXICAL_ANALYSIS 1

class LexicalParser {
    struct RegExprState {
        RegExprEngine* _regExprEngine;
        std::string _key;
        int _curState;

        RegExprState(std::string regExpr, std::string key, int curState) 
            : _key(key), _curState(curState) {
            this->_regExprEngine = new RegExprEngine(regExpr, true);
        }

        bool operator< (const RegExprState& regExprState) const {
            if (this->_regExprEngine == regExprState._regExprEngine) {
                return this->_curState < regExprState._curState;
            } else {
                return this->_regExprEngine < regExprState._regExprEngine;
            }
        }

        ~RegExprState() {
            delete this->_regExprEngine;
        }
    };

    struct LexicalParserState {
        int _number;
        std::set<RegExprState>* _regExprStateSet;
        std::string _matchingValue;

        LexicalParserState(int number) : _number(number) {
            this->_regExprStateSet = new std::set<RegExprState>();
        }

        virtual ~LexicalParserState() {
            delete this->_regExprStateSet;
        }

        bool operator< (const LexicalParserState& lexicalParserState) {
            return *this->_regExprStateSet < *lexicalParserState._regExprStateSet;
        }
    };

private:
    std::vector<std::vector<int>> _transTable;
    std::vector<LexicalParserState*> _stateVec;
    std::map<LexicalParserState*, int, SetCmp<LexicalParserState*>> _stateMap;

public:
    LexicalParser(const std::map<std::string, std::string>& keyRegExpMap) {
        auto firstState = new LexicalParserState(this->_stateVec.size());
        for (auto itr : keyRegExpMap) {
            RegExprState regExprState(itr.second, itr.first, 0);
            if (regExprState._regExprEngine->IsTerminalState(0)) {
                firstState->_matchingValue = itr.first;
            }
            firstState->_regExprStateSet->insert(regExprState);
        }
        InsertLexicalParserState(firstState);
        int oldSize = 0;
        while (this->_stateVec.size() != oldSize) {
            int size = this->_stateVec.size();
            for (; oldSize < size; oldSize++) {
                LexicalParserState* state = this->_stateVec[oldSize];
            }
        }
    }

    virtual ~LexicalParser() {}

    void GenStatesByCurState(LexicalParserState* state) {
        for (unsigned char ch = 0; ch < 256; ch++) {
            LexicalParserState* nextLexState = new LexicalParserState(this->_stateVec.size());
            for (auto& regExprState : *state->_regExprStateSet) {
                int nextRegState = regExprState._regExprEngine->TransferState(regExprState._curState, ch);
                if (nextRegState != -1) {
                    nextLexStateSet->insert(RegExprState(regExprState._regExprEngine, regExprState._key, nextRegState));
                    if (regExprState._regExprEngine->IsTerminalState(nextRegState)) {
                        matchingValue = regExprState._key;
                    }
                }
            }
            LexicalParserState nextLexState(this->_stateVec.size());
            if (this->_stateMap.find(nextLexState) != this->_stateMap.end()) {

            }
        }
    }

    bool InsertLexicalParserState(LexicalParserState* state) {
        if (this->_stateMap.find(state) == this->_stateMap.end()) {
            this->_stateMap.insert(std::pair<LexicalParserState*, int>(state, this->_stateVec.size()));
            this->_stateVec.push_back(state);
            return true;
        }
        return false;
    }
};

#endif