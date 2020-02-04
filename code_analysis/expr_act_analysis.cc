
#include "./expr_act_analysis.h"

ExprActAnalysis::ExprActAnalysis() : _semicolonRegExprEngine(";"), _parenthesesRegExprEngine("\\(|\\)"), _commaRegExprEngine(",") {
    std::map<std::string, std::pair<std::string, int>> keyExprMap;
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(" ", std::pair<std::string, int>("\\\\sp", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(",", std::pair<std::string, int>("\\\\cm", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("(", std::pair<std::string, int>("\\\\lp", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(")", std::pair<std::string, int>("\\\\rp", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>(";", std::pair<std::string, int>("\\\\sm", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("=", std::pair<std::string, int>("\\\\eq", 0)));
    keyExprMap.insert(std::pair<std::string, std::pair<std::string, int>>("&", std::pair<std::string, int>("\\\\sg", 0)));
    this->_lexicalParser = new  LexicalParser(keyExprMap);
}

ExprActAnalysis::~ExprActAnalysis() {
    delete this->_lexicalParser;
}

std::unordered_map<std::string, std::vector<std::vector<std::string>>> ExprActAnalysis::GetActsFromStr(const std::string& actStr) {
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> actCmds;
    auto acts = StringUtil::Split(actStr, this->_semicolonRegExprEngine);
    for (auto act : acts) {
        std::string trimAct = StringUtil::Trim(act);

#ifdef DEBUG_CODE
        if (trimAct.size() == 0) {
            std::cout << "act str: " << actStr << " redundant ; symbol!" << std::endl;
            continue;
        }
#endif

        auto actCmdElems = StringUtil::Split(trimAct, this->_parenthesesRegExprEngine);

#ifdef DEBUG_CODE
        if (actCmdElems.size() > 2) {
            std::cout << "act cmd " << trimAct << " error!" << std::endl;
            continue;
        }
#endif

        std::vector<std::string> actCmd;
        actCmd.push_back(StringUtil::Trim(actCmdElems[0]));

        if (actCmdElems.size() == 2) {
            auto argsVec = StringUtil::Split(actCmdElems[1], this->_commaRegExprEngine);
            for (auto arg : argsVec) {
                std::string trimArg = StringUtil::Trim(arg);
                if (trimArg.size() != 0) {
                    actCmd.push_back(StringUtil::Replace(trimArg, *this->_lexicalParser));
                }
            }
        }
        if (actCmds.find(actCmd[0]) == actCmds.end()) {
            actCmds.insert(std::pair<std::string, std::vector<std::vector<std::string>>>(actCmd[0], std::vector<std::vector<std::string>>()));
        }
        actCmds.find(actCmd[0])->second.push_back(actCmd);
    }
    return actCmds;
}


// 目标函数名： genCode() 当字节点无value时，递归生成
std::string ExprActAnalysis::GenCode(CfTreeNode* root, Process* process) {
    if (root->_reductionAction.size() == 0) {
        return "";
    }
    auto actCmds = GetActsFromStr(root->_reductionAction);
    std::string code;
    for (auto actCmd : actCmds) {
        if (actCmd.first == "genCode") {

#ifdef DEBUG_CODE
            if (actCmd.second.size() != 1) {
                std::cout << "key: " << root->_key << " generate code function count error!" << std::endl;
                continue;
            }
#endif

            for (int i = 1; i < actCmd.second[0].size(); i++) {
                auto& arg = actCmd.second[0][i];
                if (arg[0] == '$') {
                    int index = std::stoi(arg.substr(1, arg.size() - 1));
                    if (root->_cnodes[index - 1]->_value.size() != 0) {
                        code += process->process(root->_cnodes[index - 1]->_value);
                    } else {
                        code += GenCode(root->_cnodes[index - 1], process);
                    }
                } else {
                    auto itr = root->_attributes.find(arg);
                    if (itr == root->_attributes.end()) {
                        code += process->process(arg);
                    } else {
                        code += process->process(itr->second->GetAsString());
                    }
                }
            }
        } else {
            // TODO
        }
    }

    return code;
}