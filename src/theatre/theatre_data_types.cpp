#include "theatre_data.hpp"
#include "debug.hpp"
#include "data_t.hpp"
#include "variable_t.hpp"
#include "filesystem/file_data.hpp"
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
: m_Name(name), m_TypeName(type_name), m_Type(ConstexprHash(type_name)), m_AssignedID(id)
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
}

const std::string& data_t::GetTypeName() const
{ return m_TypeName; }

size_t data_t::GetType() const
{ return m_Type; }

void data_t::SetType(const std::string& type)
{
    m_TypeName = type;
    m_Type = ConstexprHash(m_TypeName);
    m_Priority = g_GetPriority(m_Type);
    PRINT_DEBUG("({}) {} #{}", m_Name, m_TypeName, m_Priority)

    if(!g_IsValidType(m_Type))
        { PRINT_WARNING("data_t::SetType - The specified type '{}' is not a known type! This data structure will not be used if its type name is invalid (meaning, you won't see '{}' in the Theatre)", type, m_Name) }
}

int data_t::GetPriority() const
{ return m_Priority; }

void data_t::SetPriority(int priority)
{ m_Priority = priority; }

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

bool data_t::GetFileData(std::shared_ptr<const FileData> real_variable, const std::string& variable_name) const
{
    EARLY_RETURN_MACRO(variable)
    return(ResourceData::GetData(real_variable, variable->m_Value));
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
// QuickSort functions heavily plagiarized from: https://www.w3schools.com/dsa/dsa_algo_quicksort.php
static int s_DataPartition(std::vector<data_t>& array, int low, int high)
{
    int pivot = array.at(high).GetPriority();
    int i = (low - 1);

    for(int j = low; j < high; ++j)
    {
        if(array.at(j).GetPriority() <= pivot)
        {
            ++i;
            data_t temp = array.at(i);
            array.at(i) = array.at(j);
            array.at(j) = temp;
        }
    }

    data_t temp = array.at(i + 1);
    array.at(i + 1) = array.at(high);
    array.at(high) = temp;
    return (i + 1);
}

static void s_DataQuickSort(std::vector<data_t>& array, int low, int high)
{
    if(low < high)
    {
        int pivot_index = s_DataPartition(array, low, high);
        s_DataQuickSort(array, low, pivot_index - 1);
        s_DataQuickSort(array, pivot_index + 1, high);
    }
}

void TheatreData::OrderByPriority()
{
    DEBUG(
        std::print("Unsorted:\n");
        for(size_t i = 0 ; i < m_Data.size() ; ++i)
        {
            std::print("{}", m_Data.at(i).GetPriority());
            if(i+1 < m_Data.size())
                { std::print(", "); }
        }
        std::print("\n");
    )
    s_DataQuickSort(m_Data, 0, m_Data.size() - 1);
    DEBUG(
        std::print("Data Priority Sorting Check:\n");
        for(size_t i = 0 ; i < m_Data.size() ; ++i)
        {
            std::print("{}", m_Data.at(i).GetPriority());
            if(i+1 < m_Data.size())
                { std::print(", "); }
        }
        std::print("\n");
    )
}

SafeStatus TheatreData::AddData(const data_t& data)
{
    if(!g_IsValidType(data.GetType()))
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
