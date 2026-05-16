#include "./thing.hpp"
#include "./thing_data.hpp"
#include "./thing_factory.hpp"
#include "./thinkers/thinker.hpp"
#include "./resources/resource.hpp"

#define LOCK(MUTEX) LockGuard<RMutex> lock{MUTEX}

using namespace TheatreFile;

Shared<Thing> Thing::Invalid()
{
    Shared<Thing> _output{MakeShared<Thing>()};
    _output->mUID = ID::Invalid;
    return _output;
}

Shared<Thing> Thing::InvalidThinker()
{
    Shared<Thinker> _output{MakeShared<Thinker>()};
    _output->mUID = ID::Invalid;
    return _output;
}

Shared<Thing> Thing::InvalidResource()
{
    Shared<Resource> _output{MakeShared<Resource>()};
    _output->mUID = ID::Invalid;
    return _output;
}

Thing::~Thing() noexcept
{
    Shutdown();
    if(ThingFactory::IsInitialized())
        { ThingFactory::FreeUID(mUID); }
}

void Thing::InitVariables()
{
    if(m_pDefaultVariables)
        { return; }
    m_pDefaultVariables = MakeShared<ThingData>();
    m_pDefaultVariables->type = Type();
    m_pDefaultVariables->name = name();
}

void Thing::SetVariables(Farg<ThingData> data)
{
    LOCK(mMutex);
    if(mName.empty())
        { mName = data.name; }
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
{
    LOCK(mMutex);
    return ThingFactory::IsDerivedFrom(Type(), inType);
}

bool Thing::IsThinker() const
{
    LOCK(mMutex);
    return ThingFactory::IsThinker(Type());
}

bool Thing::IsResource() const
{
    LOCK(mMutex);
    return ThingFactory::IsResource(Type());
}

ThingData Thing::GetDefaultVariables() const
{
    LOCK(mMutex);
    return (m_pDefaultVariables) ? *m_pDefaultVariables : ThingData{Type(), name()};
}

ID Thing::uid() const
{
    LOCK(mMutex);
    return mUID;
}

Farg<std::string> Thing::name() const
{
    LOCK(mMutex);
    return mName;
}

const char* const Thing::c_name() const
{
    LOCK(mMutex);
    return mName.data();
}

Error Thing::rename(Sarg inNewName)
{
    LOCK(mMutex);
    if(not invalid())
    {
        if(auto _out{ThingFactory::SetName(mName, inNewName)}; not _out)
            { return _out; }
    }
    mName = inNewName;
    return OK;
}

bool Thing::invalid() const
{ return mUID.invalid() or mName.empty(); }

std::string Thing::to_string() const
{ return std::format("<{}#{}>", Type().name(), uid()()); }

void Thing::_initialize_variables()
{
    if(m_pDefaultVariables)
        { return; }
    this->InitVariables();
}
