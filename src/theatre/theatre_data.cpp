#include "theatre_data.hpp"
#include "theatre_parsing_lookups.hpp"

// Variable

const variable_t variable_t::undefined = variable_t();

variable_t::variable_t() = default;

variable_t::variable_t(const std::string& name, const std::string& value, const VariableType& type)
: m_Name(name), m_Value(value), m_Type(type), m_Hash(g_StringHash(m_Name)) {}

variable_t::variable_t(const std::string& name)
: variable_t(name, "N/A", VariableType::Default)
{}

void variable_t::clear()
{ *this = variable_t(); }

// Data

data_t::data_t(const std::string& name)
: m_Name(name), m_Hash(g_StringHash(m_Name))
{}

data_t::data_t() = default;

short data_t::SetVariable(const std::string& name, const std::string& data, const VariableType& type)
{
    bool variable_exists = (m_Variables.find(name) != m_Variables.end());
    if(variable_exists)
        { m_Variables.erase(name); }

    m_Variables.emplace(name, data, type);
    return variable_exists;
}

/*SafeReturn<const variable_t&> data_t::try_GetVariable(const std::string& name)
{
    if(!m_Variables.contains(name))
    { return SafeReturn<const variable_t&>(variable_t::undefined, Status::DataTypeINVALID_VARIABLE_NAME); }

    if(m_Variables.find(name)->m_Value.empty())
    { return SafeReturn<const variable_t&>(variable_t::undefined, Status::DataTypeEMPTY_VARIABLE); }

    return SafeReturn<const variable_t&>(*m_Variables.find(name));
}*/

const std::string& data_t::GetName() const
{ return m_Name; }

void data_t::SetName(const std::string& name)
{
    m_Name = name;
    m_Hash = g_StringHash(m_Name);
}

const std::string& data_t::GetType() const
{ return m_Type; }

void data_t::SetType(const std::string& type)
{
    m_Type = type;
    m_Hash = g_StringHash(m_Name);
}

void data_t::clear()
{
    m_Variables.clear();
    m_Name = "Untitled";
    m_Type = "Unset";
}

// Theatre Data

data_t TheatreData::s_SafeDataReturn = data_t();

const std::set<data_t>& TheatreData::GetThings() const
{ return m_Things; }

const std::set<data_t>& TheatreData::GetResources() const
{ return m_Resources; }

#ifdef DEBUGGING
std::set<data_t> TheatreData::GetMergedData() const
{
    std::set<data_t> combined_sets(m_Things.cbegin(), m_Things.cend());
    combined_sets.insert(m_Resources.cbegin(), m_Resources.cend());
    return combined_sets;
}
#endif // DEBUGGING

SafeReturn<data_t> TheatreData::try_GetData(const std::string& name) const
{
    auto is_thing = m_Things.find(name);
    if(is_thing != m_Things.cend())
        { return *is_thing; }

    auto is_resource = m_Resources.find(name);
    if(is_resource != m_Resources.cend())
        { return *is_resource; }

    return SafeReturn(s_SafeDataReturn, Status::TheatreDataTypeINVALID_NAME);
}

SafeStatus TheatreData::AddData(const data_t& data)
{
    switch(GetBaseType(data.GetType()))
    {
    case BaseType::Unknown:
        return Status::TheatreDataTypeINVALID_NAME;
    case BaseType::Resource:
        m_Resources.insert(data);
        break;
    case BaseType::Thing:
        m_Things.insert(data);
        break;
    }
    return Status::NO_ERR;
}

void TheatreData::clear()
{
    m_Things.clear();
    m_Resources.clear();
}
