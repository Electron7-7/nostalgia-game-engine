#include "parser.hpp"
#include "variable_registry.hpp"
#include "parser/number_parser.hpp"
#include "theatre/things/thinkers/thinker.hpp"
#include <math/glm_format.hpp> // used when converting GLM variables to strings

using namespace TheatreFile;

template<typename T> requires ThingVariable_t<T>
    static Error s_InterpretVariable(T& outVariable, Farg<ThingVariable> inVariable)
    {
        if constexpr(StringType<T>)
        {
            if(inVariable.value.empty()) { return ERR_EMPTY; }
            outVariable = inVariable.value;
            return OK;
        }
        else if constexpr(Bool<T>)
        {
            if(!inVariable.value.compare("false"))
                { outVariable = false; }
            else if(!inVariable.value.compare("true"))
                { outVariable = true; }
            else
                { return ERR_INVALID; }
            return OK;
        }
        else if constexpr(Number<T> or GLMContainer<T>)
        {
            if(!StringToNum(outVariable, inVariable.value))
                { return ERR_INVALID; }
            return OK;
        }
        else if constexpr(IsEnum<T>)
        {
            if(!g_pVariableRegistry->try_GetEnum(inVariable.value, outVariable))
                { return ERR_INVALID; }
            return OK;
        }
        else if constexpr(ID_t<T>)
        {
            if(!g_pVariableRegistry->try_GetID(inVariable.value, outVariable))
                { return ERR_INVALID; }
            return OK;
        }
        else
            { return ERR_INVALID_TYPE; }
    }

template<typename T> requires ThingVariable_t<T>
    static constexpr bool s_DoVariablesMatch(Farg<T> inVariable, ThingVarType inType)
    {
        switch(inType)
        {
        case ThingVarType::String:
            return StringType<T>;
        case ThingVarType::Bool:
            return Bool<T>;
        case ThingVarType::Number:
            return Number<T> or GLMContainer<T>;
        case ThingVarType::Enum:
            return IsEnum<T>;
        case ThingVarType::Child:
            [[fallthrough]];
        case ThingVarType::Parent:
            return std::same_as<T, ThinkerRelative>;
        case ThingVarType::ID:
            return ID_t<T>;
        case ThingVarType::None:
            return false;
        }
    }

template<typename T, StringType... Names> requires ThingVariable_t<T>
    Error ThingData::set_variable(Farg<T> inValue, Sarg inName)
    {
        ThingVarType type{ThingVarType::None};
        if constexpr(StringType<T>)
            { type = ThingVarType::String; }
        else if constexpr(Bool<T>)
            { type = ThingVarType::Bool; }
        else if constexpr(Number<T> or GLMContainer<T>)
            { type = ThingVarType::Number; }
        else if constexpr(IsEnum<T>)
        {
            std::string enum_name{};
            if(!g_pVariableRegistry->try_GetEnumName(inValue, enum_name))
                { return ERR_INVALID; }
            variables.emplace_back(inName, enum_name, ThingVarType::Enum);
            return OK;
        }
        else if constexpr(ID_t<T>)
        {
            std::string thing_name{};
            if(!g_pVariableRegistry->try_GetIDName(inValue, thing_name))
                { return ERR_INVALID_ID; }
            variables.emplace_back(inName, thing_name, ThingVarType::ID);
            return OK;
        }
        else { return ERR_INVALID_TYPE; }
        variables.emplace_back(inName, std::format("{}", inValue), type);
        return OK;
    }

template<typename T, StringType... Names> requires ThingVariable_t<T>
    Error ThingData::get_variable(T& outValue, Names... inNames)
    {
        ThingVariable thing_var{};
        for(FAUTO name : {inNames...})
        {
            for(FAUTO var : variables)
            {
                if(var.name.compare(name))
                    { continue; }
                thing_var = var;
                break;
            }
            if(!thing_var.invalid()) { break; }
        }
        if(thing_var.invalid()) { return ERR_NOT_FOUND; }
        else if(!s_DoVariablesMatch(outValue, thing_var.type))
            { return ERR_MISMATCHED_TYPES; }
        return s_InterpretVariable(outValue, thing_var);
    }

template<StringType... Names>
    int ThingData::erase_variables(Names... inNames)
    {
        int erase_value{0};
        for(FAUTO name : {inNames...})
        {
            for(auto it{variables.begin()}; it != variables.end();)
            {
                if(!it->name.compare(name))
                {
                    it = variables.erase(it);
                    ++erase_value;
                    continue;
                }
                ++it;
            }
        }
        return erase_value;
    }

int ThingData::get_children(ThinkerChildren& outChildren)
{
    int child_count{0};
    for(FAUTO var : variables)
    {
        uint child_id{};
        if(var.type != ThingVarType::Child
            or !g_pVariableRegistry->try_GetID(var.value, child_id))
            { continue; }
        outChildren.emplace_back(child_id, var.name);
        ++child_count;
    }
    return child_count;
}

int ThingData::set_children(Farg<ThinkerChildren> inChildren)
{
    int child_count{0};
    for(FAUTO child : inChildren)
    {
        std::string child_name{};
        if(child.id.invalid()
            or !g_pVariableRegistry->try_GetIDName(child.id[], child_name))
            { continue; }
        variables.emplace_back(child.type.name(), child_name, ThingVarType::Child);
        ++child_count;
    }
    return child_count;
}

Error ThingData::get_parent(ThinkerRelative& outParent)
{
    uint parent_id{};
    for(FAUTO var : variables)
    {
        if(var.type == ThingVarType::Parent)
        {
            if(!g_pVariableRegistry->try_GetID(var.value, parent_id))
                { return ERR_INVALID_ID; }
            outParent = {parent_id, var.name};
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Error ThingData::set_parent(Farg<ThinkerRelative> inParent)
{
    std::string parent_name{};
    if(inParent.id.invalid()
        or !g_pVariableRegistry->try_GetIDName(inParent.id[], parent_name))
        { return ERR_INVALID_ID; }
    variables.emplace_back(inParent.type.name(), parent_name, ThingVarType::Parent);
    return OK;
}
