#include "theatre/theatre.hpp"

using namespace TheatreFile;

Thing::~Thing() noexcept = default;
Thing::Thing() noexcept = default;

Thing::Thing(FPID inType, Sarg inName, ID inUID) noexcept:
    mUID{inUID}, mName{inName}, mType{inType} {}

void Thing::SetVariables(Farg<ThingData> data)
{
    mName = data.name;
    mType = data.type;
    mUID = data.uid;
    m_pStartingData = MakeShared<ThingData>(data);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>()};
    data->name = mName;
    data->uid  = mUID;
    data->type = mType;
    return data;
}

void Thing::Free()
{ print_error_enum(Theatre::Current()->DestroyThing(mUID)); }

ThingData Thing::GetStartingVariables() const
{ return *m_pStartingData; }

ID Thing::uid() const
{ return mUID; }

FPID Thing::type() const
{ return mType; }

Farg<std::string> Thing::name() const
{ return mName; }

void Thing::set_name(Sarg inName)
{ mName = inName; }

const char* const Thing::c_name() const
{ return mName.data(); }

bool Thing::FreeUID() const
{
    if(UID::IsReserved(mUID))
        { return UID::EraseReservedUID(mUID); }
    return UID::Erase(mUID);
}
