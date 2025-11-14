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

const ID& Thing::uid() const
{ return mUID; }

const ID& Thing::type() const
{ return mType; }

const std::string& Thing::name() const
{ return mName; }

const char* const Thing::c_name() const
{ return mName.data(); }
