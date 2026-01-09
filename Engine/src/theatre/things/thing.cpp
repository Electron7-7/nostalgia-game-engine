#include "thing.hpp"
#include "core/uid.hpp"
#include "theatre/parser/thing_data.hpp"
#include "theatre/thing_factory.hpp"

Thing::Thing()  noexcept = default;
Thing::~Thing() noexcept = default;

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
    mStartingData = MakeUnique<ThingData>(data);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>(mName, mType, mUID)};
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

const char* const Thing::c_name() const
{ return mName.data(); }

bool Thing::uid(ID inID)
{
    if(inID == mUID)
        { return true; }
    else if(inID.invalid())
        { inID = UID::Generate(); }
    else if(inID != UID::a_Player)
        { UID::Push(inID[]); }
    UID::Erase(mUID[]);
    mUID = inID;
    return true;
}

void Thing::name(Sarg inName)
{ mName = inName; }
