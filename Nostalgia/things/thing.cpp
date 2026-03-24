#include "theatre/theatre.hpp"

using namespace TheatreFile;

Thing::Thing()  noexcept = default;
Thing::~Thing() noexcept = default;

Thing::Thing(FPID inType, Sarg inName, ID inUID) noexcept:
    mUID{inUID}, mName{inName}, mType{inType}
{
    if(mUID.invalid())
        { print_error_enum(UID::Generate(mUID)); }
    if(mName.empty())
        { mName = std::format("Untitled_Thing#{}", mUID()); }
    if(mType.invalid())
        { mType = ThingType::Thing; }
}

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type;
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
