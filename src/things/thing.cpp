#include "thing.hpp"
#include "theatre/data_t.hpp"

id_t _thing::GetID() const
{ return m_ID; }

const std::string& _thing::GetName() const
{ return m_Name; }

size_t _thing::GetType() const
{ return m_Type; }

void Thing::SetupVariables(const data_t& data)
{
    m_ID = data.GetID();
    m_Name = data.GetName();
    m_Type = data.GetType();
}
