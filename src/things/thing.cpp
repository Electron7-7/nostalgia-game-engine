#include "thing.hpp"

id_t _thing::GetID() const
{ return m_ID; }

const std::string& _thing::GetName() const
{ return m_Name; }

size_t _thing::GetType() const
{ return m_Type; }
