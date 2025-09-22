#ifndef VARIABLE_T_H
#define VARIABLE_T_H

#include <string>

enum class VariableType
{
    Nothing, // This is only used by the parser
    TheatreRef,
    EngineRef,
    Number,
    Bool,
    String,
    Default = String,
};

// This is for printouts
constexpr std::string StringifyEnum(const VariableType& VariableTypeEnum)
{
    switch(VariableTypeEnum)
    {
    case VariableType::TheatreRef:
        return "TheatreRef";
    case VariableType::EngineRef:
        return "EngineRef";
    case VariableType::Number:
        return "Number";
    case VariableType::Bool:
        return "Bool";
    case VariableType::String:
        return "String";
    default:
        return "Uhh... what?";
    }
}

struct variable_t
{
public:
    static const variable_t undefined;

    variable_t(const std::string& Name, const std::string& Value, const VariableType& Type);
    variable_t();

    const std::string& Name() const;
    const std::string& Value() const;
    const VariableType& Type() const;

    const bool operator==(const std::string& Compare) const
    { return !m_Name.compare(Compare); }

private:
    friend struct TheatreData;
    friend struct data_t;
    variable_t(const std::string& Name);

    std::string  m_Name  = "Untitled";
    std::string  m_Value = "";
    VariableType m_Type  = VariableType::Default;

    #pragma message("(TODO) Maybe make this public?")
    void clear();
};

#endif // VARIABLE_T_H
