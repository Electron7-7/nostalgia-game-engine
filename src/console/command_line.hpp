#ifndef CONSOLE_H
#define CONSOLE_H

#include "command.hpp"
#include "variable.hpp"
#include <unordered_map>

class CommandLine
{
public:
    static bool VariableExists(const std::string& Name);
    static std::string GetStringVariable(const std::string& Name);
    static int GetIntVariable(const std::string& Name);
    static float GetFloatVariable(const std::string& Name);
    static bool GetBoolVariable(const std::string& Name);
    static void AddVariable(const ConsoleVariable& Variable);
    static bool SetVariable(const std::string& VariableName, const std::string& Value);
    static void RemoveVariable(const std::string& VariableName);
    static void ClearAllVariables();

    static bool RunCommand(const ConsoleCommand& Command);

private:
    typedef std::string ConVar_Name;
    static std::unordered_map<ConVar_Name, ConsoleVariable> _console_variables;

    static int assert_variable(const std::string& FunctionName, const std::string& VariableName, const unsigned int VariableType);

    static constexpr int NO_ERR              =  0;
    static constexpr int ERR_VAR_NONEXISTANT = -1;
    static constexpr int ERR_VAR_TYPE_WRONG  = -2;
};

#endif // CONSOLE_H
