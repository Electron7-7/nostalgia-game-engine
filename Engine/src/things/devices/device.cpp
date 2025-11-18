#include "device.hpp"
#include "theatre/parser/thing_data.hpp"

void Device::SetVariables(const ThingData& data)
{ Thing::SetVariables(data); }

ThingData Device::GetVariables() const
{ return Thing::GetVariables(); }
