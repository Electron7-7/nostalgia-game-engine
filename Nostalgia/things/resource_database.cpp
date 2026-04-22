#include "./resource_database.hpp"
#include "things/thing_factory.hpp"

#define LOCK_MUTEX LockGuard<RMutex> lock{sMutex}
#define FOUND_IT(MAP, KEY) auto found_it{MAP.find(KEY)}; found_it != MAP.end()

static RMutex sMutex{};
static std::map<ID, Shared<Resource>> sResources{};
static std::map<std::string, ID> sNames{};

ID ResourceDatabase::Create(FPID inType, Sarg inName)
{
    LOCK_MUTEX;
    if(not ThingFactory::IsResource(inType))
    {
        print_error("Type '{}' is not a Resource", inType.name());
        return ID::Invalid;
    }
    else if(Contains(inName))
    {
        print_error("A Resource named '{}' already exists", inName);
        return ID::Invalid;
    }
    return Register(DCast<Resource>(ThingFactory::MakeThing(inType, inName)));
}

ID ResourceDatabase::Register(Shared<Resource> inResource, Sarg inNameOverride)
{
    LOCK_MUTEX;
    if(not inResource)
        { return ID::Invalid; }
    else if(not inNameOverride.empty())
        { inResource->mName = inNameOverride; }

    ID uid{inResource->uid()};
    Sarg name{inResource->name()};

    if(Contains(uid))
        { return uid; }
    else if(Contains(name))
        { return GetUID(name); }
    sNames[name] = uid;
    sResources[uid] = inResource;
    inResource->SetNameChangeCallback(&ResourceDatabase::SetName);

    if(Console::GetVariable("ResourceDatabase.debug_register_msgs")->int_value)
        { print_debug("Registered {} [{}, {}]", inResource->Type().name(), name, uid()); }

    return uid;
}

void ResourceDatabase::DestroyAll()
{
    LOCK_MUTEX;
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
{ return GetResource(inUID)->DerivedFrom(inType); }

bool ResourceDatabase::DerivedFrom(Sarg inName, FPID inType)
{ return GetResource(inName)->DerivedFrom(inType); }

Error ResourceDatabase::SetName(ID inUID, Sarg inName)
{
    if(Contains(inName))
        { return ERR_ALREADY_EXISTS; }
    else if(not Contains(inUID))
        { return ERR_NOT_FOUND; }
    LOCK_MUTEX;
    auto resource{sResources.at(inUID)};
    sNames.erase(resource->mName);
    sNames[inName] = inUID;
    resource->mName = inName;
    return OK;
}

Error ResourceDatabase::SetName(Sarg inOldName, Sarg inNewName)
{
    if(not Contains(inOldName))
        { return ERR_NOT_FOUND; }
    return SetName(GetUID(inOldName), inNewName);
}

FPID ResourceDatabase::TypeOf(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sResources, inUID))
        { return found_it->second->Type(); }
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
