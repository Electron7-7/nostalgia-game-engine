#include "command_line.hpp"
#include "common/debugging.hpp"
std::unordered_map<std::string, ConsoleVariable> CommandLine::_console_variables = {};

bool CommandLine::VariableExists(const std::string& name)
{ return (_console_variables.contains(name)); }

std::string CommandLine::GetStringVariable(const std::string& name)
{
    if(assert_variable("CommandLine::GetStringVariable", name, ConsoleVariable::STRING) != NO_ERR)
        return "";

    return (_console_variables.at(name).Value());
}

int CommandLine::GetIntVariable(const std::string& name)
{
    if(assert_variable("CommandLine::GetIntVariable", name, ConsoleVariable::INT))
        return 0;

    std::string value_str = _console_variables.at(name).Value();
    int return_value = 0;

    try
    {
        return_value = std::stoi(value_str);
    }
    catch (std::invalid_argument const& exception)
    {
        PRINTERR("CommandLine::GetIntVariable - Variable value unable to be converted from string");
        return_value = 0; // Just in case...
    }

    return return_value;
}

float CommandLine::GetFloatVariable(const std::string& name)
{
    if(assert_variable("CommandLine::GetFloatVariable", name, ConsoleVariable::FLOAT))
        return 0.0f;

    std::string value_str = _console_variables.at(name).Value();
    float return_value = 0.0f;

    try
    {
        return_value = std::stof(value_str);
    }
    catch (std::invalid_argument const& exception)
    {
        PRINTERR("CommandLine::GetFloatVariable - Variable value unable to be converted from string");
        return_value = 0.0f; // Just in case...
    }

    return return_value;
}

bool CommandLine::GetBoolVariable(const std::string& name)
{
    if(assert_variable("CommandLine::GetBoolVariable", name, ConsoleVariable::BOOLEAN))
        return 0;

    std::string value_str = _console_variables.at(name).Value();
    return !value_str.compare("1"); // If '0' or any other mangled data, will return false
}

void CommandLine::AddVariable(const ConsoleVariable& variable)
{ _console_variables[variable.Name()] = variable; }

bool CommandLine::SetVariable(const std::string& variable_name, const std::string& value)
{
    if(!VariableExists(variable_name))
    {
        PRINTWARN("CommandLine::SetVariable - variable doesn't exist!");
        return false;
    }

    _console_variables.at(variable_name).SetValue(value);
    return true;
}

void CommandLine::RemoveVariable(const std::string &variable_name)
{
    if(!VariableExists(variable_name))
    {
        PRINTWARN("CommandLine::RemoveVariable - variable already removed!");
        return;
    }

    _console_variables.erase(variable_name);
}

void CommandLine::ClearAllVariables()
{ _console_variables.clear(); }

bool CommandLine::RunCommand(const ConsoleCommand& command)
{
    if(command == Commands::_NULL)
    {
        PRINTWARN("CommandLine::RunCommand - command is _NULL!");
        return false;
    }

    switch(command.UID())
    {
    case Commands::START_JUMP:
        if(!SetVariable("+jump", "1"))
            AddVariable(ConVar_bool("+jump", true));
        break;
    case Commands::STOP_JUMP:
        if(!SetVariable("+jump", "0"))
            AddVariable(ConVar_bool("+jump", false));
        break;
    default:
        PRINTWARN("CommandLine::RunCommand - command not caught by switch statement!");
        return false;
    }

    return true;
}

int CommandLine::assert_variable(const std::string& function_name, const std::string& variable_name, const unsigned int variable_type)
{
    if(!VariableExists(variable_name))
    {
        PRINTWARN(function_name + " - requested a variable that does not exist!");
        return ERR_VAR_NONEXISTANT;
    }

    if(_console_variables.at(variable_name).Type() != variable_type)
    {
        PRINTWARN(function_name + " - requested a variable of a certain type, but that variable is of a different type!");
        return ERR_VAR_TYPE_WRONG;
    }

    return NO_ERR;
}
