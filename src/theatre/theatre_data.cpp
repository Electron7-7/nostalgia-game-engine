#include "theatre_data.hpp"
#include "string_to_num.hpp"
#include "types/typenames.hpp"

// Variable

const variable_t variable_t::undefined = variable_t();

variable_t::variable_t() = default;

variable_t::variable_t(const std::string& name, const std::string& value, const VariableType& type)
: m_Name(name), m_Value(value), m_Type(type) {}

variable_t::variable_t(const std::string& name)
: variable_t(name, "", VariableType::Default)
{}

const std::string& variable_t::Name() const
{ return m_Name; }

const std::string& variable_t::Value() const
{ return m_Value; }

const VariableType& variable_t::Type() const
{ return m_Type; }

void variable_t::clear()
{ *this = variable_t(); }

// Data

data_t::data_t(const std::string& name, const std::string& type_name)
: m_Name(name), m_TypeName(type_name), m_Type(ConstexprHash(type_name)), m_Hash(ConstexprHash(m_Name + m_TypeName))
{}

data_t::data_t() = default;

short data_t::AddVariable(const std::string& name, const std::string& data, const VariableType& type)
{
    auto variable = std::find(m_Variables.begin(), m_Variables.end(), name);
    if(variable != m_Variables.end())
    {
        variable->m_Name  = name;
        variable->m_Value = data;
        variable->m_Type  = type;
    }

    m_Variables.emplace_back(name, data, type);
    return (variable != m_Variables.end());
}

void data_t::UpdateTheatreReferences(const std::map<std::string, unsigned int>& ids)
{
    for(variable_t& variable : m_Variables)
    {
        if(variable.m_Type != VariableType::TheatreRef)
            { continue; }
        if(ids.contains(variable.m_Value))
            { variable.m_Value = std::to_string(ids.at(variable.m_Value)); }
    }
}

const std::string& data_t::GetName() const
{ return m_Name; }

void data_t::SetName(const std::string& name)
{
    m_Name = name;
    m_Hash = ConstexprHash(m_Name + m_TypeName);
}

const std::string& data_t::GetTypeName() const
{ return m_TypeName; }

size_t data_t::GetType() const
{ return m_Type; }

void data_t::SetType(const std::string& type)
{
    m_TypeName = type;
    m_Type = ConstexprHash(m_TypeName);
    m_Hash = ConstexprHash(m_Name + m_TypeName);

    if(GetBaseType(m_Type) == BaseType::Invalid)
        { PRINT_WARNING("data_t::SetType - The specified type '{}' is not a known type! This data structure will not be used if its type name is invalid (meaning, you won't see '{}' in the Theatre)", type, m_Name) }
}

void data_t::clear()
{
    m_Variables.clear();
    m_Name = "Untitled";
    m_Type = Type::Invalid;
    m_TypeName = "Invalid";
}

#define EARLY_RETURN_MACRO(VAR_NAME) \
auto VAR_NAME = std::find(m_Variables.begin(), m_Variables.end(), variable_name); \
if(VAR_NAME == m_Variables.end() || variable->m_Value.empty()) \
    { return false; }

bool data_t::GetTheatreRef(unsigned int& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    return StringToNum<unsigned int>(real_variable, variable->m_Value);
}

bool data_t::GetBool(bool& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    if(!variable->m_Value.compare("true"))
    {
        real_variable = true;
        return true;
    }
    else if(!variable->m_Value.compare("false"))
    {
        real_variable = false;
        return true;
    }
    return false;
}

bool data_t::GetString(std::string& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    if(variable->m_Value.empty())
        { return false; }
    real_variable = variable->m_Value;
    return true;
}

// Theatre Data

const TheatreData TheatreData::Missing;

const std::vector<data_t>& TheatreData::GetData() const
{ return m_Data; }

SafeStatus TheatreData::AddData(const data_t& data)
{
    // switch(GetBaseType(data.GetType()))
    // {
    // case BaseType::Invalid:
    //     return Status::TheatreDataINVALID_TYPE;
    // case BaseType::Resource:
    //     m_Resources.push_back(data);
    //     break;
    // case BaseType::Thing:
    //     m_Things.push_back(data);
    //     break;
    // }
    if(GetBaseType(data.GetType()) == BaseType::Invalid)
        { return Status::TheatreDataINVALID_TYPE; }
    m_Data.push_back(data);
    return Status::NO_ERR;
}

void TheatreData::clear()
{ m_Data.clear(); }

#ifdef DEBUGGING
void TheatreData::debug_PrintData()
{
    PRINT_DEBUG("Parsed Theatre Data Printout:")

    for(const auto& data : m_Data)
    {
        std::print("({}) {}:\n", StringifyType(data.m_Type), data.m_Name);

        const std::vector<variable_t>& variables = data.m_Variables;
        for(const variable_t& variable : variables)
        { std::print("\t({}) {} = {}\n", StringifyEnum(variable.m_Type), variable.m_Name, variable.m_Value); }

        std::print("\n");
    }
}
#endif // DEBUGGING
