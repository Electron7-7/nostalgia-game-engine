#include "thing.hpp"
#include "theatre_parser/thing_data.hpp"

Thing::~Thing()
{ UniqueIDs::Erase(mUID); }

void Thing::SetupVariables(const ThingData& data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
}

ID Thing::uid() const
{ return mUID; }

ID Thing::type() const
{ return mType; }

const std::string& Thing::name() const
{ return mName; }
