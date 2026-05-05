#include "./thing.hpp"
#include "./thing_data.hpp"
#include "./thing_factory.hpp"

#define LOCK(MUTEX) LockGuard<RMutex> lock{MUTEX}

using namespace TheatreFile;

Thing::Thing() noexcept: mUID{ID::Invalid} {}
Thing::Thing(Sarg inName) noexcept: mUID{_generate()}, mName{inName} {}

Thing::~Thing() noexcept
{
    this->Shutdown();
    LOCK(mMutex);
    m_sActiveUIDs.erase(mUID());
}

void Thing::SetVariables(Farg<ThingData> data)
{
    LOCK(mMutex);
    if(mName.empty())
        { mName = data.name; }
    if(not m_pStartingData)
        { m_pStartingData = MakeShared<ThingData>(data); }
}

Shared<ThingData> Thing::GetVariables() const
{
    LOCK(mMutex);
    auto data{MakeShared<ThingData>()};
    data->name = mName;
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
{
    LOCK(mMutex);
    return (m_pStartingData)
        ? *m_pStartingData
        : ThingData{};
}

ID Thing::uid() const
{ LOCK(mMutex); return mUID; }

Farg<std::string> Thing::name() const
{ LOCK(mMutex); return mName; }

const char* const Thing::c_name() const
{ LOCK(mMutex); return mName.data(); }

Error Thing::rename(Sarg inNewName)
{
    LOCK(mMutex);
    Error status{OK};
    if(m_pNameChangeCallback)
        { status = m_pNameChangeCallback(mName, inNewName); }
    if(status == OK)
        { mName = inNewName; }
    return status;
}

bool Thing::invalid() const
{ return mUID.invalid() or mName.empty(); }

std::string Thing::to_string() const
{ return std::format("<{}#{}>", Type().name(), uid()()); }

void Thing::SetNameChangeCallback(pNameChangeCallback_f inCallback)
{
    LOCK(mMutex);
    m_pNameChangeCallback = inCallback;
}

ID Thing::_generate()
{
    LOCK(m_sUIDMutex);
    if(m_sActiveUIDs.size() == UID::max_size)
    {
        print_warning("Somehow, you have hit the maximum number of UIDs for this set ({}). Please consider removing a few. Please.", UID::max_size);
        return ID::Invalid;
    }
    uint new_id{_get_random()};
    while(not m_sActiveUIDs.insert(new_id).second)
        { new_id = _get_random(); }
    return new_id;
}

uint Thing::_get_random()
{
    m_sIdEngine.seed(m_sRandomSeed());
    return m_sDistribution(m_sIdEngine);
}
