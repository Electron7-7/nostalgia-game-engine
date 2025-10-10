#include "thing.hpp"
#include "theatre_parser/thing_data.hpp"

Thing::~Thing()
{}

void Thing::SetVariables(const ThingData& data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
}

ThingData Thing::GetVariables() const
{ return ThingData{mName, mType, mUID}; }

ID Thing::uid() const
{ return mUID; }

ID Thing::type() const
{ return mType; }

const std::string& Thing::name() const
{ return mName; }
