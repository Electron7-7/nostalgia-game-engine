#include "./resource_database.hpp"
#include "things/thing_factory.hpp"
#include <ranges>

#define LOCK_MUTEX LockGuard<RMutex> lock{sMutex}
#define FOUND_IT(MAP, KEY) auto found_it{MAP.find(KEY)}; found_it != MAP.end()

static RMutex sMutex{};
static std::map<ID, Shared<Resource>> sResources{};
static std::map<std::string, ID> sNames{};

ID ResourceDatabase::Register(TheatreFile::ThingData inData)
{
    LOCK_MUTEX;
    if(inData.uid.invalid() and not print_error_enum(UID::Generate(inData.uid)))
        { return ID::Invalid; }
    return Register(ThingFactory::MakeResource(inData.type, inData.name, inData.uid));
}

ID ResourceDatabase::Register(Shared<Resource> inResource)
{
    LOCK_MUTEX;
    if(Contains(inResource->uid()) or Contains(inResource->name()))
        { return inResource->uid(); }
    else if(inResource->uid().invalid())
        { return ID::Invalid; }
    sNames[inResource->name()] = inResource->uid();
    sResources[inResource->uid()] = inResource;
    inResource->Init();
    return inResource->uid();
}

void ResourceDatabase::DestroyAll()
{
    LOCK_MUTEX;
    for(FAUTO [uid, resource] : sResources)
        { resource->Shutdown(); }
    sResources.clear();
    sNames.clear();
}

IdVec_t ResourceDatabase::GetUIDs()
{
    LOCK_MUTEX;
    auto keys{std::views::keys(sResources)};
    return {keys.begin(), keys.end()};
}

Sarg ResourceDatabase::GetName(ID inUID)
{
    static std::string empty{};
    LOCK_MUTEX;
    if(FOUND_IT(sResources, inUID))
        { return found_it->second->name(); }
    return empty;
}

ID ResourceDatabase::GetUID(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return found_it->second; }
    return ID::Invalid;
}

bool ResourceDatabase::DerivedFrom(ID inUID, FPID inType)
{ return ThingFactory::IsDerivedFrom(TypeOf(inUID), inType); }

bool ResourceDatabase::DerivedFrom(Sarg inName, FPID inType)
{ return ThingFactory::IsDerivedFrom(TypeOf(inName), inType); }

FPID ResourceDatabase::TypeOf(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sResources, inUID))
        { return found_it->second->type(); }
    return ThingType::Invalid;
}

FPID ResourceDatabase::TypeOf(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return TypeOf(found_it->second); }
    return ThingType::Invalid;
}

Error ResourceDatabase::Destroy(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sResources, inUID))
    {
        if(not sNames.erase(found_it->second->name()))
        {
            for(FAUTO [name, uid] : sNames)
            {
                if(uid == inUID)
                {
                    sNames.erase(name);
                    break;
                }
            }
        }
        found_it->second->Shutdown();
        sResources.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error ResourceDatabase::Destroy(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return Destroy(found_it->second); }
    return ERR_NOT_FOUND;
}

bool ResourceDatabase::Contains(ID inUID)
{
    LOCK_MUTEX;
    return sResources.contains(inUID);
}

bool ResourceDatabase::Contains(Sarg inName)
{
    LOCK_MUTEX;
    return sNames.contains(inName);
}

Shared<Resource> ResourceDatabase::GetResource(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sResources, inUID))
        { return found_it->second; }
    return MakeShared<Resource>();
}

Shared<Resource> ResourceDatabase::GetResource(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return GetResource(found_it->second); }
    return MakeShared<Resource>();
}
