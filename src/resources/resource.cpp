#include "resource.hpp"

id_t _resource::GetID() const
{ return m_ID; }

const std::string& _resource::GetName() const
{ return m_Name; }

size_t _resource::GetType() const
{ return m_Type; }
