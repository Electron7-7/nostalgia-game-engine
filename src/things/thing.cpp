#include "thing.hpp"

tid_t Thing::GetID() const
{ return m_ID; }

const std::string& Thing::GetName() const
{ return m_Name; }

size_t Thing::GetType() const
{ return m_Type; }
