#ifndef THING_DATA_H
#define THING_DATA_H

#include "thing_variable.hpp"
#include "string_to_num.hpp"
#include "safe_return.hpp"
#include "things/types.hpp"

struct ThingData
{
public:
    static const ThingData PlayerDefaults;

    ThingData();
    ThingData(const std::string& Name, const std::string& TypeName);
    ThingData(const std::string& Name, ID Type, ID, const std::vector<ThingVar>& Variables = {});

    std::string name = "Untitled Thing";
    ID uid = ID::Invalid;
    std::vector<ThingVar> variables = {};

    bool RemoveVariable(const std::string& VariableName);
    void AddVariable(const std::string& Name, const std::string& ValueAsString, const penum_t& Type);
    void AddVariable(const ThingVar& Value, const std::string& Name);
    template<IsNumber T>
        void AddVariable(const T& Value, const std::string& Name)
        { AddVariable(ThingVar{Value}, Name); }

    template<typename T, class... Names>
        requires(... && (std::is_same_v<Names, std::string> || std::is_same_v<Names, const char*>))
    bool GetVariable(T& output, Names... names) const
    {
        if constexpr(std::is_same_v<std::string, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetString(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_same_v<std::decay_t<ID>, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetReference(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_same_v<std::decay_t<penum_t>, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetPrettyEnum(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_same_v<bool, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetBoolean(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_arithmetic_v<T> || GLMContainer<T>)
        {
            for(const auto& name : {names...})
            {
                if(auto assert_var = AssertVariable(name, ThingVar::eNumber);
                    SafeStatus::Check(assert_var.Status()) &&
                    StringToNum<T>(output, assert_var.Data()->value))
                { return true; }
            }
            return false;
        }
    }

    std::string log(bool colored = false, bool indent_items = false) const;
    ID   type() const;
    bool set_type(const std::string& TypeName);
    bool set_type(ID Type);
    void clear();

private:
    ID type_ = ThingType::Thing;

    typedef std::vector<ThingVar>::const_iterator VarIter_t;
    SafeReturn<VarIter_t> AssertVariable(const std::string& VarName, const penum_t& VarType) const;

    bool GetReference(ID& ReferenceOutput, const std::string& VariableName) const;
    bool GetPrettyEnum(penum_t& ReferenceOutput, const std::string& VariableName) const;
    bool GetBoolean(bool& BooleanOutput, const std::string& VariableName) const;
    bool GetString(std::string& StringOutput, const std::string& VariableName) const;
};

template<>
struct std::formatter<ThingData> : std::formatter<std::string>
{
    auto format(const ThingData& data, std::format_context& ctx) const
    { return std::formatter<std::string>::format(data.log(), ctx); }
};

#endif // THING_DATA_H
