#include "theatre_data.hpp"
#include "string_to_num.hpp"
#include "types/typenames.hpp"
#include "things/resources/resource_data.hpp"

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

data_t::data_t(const std::string& name, const std::string& type_name, id_t id)
: m_Name(name), m_TypeName(type_name), m_Type(ConstexprHash(type_name)), m_Hash(ConstexprHash(m_Name + m_TypeName)), m_AssignedID(id)
{}

data_t::data_t() = default;

void data_t::AddVariable(const std::string& name, const std::string& data, const VariableType& type)
{
    auto variable = std::find(m_Variables.begin(), m_Variables.end(), name);
    if(variable != m_Variables.end())
    {
        variable->m_Name  = name;
        variable->m_Value = data;
        variable->m_Type  = type;
    }
    m_Variables.emplace_back(name, data, type);
}

id_t data_t::GetID() const
{ return m_AssignedID; }

void data_t::SetID(id_t id)
{ m_AssignedID = id; }

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

    if(GetBaseType(m_Type) == Type::Invalid)
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
const auto& VAR_NAME = std::find(m_Variables.begin(), m_Variables.end(), variable_name); \
if(VAR_NAME == m_Variables.end() || variable->m_Value.empty()) \
    { return false; }

bool data_t::GetTheatreRef(unsigned int& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    return StringToNum<unsigned int>(real_variable, variable->m_Value);
}

bool data_t::GetEngineRef(BinaryFileData& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    SafeReturn<const BinaryFileData&> reference = ResourceData::try_GetBinaryData(variable->m_Value);
    if(!SafeStatus::PrintCheck(reference.Status()))
        { return false; }

    real_variable = reference.Data();
    return true;
}

bool data_t::GetEngineRef(StringFileData& real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    SafeReturn<const StringFileData&> reference = ResourceData::try_GetStringData(variable->m_Value);
    if(!SafeStatus::PrintCheck(reference.Status()))
        { return false; }

    real_variable = reference.Data();
    return true;
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

void TheatreData::UpdateTheatreReferences(const std::map<std::string, std::string>& ids)
{
    for(data_t& data : m_Data)
    {
        for(variable_t& variable : data.m_Variables)
        {
            if(variable.m_Type != VariableType::TheatreRef)
                { continue; }
            if(ids.contains(variable.m_Value))
                { variable.m_Value = ids.at(variable.m_Value); }
        }
    }
}

SafeStatus TheatreData::AddData(const data_t& data)
{
    if(GetBaseType(data.GetType()) == Type::Invalid)
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
