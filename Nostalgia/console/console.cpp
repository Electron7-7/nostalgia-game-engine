#include "./console.hpp"
#include "thirdparty/frozen/set.h"

using namespace Console;

static ConsoleCommandCallback_f sCommandCallback{nullptr};
static Shared<Variable> sEmptyVariable{MakeShared<Variable>()};
static constexpr frozen::set<char, 4> sWhitespace{' ', '\t', '\n', '\r'};
static Commands
sCommands{
    "noclip",
};
static std::set<std::string>
sVariableNames{
    "Theatre.draw_3d",
    "Theatre.draw_2d",
    "Theatre.debug_save_msgs",
    "Theatre.draw_text_new",
    "Collider3D.debug_collision_msgs",
};
static Variables
sVariables{
    MakeShared<Variable>("Theatre.draw_3d", true),
    MakeShared<Variable>("Theatre.draw_2d", true),
    MakeShared<Variable>("Theatre.debug_save_msgs", false),
    MakeShared<Variable>("Theatre.draw_text_new", false),
    MakeShared<Variable>("Collider3D.debug_collision_msgs", false),
};

static Error sProcessEngineCommand(Sarg);

void Console::SetCommandCallback(ConsoleCommandCallback_f inCallback)
{ sCommandCallback = inCallback; }

Error Console::ProcessLine(Sarg inInput)
{
    std::string buffer{};
    std::vector<std::string> shitty_tokens{};
    for(char character : inInput)
    {
        if(sWhitespace.contains(character))
        {
            shitty_tokens.push_back(buffer);
            buffer.clear();
            continue;
        }
        buffer += character;
    }
    shitty_tokens.push_back(buffer);
    for(int i{0}; i < shitty_tokens.size(); ++i)
    {
        if(sVariableNames.contains(shitty_tokens[i]))
        {
            auto variable{try_GetVariable(shitty_tokens[i])};
            while(i < shitty_tokens.size() and shitty_tokens[i].compare("=")) { ++i; }
            if(++i < shitty_tokens.size())
            {
                std::string value{shitty_tokens[i]};
                switch(variable->type)
                {
                case Variable::STRING_TYPE:
                    variable->string_value = value;
                    break;
                case Variable::FLOAT_TYPE:
                    StringToNum(variable->float_value, value);
                    break;
                case Variable::INTEGER_TYPE:
                    StringToNum(variable->int_value, value);
                    break;
                }
            }
        }
        else if(sCommands.contains(shitty_tokens[i]))
        {
            sProcessEngineCommand(shitty_tokens[i]);
            if(sCommandCallback) { sCommandCallback(shitty_tokens[i]); }
            return OK;
        }
    }
    return FAILED;
}

Shared<Variable> Console::try_GetVariable(Sarg inName)
{
    if(sVariables.empty() or !sVariableNames.contains(inName))
        { return sEmptyVariable; }
    for(FAUTO var : sVariables)
        { if(!var->name.compare(inName)) { return var; } }
    return sEmptyVariable;
}

Error Console::GetVariable(Sarg inName, Shared<Variable>& outVariable)
{
    if(sVariables.empty())
        { return ERR_EMPTY; }
    else if(!sVariableNames.contains(inName))
        { return ERR_NOT_FOUND; }
    for(FAUTO var : sVariables)
    {
        if(!var->name.compare(inName))
        {
            outVariable = var;
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Error Console::SetVariable(Sarg inName, Sarg inValue)
{
    if(sVariables.empty())
        { return ERR_EMPTY; }
    else if(!sVariableNames.contains(inName))
        { return ERR_NOT_FOUND; }
    for(FAUTO var : sVariables)
        { if(!var->name.compare(inName)) { var->string_value = inValue; } }
    return OK;
}

Error Console::SetVariable(Sarg inName, int inValue)
{
    if(sVariables.empty())
        { return ERR_EMPTY; }
    else if(!sVariableNames.contains(inName))
        { return ERR_NOT_FOUND; }
    for(FAUTO var : sVariables)
        { if(!var->name.compare(inName)) { var->int_value = inValue; } }
    return OK;
}

Error Console::SetVariable(Sarg inName, float inValue)
{
    if(sVariables.empty())
        { return ERR_EMPTY; }
    else if(!sVariableNames.contains(inName))
        { return ERR_NOT_FOUND; }
    for(FAUTO var : sVariables)
        { if(!var->name.compare(inName)) { var->float_value = inValue; } }
    return OK;
}

Error Console::AddVariable(Sarg inName, Sarg inInitialValue)
{
    if(sVariableNames.contains(inName))
        { return ERR_ALREADY_EXISTS; }
    sVariableNames.insert(inName);
    sVariables.push_back(MakeShared<Variable>(inName, inInitialValue));
    return OK;
}

Error Console::AddVariable(Sarg inName, int inInitialValue)
{
    if(sVariableNames.contains(inName))
        { return ERR_ALREADY_EXISTS; }
    sVariableNames.insert(inName);
    sVariables.push_back(MakeShared<Variable>(inName, inInitialValue));
    return OK;
}

Error Console::AddVariable(Sarg inName, float inInitialValue)
{
    if(sVariableNames.contains(inName))
        { return ERR_ALREADY_EXISTS; }
    sVariableNames.insert(inName);
    sVariables.push_back(MakeShared<Variable>(inName, inInitialValue));
    return OK;
}

Error Console::RemoveVariable(Sarg inName)
{
    if(sVariables.empty())
        { return ERR_EMPTY; }
    else if(!sVariableNames.contains(inName))
        { return ERR_NOT_FOUND; }
    for(auto iter{sVariables.begin()}; iter != sVariables.end(); ++iter)
        { if(!iter->get()->name.compare(inName)) { sVariables.erase(iter); } }
    sVariableNames.erase(inName);
    return OK;
}

Farg<Variables> Console::GetVariables()
{ return sVariables; }

Error Console::AddCommand(Sarg inCommand)
{
    return (sCommands.insert(inCommand).second)
        ? OK
        : ERR_ALREADY_EXISTS;
}

Error Console::RemoveCommand(Sarg inCommand)
{
    return (sCommands.erase(inCommand))
        ? OK
        : (sCommands.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
}

Farg<Commands> Console::GetCommands()
{ return sCommands; }

Error sProcessEngineCommand(Sarg inCommand)
{
    return OK;
}
