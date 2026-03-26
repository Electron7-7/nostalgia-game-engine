#include "theatre/theatre.hpp"

using namespace TheatreFile;

Thing::~Thing() noexcept = default;
Thing::Thing() noexcept = default;

Thing::Thing(FPID inType, Sarg inName, ID inUID) noexcept:
    mName{inName}, mUID{inUID}, mType{inType} {}

void Thing::SetVariables(Farg<ThingData> data)
{
    if(mName.empty() and not data.name.empty())
        { mName = data.name; }
    if(mType.invalid() and not data.type.invalid())
        { mType = data.type; }
    if(mUID.invalid() and not data.type.invalid())
        { mUID = data.uid; }
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

const char* const Thing::c_name() const
{ return mName.data(); }
