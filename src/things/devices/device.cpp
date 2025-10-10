#include "device.hpp"
#include "theatre_parser/thing_data.hpp"

void Device::SetVariables(const ThingData& data)
{ Thing::SetVariables(data); }

ThingData Device::GetVariables() const
{ return Thing::GetVariables(); }
