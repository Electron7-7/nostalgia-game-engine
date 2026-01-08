#include "thinker.hpp"

void Thinker::Ready()
{}

void Thinker::SetVariables(Farg<ThingData> inData)
{ Thing::SetVariables(inData); }

Shared<ThingData> Thinker::GetVariables() const
{
    return Thing::GetVariables();
}
