#include "thing.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"

Thing::Thing()  noexcept = default;
Thing::~Thing() noexcept = default;

void Thing::Free()
{ g_pTheatreManager->DestroyThing(mUID); }

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
