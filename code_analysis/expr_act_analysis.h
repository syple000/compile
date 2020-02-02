
#include <string>
#include <vector>

#include "../tree_node/tree_node.h"
#include "./variable_scope.h"
#include "../string_util/string_util.h"

#ifndef CODE_ANALYSIS_EXPR_ACT_ANALYSIS
#define CODE_ANALYSIS_EXPR_ACT_ANALYSIS

class ExprActAnalysis {
public:
    static std::vector<std::vector<std::string>> ActAnalysis(const std::string& actStr) {
        std::vector<std::vector<std::string>> actCmds;
        auto acts = StringUtil::split(actStr, ";");
        for (auto act : acts) {
            std::string trimAct = StringUtil::trim(act);

#ifdef DEBUG_CODE
            if (trimAct.size() == 0) {
                std::cout << "act str: " << actStr << " redundant ; symbol!" << std::endl;
                continue;
            }
#endif

            auto actCmdElems = StringUtil::split(trimAct, "\\(|\\)");

#ifdef DEBUG_CODE
            if (actCmdElems.size() > 2) {
                std::cout << "act cmd " << trimAct << " error!" << std::endl;
                continue;
            }
#endif

            std::vector<std::string> actCmd;
            actCmd.push_back(StringUtil::trim(actCmdElems[0]));

            if (actCmdElems.size() == 2) {
                auto argsVec = StringUtil::split(actCmdElems[1], ",");
                for (auto arg : argsVec) {
                    std::string trimArg = StringUtil::trim(arg);
                    if (trimArg.size() != 0) {
                        actCmd.push_back(trimArg);
                    }
                }
            }

            actCmds.push_back(actCmd);
        }
        return actCmds;
    } 

    static std::string GetTranslatedCode(CfTreeNode* root, const std::string& act);

    static void AddChildScope(Scope* scope, CfTreeNode* root, const std::string& act);

    static void AddVariable(Scope* scope, CfTreeNode* root, const std::string& act);

    static void AddVariableType(Scope* scope, CfTreeNode* root, const std::string& act);

    static void RemoveChildScope(Scope* scope, CfTreeNode* root, const std::string& act);

    static void TransScope(Scope* scope, CfTreeNode* root, const std::string& act);

    static void BacktrackScope(Scope* scope, CfTreeNode* root, const std::string& act);

};

#endif