
#include <string>
#include <vector>

#include "../string_util/string_util.h"

#ifndef CODE_ANALYSIS_EXPR_ACT_ANALYSIS
#define CODE_ANALYSIS_EXPR_ACT_ANALYSIS

class ExprActAnalysis {
public:
    static std::unordered_map<std::string, std::vector<std::vector<std::string>>> GetActsFromStr(const std::string& actStr) {
        std::unordered_map<std::string, std::vector<std::vector<std::string>>> actCmds;
        auto acts = StringUtil::Split(actStr, ";");
        for (auto act : acts) {
            std::string trimAct = StringUtil::Trim(act);

#ifdef DEBUG_CODE
            if (trimAct.size() == 0) {
                std::cout << "act str: " << actStr << " redundant ; symbol!" << std::endl;
                continue;
            }
#endif

            auto actCmdElems = StringUtil::Split(trimAct, "\\(|\\)");

#ifdef DEBUG_CODE
            if (actCmdElems.size() > 2) {
                std::cout << "act cmd " << trimAct << " error!" << std::endl;
                continue;
            }
#endif

            std::vector<std::string> actCmd;
            actCmd.push_back(StringUtil::Trim(actCmdElems[0]));

            if (actCmdElems.size() == 2) {
                auto argsVec = StringUtil::Split(actCmdElems[1], ",");
                for (auto arg : argsVec) {
                    std::string trimArg = StringUtil::Trim(arg);
                    if (trimArg.size() != 0) {
                        actCmd.push_back(StringUtil::Replace(trimArg, "\\\\s", " "));
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
    static std::string GenCode(CfTreeNode* root, std::string(*process)(const std::string&)) {
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
                            code += process(root->_cnodes[index - 1]->_value);
                        } else {
                            code += GenCode(root->_cnodes[index - 1], process);
                        }
                    } else {
                        auto itr = root->_attributes.find(arg);
                        if (itr == root->_attributes.end()) {
                            code += arg;
                        } else {
                            code += itr->second->GetAttributeAsString();
                        }
                        code += ' ';
                    }
                }
            } else {
                // TODO
            }
        }

        return code;
    }
};

#endif