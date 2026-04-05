#include "./thing.hpp"
#include "things/thing_data.hpp"
#include "./thing_factory.hpp"

#define LOCK(MUTEX) LockGuard<RMutex> lock{MUTEX}

using namespace TheatreFile;

Thing::Thing(Sarg inName, ID inUID) noexcept:
    mUID{inUID}, mName{inName} {}

Thing::~Thing() noexcept
{
    this->Shutdown();
    LOCK(mMutex);
    if(not mUID.invalid() and UID::Contains(mUID) and UID::Erase(mUID))
        { mUID = ID::Invalid; }
}

void Thing::SetVariables(Farg<ThingData> data)
{
    LOCK(mMutex);
    if(mName.empty() and not data.name.empty())
        { mName = data.name; }
    if(mUID.invalid() and not data.type.invalid())
        { mUID = data._uid; }
    m_pStartingData = MakeShared<ThingData>(data);
}

Shared<ThingData> Thing::GetVariables() const
{
    LOCK(mMutex);
    auto data{MakeShared<ThingData>()};
    data->name = mName;
    data->_uid = mUID;
    data->type = Type();
    return data;
}

bool Thing::DerivedFrom(FPID inType) const
{ LOCK(mMutex); return ThingFactory::IsDerivedFrom(Type(), inType); }

bool Thing::IsThinker() const
{ LOCK(mMutex); return ThingFactory::IsThinker(Type()); }

bool Thing::IsResource() const
{ LOCK(mMutex); return ThingFactory::IsResource(Type()); }

ThingData Thing::GetStartingVariables() const
{ LOCK(mMutex); return *m_pStartingData; }

ID Thing::uid() const
{ LOCK(mMutex); return mUID; }

Farg<std::string> Thing::name() const
{ LOCK(mMutex); return mName; }

const char* const Thing::c_name() const
{ LOCK(mMutex); return mName.data(); }
