#ifndef THING_DATA_H
#define THING_DATA_H

#include "fwd/things.hpp"
#include "theatre/parser/thing_variable.hpp"
#include "theatre/parser/number_parser.hpp"
#include "theatre/variable_registry.hpp"
#include "core/concepts.hpp"
#include "thing/types.hpp"

constexpr const char* cChildVarName{"Child"};

struct ThingData
{
public:
    ThingData();
    ThingData(Sarg Name, Sarg TypeName);
    ThingData(Sarg Name, Farg<PID> Type, Farg<std::vector<ThingVar>> Variables = {});
    ThingData(Sarg Name, Farg<PID> Type, ID UID, Farg<std::vector<ThingVar>> Variables = {});

    ID uid{};
    std::string name{"Untitled Thing"};
    std::vector<ThingVar> variables{};
    std::vector<ThingVar> children{};

    bool RemoveVariable(Sarg VariableName);
    ThingVar& AddVariable(Sarg Name, Sarg ParsedValue, ThingVar::Type Type, Sarg ParsedChildType = "");
    ThingVar& AddVariable(Farg<ThingVar> Value, Sarg Name);

    bool GetChildren(relatives_t& output) const;
    void SetChildren(Farg<relatives_t> input);

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
    Farg<PID> type() const;
    bool set_type(Farg<PID> Type);
    void clear();

private:
    PID type_{ThingType::Thing};

    typedef std::vector<ThingVar>::const_iterator VarIter_t;
    VarIter_t AssertVariable(Sarg VarName, ThingVar::Type VarType) const;

    bool GetBoolean(bool& BooleanOutput, Sarg VariableName) const;
    bool GetString(std::string& StringOutput, Sarg VariableName) const;
};

template<>
struct std::formatter<ThingData> : std::formatter<std::string>
{
    auto format(Farg<ThingData> inData, std::format_context& ctx) const
    { return std::formatter<std::string>::format(inData.log(), ctx); }
};

#endif // THING_DATA_H
