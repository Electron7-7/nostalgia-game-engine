#include "thing.hpp"
#include "theatre/thing_factory.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

Thing::Thing()  noexcept = default;
Thing::~Thing() noexcept = default;

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type;
    mStartingData = MakeUnique<ThingData>(data);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>()};
    data->name = mName;
    data->uid  = mUID;
    data->type = mType;
    data->theatre_registry = mStartingData->theatre_registry;
    return data;
}

ThingData Thing::GetStartingVariables() const
{ return *mStartingData; }

ID Thing::uid() const
{ return mUID; }

FPID Thing::type() const
{ return mType; }

Farg<ThingType> Thing::full_type() const
{ return ThingFactory::GetType(mType); }

Farg<std::string> Thing::name() const
{ return mName; }

void Thing::set_name(Sarg inName)
{ mName = inName; }

const char* const Thing::c_name() const
{ return mName.data(); }
