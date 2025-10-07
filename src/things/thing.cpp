#include "thing.hpp"
#include "theatre_parser/thing_data.hpp"

Thing::~Thing() = default;

void Thing::SetupVariables(const ThingData& data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
}

void Thing::Shutdown()
{ UniqueIDs::Erase(mUID); }

ID Thing::uid() const
{ return mUID; }

ID Thing::type() const
{ return mType; }

const std::string& Thing::name() const
{ return mName; }
