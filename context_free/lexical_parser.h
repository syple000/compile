
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

        bool operator< (const LexicalParserState& lexicalParserState) const {
            return this->_regExprStateSet < lexicalParserState._regExprStateSet;
        }
    };

private:
    std::vector<std::vector<int>> _transTable;
    std::vector<LexicalParserState*> _stateVec;
    std::map<LexicalParserState*, int, PointerObjectCmp<LexicalParserState>> _stateMap;
    std::vector<RegExprEngine*> _regExprEngineVec;
    int _maxPriority = -1;

    void GenStatesByCurState(LexicalParserState* state, int number);

    bool InsertLexicalParserState(LexicalParserState* state);

public:
    LexicalParser(const std::map<std::string, std::pair<std::string, int>>& keyRegExpMap);

    virtual ~LexicalParser();

    // 空格/换行是默认分割符
    std::string GetNextWord(Buffer& buffer);

    bool InitSuccess();
};

#endif