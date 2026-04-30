#include "./console.hpp"
#include "thirdparty/frozen/set.h"

#define LOCK_CMDS LockGuard<RMutex> _cmds_lock{sCommandsMutex};
#define LOCK_HIST LockGuard<RMutex> _hist_lock{sHistoryMutex };

using namespace Console;

struct ConsoleToken
{
    enum Type : int
    { VARIABLE, INT_DEF, BOOL_DEF, FLOAT_DEF, STR_DEF, NONE };

    std::string token{""};
    Type type{NONE};
};

static RMutex sCommandsMutex{}, sHistoryMutex{};
static bool sInitialized{false};
static const Variable sEmptyVariable{""};
static constexpr frozen::set<char, 4> sWhitespace{' ', '\t', '\n', '\r'};

static uint        sHistoryMaxSize{4815}; // 162342
static History_t   sHistory{};
static Commands_t  sCommands{};
static Variables_t sVariables{};
static Callbacks_t sCallbacks{};

void Console::Init()
{
    if(sInitialized)
        { return; }

    SetVariable("Theatre.draw_3d", true);
    SetVariable("Theatre.draw_2d", true);
    SetVariable("Theatre.draw_text_new", false);
    SetVariable("Theatre.debug_save_msgs", false);
    SetVariable("Theatre.debug_create_thing_msgs", false);
    SetVariable("TheatreFile.Parser.print_declarations", false);
    SetVariable("TheatreFile.Parser.print_name_type_map", false);
    SetVariable("Theatre.debug_callsheet_msgs", false);
    SetVariable("ThingFactory.debug_register_msgs", false);
    SetVariable("ResourceDatabase.debug_register_msgs", false);
    SetVariable("Collider3D.debug_collision_msgs", false);
    SetVariable("Collider3D.debug_collision_persisted_msgs", false);
    SetVariable("nodraw_3d", false);
    SetVariable("nodraw_3d_foreground", false);
    SetVariable("nodraw_3d_background", false);
    AddCommand("noclip");

    sInitialized = true;
}

void Console::AddCallback(ConsoleCommandCallback_f inCallback)
{
    LOCK_CMDS
    sCallbacks.push_back(inCallback);
}

void Console::RemoveCallback(ConsoleCommandCallback_f inCallback)
{
    LOCK_CMDS
    if(auto found_it{std::find(sCallbacks.begin(), sCallbacks.end(), inCallback)}; found_it != sCallbacks.end())
        { sCallbacks.erase(found_it); }
}

Error Console::ProcessLine(Sarg inInput)
{
    {
        LOCK_HIST
        if(sHistory.size() == sHistoryMaxSize)
            { sHistory.pop_back(); }
        sHistory.push_back(inInput);
    }

    std::string buffer{}, variable_name{};
    std::vector<ConsoleToken> shitty_tokens{};
    bool _in_string{false};
    for(char character : inInput)
    {
        if(character == '"' or character == '\'')
        {
            _in_string = not _in_string;
            if(not _in_string)
            {
                shitty_tokens.emplace_back(buffer, ConsoleToken::STR_DEF);
                buffer.clear();
            }
            continue;
        }
        else if(character == '=' and not _in_string)
        {
            shitty_tokens.emplace_back(buffer, ConsoleToken::VARIABLE);
            variable_name = buffer;
            buffer.clear();
            continue;
        }
        else if((sWhitespace.contains(character) and not _in_string) or character == inInput.back())
        {
            if(buffer.empty() and character == inInput.back() and not sWhitespace.contains(character))
                { buffer += character; }
            if(not variable_name.empty() and not buffer.empty())
            {
                if(IsInt(buffer))
                    { shitty_tokens.emplace_back(buffer, ConsoleToken::INT_DEF); }
                else if(IsFloat(buffer))
                    { shitty_tokens.emplace_back(buffer, ConsoleToken::FLOAT_DEF); }
                else if(IsBool(buffer))
                    { shitty_tokens.emplace_back(GetLowercase(buffer), ConsoleToken::BOOL_DEF); }
                else
                    { shitty_tokens.emplace_back(buffer, ConsoleToken::STR_DEF); }
                buffer.clear();
                variable_name.clear();
            }
            continue;
        }
        buffer += character;
    }
    std::string _variable_name{};
    float _f_value{};
    int   _i_value{};
    for(FAUTO token : shitty_tokens)
    {
        if(sCommands.contains(token.token))
        {
            LOCK_CMDS
            for(FAUTO callback : sCallbacks)
                { callback(token.token); }
            continue;
        }
        switch(token.type)
        {
        case ConsoleToken::VARIABLE:
            _variable_name = token.token;
            [[fallthrough]];
        case ConsoleToken::NONE:
            continue;
        case ConsoleToken::FLOAT_DEF:
            if(StringToNum(_f_value, token.token))
                { SetVariable(_variable_name, _f_value); }
            break;
        case ConsoleToken::INT_DEF:
            if(StringToNum(_i_value, token.token))
                { SetVariable(_variable_name, _i_value); }
            break;
        case ConsoleToken::BOOL_DEF:
            SetVariable(_variable_name, static_cast<int>(token.token == "true"));
            break;
        case ConsoleToken::STR_DEF:
            SetVariable(_variable_name, token.token);
            break;
        }
        _variable_name.clear();
    }
    return FAILED;
}

History_t Console::GetHistory(uint inFrom)
{
    LOCK_HIST
    if(inFrom > sHistory.size() or inFrom > sHistoryMaxSize)
        { inFrom = sHistoryMaxSize - 1; }
    return {sHistory.begin() + inFrom, sHistory.end()};
}

void Console::ClearHistory(uint inFrom)
{
    LOCK_HIST
    if(inFrom > sHistory.size() or inFrom > sHistoryMaxSize)
        { inFrom = sHistory.size() - 1; }
    sHistory.erase(sHistory.begin() + inFrom, sHistory.end());
}

std::string Console::GetLine(uint inAt)
{
    LOCK_HIST
    if(inAt > sHistory.size() or inAt > sHistoryMaxSize)
        { inAt = sHistory.size() - 1; }
    return sHistory[inAt];
}

void Console::SetHistoryMaxSize(uint inSize)
{
    LOCK_HIST
    sHistory.resize(sHistoryMaxSize = inSize);
}

uint Console::GetHistoryMaxSize()
{
    LOCK_HIST
    return sHistoryMaxSize;
}

Farg<Variable> Console::GetVariable(Sarg inName)
{
    if(auto found_it{sVariables.find(inName)}; found_it != sVariables.end())
        { return *found_it; }
    return sEmptyVariable;
}

void Console::SetVariable(Sarg inVariableName, Farg<int> inValue)
{
    sVariables.erase(inVariableName);
    sVariables.emplace(inVariableName, inValue);
}

void Console::SetVariable(Sarg inVariableName, Farg<bool> inValue)
{
    sVariables.erase(inVariableName);
    sVariables.emplace(inVariableName, inValue);
}

void Console::SetVariable(Sarg inVariableName, Farg<float> inValue)
{
    sVariables.erase(inVariableName);
    sVariables.emplace(inVariableName, inValue);
}

void Console::SetVariable(Sarg inVariableName, Sarg inValue)
{
    sVariables.erase(inVariableName);
    sVariables.emplace(inVariableName, inValue);
}

Error Console::RemoveVariable(Sarg inName)
{
    if(auto found_it{sVariables.find(inName)}; found_it != sVariables.end())
    {
        sVariables.erase(inName);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Farg<Variables_t> Console::GetAllVariables()
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

Farg<Commands_t> Console::GetAllCommands()
{ return sCommands; }
