#include "thing.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "core/uid.hpp"

Thing::Thing() noexcept: mChildren{MakeShared<ThingChildren>()} {}
Thing::Thing(ID inID) noexcept: mUID{inID}, mChildren{MakeShared<ThingChildren>()} {}
Thing::Thing(Farg<ThingData> inData) noexcept:
    mUID{inData.uid},
    mName{inData.name},
    mType{inData.type()},
    mChildren{MakeShared<ThingChildren>()} {}

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

bool Thing::uid(ID inID)
{
    if(inID.invalid())
        { inID = UID::Generate(); }
    else if(UID::Contains(inID[]))
        { return false; }
    UID::Erase(mUID[]);
    mUID = inID;
    return true;
}

void Thing::name(Sarg inName)
{ mName = inName; }

Thing::Children Thing::children() const
{ return mChildren->data; }

Error Thing::destroy_child(ID inID)
{
    if(disown_child(inID) == ERR_INVALID_ID)
        { return ERR_INVALID_ID; }
    return (g_pTheatreManager->DestroyThing(inID))
        ? OK
        : ERR_NOT_FOUND;
}

Error Thing::create_child(Farg<ThingData> inData, Sarg inUniqueName)
{
    if(inData.uid.invalid())
        { return ERR_INVALID_ID; }
    return own_child(g_pTheatreManager->CreateThing(inData), inUniqueName);
}

Error Thing::own_child(ID inID, Sarg inUniqueName)
{
    std::lock_guard<std::recursive_mutex> lock{mChildrenMutex};
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mChildren->data.insert({g_pTheatreManager->GetThing(inID),inUniqueName}).second)
        ? OK
        : ERR_ALREADY_EXISTS;
}

Error Thing::disown_child(ID inID)
{
    std::lock_guard<std::recursive_mutex> lock{mChildrenMutex};
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mChildren->data.erase(Shared<Thing>{new Thing{inID}}) > 0)
        ? OK
        : (mChildren->data.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
}
