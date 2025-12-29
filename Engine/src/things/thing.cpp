#include "thing.hpp"
#include "theatre/parser/thing_data.hpp"
#include "core/uid.hpp"

Thing::Thing() noexcept {}

Thing::~Thing() noexcept {}

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
}

Shared<ThingData> Thing::GetVariables() const
{ return MakeShared<ThingData>(mName, mType, mUID); }

ID Thing::uid() const
{ return mUID; }

Farg<TTID> Thing::type() const
{ return mType; }

Farg<std::string> Thing::name() const
{ return mName; }

const char* const Thing::c_name() const
{ return mName.data(); }
