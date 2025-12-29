#ifndef THING_DATA_H
#define THING_DATA_H

#include "theatre/parser/thing_variable.hpp"
#include "theatre/parser/number_parser.hpp"
#include "theatre/variable_registry.hpp"
#include "things/types.hpp"
#include "core/concepts.hpp"

struct ThingData
{
public:
    static const ThingData PlayerDefaultData;

    ThingData();
    ThingData(Farg<std::string> Name, Farg<std::string> TypeName);
    ThingData(Farg<std::string> Name, Farg<TTID> Type, Farg<std::vector<ThingVar>> Variables = {});
    ThingData(Farg<std::string> Name, Farg<TTID> Type, ID UID, Farg<std::vector<ThingVar>> Variables = {});

    ID uid{};
    std::string name{"Untitled Thing"};
    std::vector<ThingVar> variables{};

    bool RemoveVariable(Farg<std::string> VariableName);
    ThingVar& AddVariable(Farg<std::string> Name, Farg<std::string> ParsedValue, ThingVar::Type Type);
    ThingVar& AddVariable(Farg<ThingVar> Value, Farg<std::string> Name);

    template<StringContainer T, StringType... Names>
        bool GetVariable(T& output, Names... names) const
        {
            for(FARG(auto) var_name : {names...})
                { if(GetString(output, var_name)) { return true; } }
            return false;
        }
    template<ID_t T, StringType... Names>
        bool GetVariable(T& output, Names... names) const
        {
            for(FARG(auto) var_name : {names...})
            {
                if(auto found_it{AssertVariable(var_name, ThingVar::Type::Reference)};
                    found_it != variables.cend() && found_it->id_or_enum != ID::Invalid)
                {
                    output = T{static_cast<uint>(found_it->id_or_enum)};
                    return true;
                }
            }
            return false;
        }
    template<IsEnum T, StringType... Names>
        bool GetVariable(T& output, Names... names) const
        {
            for(FARG(auto) var_name : {names...})
            {
                if(auto found_it{AssertVariable(var_name, ThingVar::Type::Enum)}; found_it != variables.cend())
                    { return g_pVariableRegistry->try_GetEnum(found_it->value, output); }
            }
            return false;
        }
    template<StringType... Names>
        bool GetVariable(bool& output, Names... names) const
        {
            for(FARG(auto) var_name : {names...})
                { if(GetBoolean(output, var_name)) { return true; } }
            return false;
        }
    template<NumberOrGLM T, StringType... Names>
        bool GetVariable(T& output, Names... names) const
        {
            for(FARG(auto) var_name : {names...})
            {
                if(auto found_it{AssertVariable(var_name, ThingVar::Type::Number)};
                    found_it != variables.cend() && StringToNum<T>(output, found_it->value))
                { return true; }
            }
            return false;
        }

    std::string log(bool colored = false, bool indent_items = false) const;
    Farg<TTID> type() const;
    bool set_type(const std::string& TypeName);
    bool set_type(Farg<TTID> Type);
    void clear();

private:
    TTID type_{ThingType::Thing};

    typedef std::vector<ThingVar>::const_iterator VarIter_t;
    VarIter_t AssertVariable(Farg<std::string> VarName, ThingVar::Type VarType) const;

    bool GetBoolean(bool& BooleanOutput, Farg<std::string> VariableName) const;
    bool GetString(std::string& StringOutput, Farg<std::string> VariableName) const;
};

template<>
struct std::formatter<ThingData> : std::formatter<std::string>
{
    auto format(Farg<ThingData> inData, std::format_context& ctx) const
    { return std::formatter<std::string>::format(inData.log(), ctx); }
};

#endif // THING_DATA_H
