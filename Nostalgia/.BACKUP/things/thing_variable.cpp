#include "./thing_variable.hpp"

bool TheatreFile::ThingVariable::invalid() const noexcept
{ return name.empty(); }
