#include "device.hpp"
#include "theatre/parser/thing_data.hpp"

void Device::SetVariables(Farg<ThingData> data)
{ Thing::SetVariables(data); }

Shared<ThingData> Device::GetVariables() const
{ return Thing::GetVariables(); }
