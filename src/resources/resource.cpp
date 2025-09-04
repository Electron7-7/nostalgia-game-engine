#include "resource.hpp"

Resource Resource::Empty;

rid_t Resource::GetID() const
{ return m_ID; }

const std::string& Resource::GetName() const
{ return m_Name; }

size_t Resource::GetType() const
{ return m_Type; }
