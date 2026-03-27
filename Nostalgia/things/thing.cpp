#include "./thing.hpp"
#include "theatre/thing_data.hpp"
#include "./thing_factory.hpp"

using namespace TheatreFile;

Thing::~Thing() noexcept = default;
Thing::Thing() noexcept = default;

Thing::Thing(Sarg inName, ID inUID) noexcept:
    mName{inName}, mUID{inUID} {}

void Thing::SetVariables(Farg<ThingData> data)
{
    if(mName.empty() and not data.name.empty())
        { mName = data.name; }
    if(mUID.invalid() and not data.type.invalid())
        { mUID = data._uid; }
    m_pStartingData = MakeShared<ThingData>(data);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>()};
    data->name = mName;
    data->_uid = mUID;
    data->type = Type();
    return data;
}

bool Thing::DerivedFrom(FPID inType) const
{ return ThingFactory::IsDerivedFrom(Type(), inType); }

bool Thing::IsThinker() const
{ return ThingFactory::IsThinker(Type()); }

bool Thing::IsResource() const
{ return ThingFactory::IsResource(Type()); }

ThingData Thing::GetStartingVariables() const
{ return *m_pStartingData; }

ID Thing::uid() const
{ return mUID; }

Farg<std::string> Thing::name() const
{ return mName; }

const char* const Thing::c_name() const
{ return mName.data(); }
