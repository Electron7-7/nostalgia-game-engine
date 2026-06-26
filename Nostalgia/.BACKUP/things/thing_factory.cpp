#include "things/thing_factory.hpp"
#include "things/resources/material.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/cubemap.hpp"
#include "things/resources/viewport_texture.hpp"
#include "things/resources/image_texture.hpp"
#include "things/resources/image.hpp"
#include "things/resources/primitive_mesh.hpp"
#include "things/resources/font.hpp"
#include "things/thinkers/viewport.hpp"
#include "things/thinkers/2d/actor_2d.hpp"
#include "things/thinkers/2d/text_2d.hpp"
#include "things/thinkers/2d/visual_2d.hpp"
#include "things/thinkers/2d/camera_2d.hpp"
#include "things/thinkers/2d/sprite_2d.hpp"
#include "things/thinkers/3d/actor_3d.hpp"
#include "things/thinkers/3d/sprite_3d.hpp"
#include "things/thinkers/3d/ramiel.hpp"
#include "things/thinkers/nostalgia_player.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "things/thinkers/3d/visual_3d.hpp"
#include "things/thinkers/3d/mesh_instance_3d.hpp"
#include <cassert>

#define LOCK(MUTEX) LockGuard<RMutex> _##MUTEX##_lock{MUTEX};
#define ADD_THING(TYPE, BASE_TYPE, PRIORITY...) \
    AddThingType(&ThingMakerTemplate<TYPE>, #TYPE, #BASE_TYPE, false, cDefaultPriority PRIORITY);
#define ADD_VIRTUAL_THING(TYPE, BASE_TYPE, PRIORITY...) \
    AddThingType(&ThingMakerTemplate<TYPE>, #TYPE, #BASE_TYPE, true, cDefaultPriority PRIORITY);
#define LOCK_MUTEX LockGuard<RMutex> lock{sMutex}
#define FOUND_IT(MAP, KEY) auto found_it{MAP.find(KEY)}; found_it != MAP.end()

static RMutex sMutex{}, sExternMutex{};
static std::map<ID, Shared<Thing>> sThings{};
static std::unordered_set<ID> sResourceUIDs{}, sThinkerUIDs{};
static std::map<std::string, ID> sNames{};

static std::map<PID, PID> sTypeDeclarations{};
static bool sIsInitialized{false}, sImportantTypesRegistered{false};
static std::map<ID, ThingFactory::ThingType_t> sThingTypes{};
static std::map<ID, pThingMakerTemplate_t> sThingMakers{};
static std::map<ID, std::unordered_set<PID>> sTypeAncestors{};
static Tree<PID> sTypes{};
static std::vector<PID> sTypeIDs{ThingType::Resource, ThingType::Thinker};

static RMutex sUIDMutex{};
static std::unordered_set<uint> sReleasedUIDs{};
static uint sCurrentUID{0};

void ThingFactory::FreeUID(ID inUID)
{
    LOCK(sUIDMutex);
    if(inUID.invalid())
        { return; }
    sReleasedUIDs.emplace(inUID());
}

ID ThingFactory::GetUID()
{
    LOCK(sUIDMutex);
    ID _uid{};
    if(not sReleasedUIDs.empty())
    {
        _uid = *sReleasedUIDs.begin();
        sReleasedUIDs.erase(sReleasedUIDs.begin());
    }
    else if(sCurrentUID == ID::Invalid)
    {
        print_warning("You have hit the maximum number of UIDs: {}. Please consider not doing that. Please.",
            UID::max_size);
    }
    else
        { _uid = sCurrentUID++; } // Because I actually *do* want the value before incrementation
    return _uid;
}

bool ThingFactory::Init()
{
    if(sIsInitialized)
        { return true; }

    AddThingType(&ThingMakerTemplate<Thing>, "Thing", PID{ID::Invalid, ""}, true, cDefaultPriority);
    AddThingType(&ThingMakerTemplate<Resource>, "Resource", ThingType::Thing, true, cDefaultPriority + 1);
    AddThingType(&ThingMakerTemplate<Thinker>, "Thinker", ThingType::Thing, true, cDefaultPriority);
    sImportantTypesRegistered = true;

    // RESOURCES
        ADD_THING(Image, Resource, +2)
        ADD_THING(Font, Resource, +2)
        ADD_VIRTUAL_THING(Mesh, Resource)
            ADD_THING(ArrayMesh, Mesh)
            ADD_VIRTUAL_THING(PrimitiveMesh, Mesh)
                ADD_THING(BoxMesh, PrimitiveMesh)
                ADD_THING(SphereMesh, PrimitiveMesh)
                ADD_THING(CylinderMesh, PrimitiveMesh)
                // ADD_THING(CapsuleMesh, PrimitiveMesh)
        ADD_VIRTUAL_THING(Texture, Resource)
            ADD_THING(Cubemap, Texture)
            ADD_THING(ViewportTexture, Texture)
            ADD_THING(ImageTexture, Texture)
        ADD_THING(Material, Resource)
    // THINKERS
        ADD_THING(Viewport, Thinker, +1)
        ADD_THING(NostalgiaPlayer, Thinker)
        ADD_THING(Actor3D, Thinker)
            ADD_THING(Ramiel, Actor3D)
            ADD_VIRTUAL_THING(Visual3D, Actor3D)
                ADD_THING(MeshInstance3D, Visual3D)
                ADD_THING(Sprite3D, Visual3D)
                ADD_VIRTUAL_THING(Light3D, Visual3D)
                    ADD_THING(PointLight3D, Light3D)
                    ADD_THING(SpotLight3D, Light3D)
                    ADD_THING(DirectionalLight3D, Light3D)
            ADD_THING(Camera3D, Actor3D)
            ADD_THING(Collider3D, Actor3D)
        ADD_THING(Actor2D, Thinker)
            ADD_THING(Camera2D, Actor2D)
            ADD_VIRTUAL_THING(Visual2D, Actor2D)
                ADD_THING(Text2D, Visual2D)
                ADD_THING(Sprite2D, Visual2D)

    return sIsInitialized = true;
}

bool ThingFactory::IsInitialized()
{ return sIsInitialized; }

void ThingFactory::Shutdown()
{ sIsInitialized = false; }

std::string ThingFactory::GetUniqueName(Sarg inName)
{
    LOCK_MUTEX;
    std::string _return{inName};
    size_t _iter{0};
    while(sNames.contains(_return) and _iter != SIZE_MAX)
        { _return = inName + std::to_string(++_iter); }
    if(_iter == SIZE_MAX)
    {
        print_error("Holy fucking shit, dude, give a few of your Things a different fucking name");
        print_error("No, seriously, you literally have {} Things named '{}'", SIZE_MAX, inName);
    }
    return _return;
}

Shared<Thing> ThingFactory::MakeThing(FPID inType, Sarg inName)
{
    LOCK_MUTEX;
    if(auto found_it{sThingMakers.find(inType)}; found_it != sThingMakers.end())
        { return MakeThing(found_it->second(), inName); }
    print_warning("'{}' is an invalid type! Thing::Invalid() will be returned", inType.name());
    return Thing::Invalid();
}

Shared<Thing> ThingFactory::MakeThing(Shared<Thing> inThing, std::string inName)
{
    LOCK_MUTEX;
    if(not inThing)
        { return Thing::Invalid(); }

    ID _uid{GetUID()};
    if(sNames.contains(inName) or inName.empty() or ThingFactory::IsThing(PID{inName}))
        { inName = std::format("{}#{}", inName, _uid()); }

    inThing->mName = inName;
    inThing->mUID  = _uid;
    sNames[inName] = _uid;
    sThings[_uid]  = inThing;

    if(IsThinker(inThing->Type()))
        { sThinkerUIDs.insert(_uid); }
    else if(IsResource(inThing->Type()))
        { sResourceUIDs.insert(_uid); }
    if(Console::GetVariable("ThingFactory.debug_thing_msgs").int_value)
        { print_debug("Registered <{}> [{}, {}]", inThing->Type().name(), inThing->name(), inThing->uid()()); }

    inThing->_initialize_variables();
    return inThing;
}

void ThingFactory::DestroyAll()
{
    LOCK_MUTEX;
    sThings.clear();
    sNames.clear();
}

void ThingFactory::DestroyAllResources()
{
    LOCK_MUTEX;
    for(ID uid : sResourceUIDs)
        { print_error_enum(Destroy(uid)); }
    sResourceUIDs.clear();
}

LockGuard<RMutex> ThingFactory::LockSortedTypeIDs()
{ return LockGuard<RMutex>{sExternMutex}; }

Farg<std::vector<PID>> ThingFactory::GetSortedTypeIDs()
{ return sTypeIDs; }

Farg<Tree<PID>> ThingFactory::GetAllTypes()
{ return sTypes; }

IdVec_t ThingFactory::GetUIDs()
{
    LOCK_MUTEX;
    auto keys{std::views::keys(sThings)};
    return {keys.begin(), keys.end()};
}

IdVec_t ThingFactory::GetResourceUIDs()
{
    LOCK_MUTEX;
    return {sResourceUIDs.begin(), sResourceUIDs.end()};
}

IdVec_t ThingFactory::GetThinkerUIDs()
{
    LOCK_MUTEX;
    return {sThinkerUIDs.begin(), sThinkerUIDs.end()};
}

Sarg ThingFactory::GetName(ID inUID)
{
    static std::string empty{};
    LOCK_MUTEX;
    if(FOUND_IT(sThings, inUID))
        { return found_it->second->name(); }
    return empty;
}

ID ThingFactory::GetUID(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return found_it->second; }
    return ID::Invalid;
}

bool ThingFactory::DerivedFrom(ID inUID, FPID inType)
{ return GetThing(inUID)->DerivedFrom(inType); }

bool ThingFactory::DerivedFrom(Sarg inName, FPID inType)
{ return GetThing(inName)->DerivedFrom(inType); }

Error ThingFactory::SetName(ID inUID, Sarg inName)
{
    if(Contains(inName))
        { return ERR_ALREADY_EXISTS; }
    else if(not Contains(inUID))
        { return ERR_NOT_FOUND; }
    LOCK_MUTEX;
    auto Thing{sThings.at(inUID)};
    sNames.erase(Thing->mName);
    sNames[inName] = inUID;
    return OK;
}

Error ThingFactory::SetName(Sarg inOldName, Sarg inNewName)
{
    if(not Contains(inOldName))
        { return ERR_NOT_FOUND; }
    return SetName(GetUID(inOldName), inNewName);
}

FPID ThingFactory::TypeOf(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sThings, inUID))
        { return found_it->second->Type(); }
    return PID::Invalid;
}

FPID ThingFactory::TypeOf(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return TypeOf(found_it->second); }
    return PID::Invalid;
}

Error ThingFactory::Destroy(ID inUID)
{
    LOCK_MUTEX;
    sThinkerUIDs.erase(inUID);
    sResourceUIDs.erase(inUID);
    if(FOUND_IT(sThings, inUID))
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
        sThings.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error ThingFactory::Destroy(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return Destroy(found_it->second); }
    return ERR_NOT_FOUND;
}

bool ThingFactory::Contains(ID inUID)
{
    LOCK_MUTEX;
    return sThings.contains(inUID);
}

bool ThingFactory::Contains(Sarg inName)
{
    LOCK_MUTEX;
    return sNames.contains(inName);
}

Shared<Thing> ThingFactory::GetThing(ID inUID)
{
    LOCK_MUTEX;
    if(FOUND_IT(sThings, inUID))
        { return found_it->second; }
    return Thing::Invalid();
}

Shared<Thing> ThingFactory::GetThing(Sarg inName)
{
    LOCK_MUTEX;
    if(FOUND_IT(sNames, inName))
        { return GetThing(found_it->second); }
    return Thing::Invalid();
}

Error ThingFactory::AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists)
{
    if(not doOverrideIfExists and sTypeDeclarations.contains(inTypeName))
        { return ERR_ALREADY_EXISTS; }
    else if(not IsThing(inSuperName))
        { return ERR_INVALID_TYPE; }
    sTypeDeclarations[inTypeName] = inSuperName;
    return OK;
}

Error ThingFactory::AddThingType(pThingMakerTemplate_t inPtr, FPID inType, FPID inInherits, bool inVirtual, int inPriority)
{
    LOCK(sExternMutex)
    if(Error _status{sTypes.add_node(inType, inInherits)}; not _status)
        { return _status; }
    else if(inPriority == cDefaultPriority)
    {
        auto _ancestors{sTypes.get_ancestors(inType)};
        for(auto iter{_ancestors.begin()}; iter != _ancestors.end(); ++iter)
        {
            if(auto found_it{sThingTypes.find(*iter)}; found_it != sThingTypes.end()
                and found_it->second.init_priority != inPriority)
            {
                inPriority = found_it->second.init_priority;
                break;
            }
        }
    }
    if(sImportantTypesRegistered)
    {
        sTypeIDs.insert(std::find(sTypeIDs.begin(), sTypeIDs.end(),
            sTypes.get_node(inType).parent) + 1,
        inType);
    }
    sThingTypes[inType]     = {inType, inInherits, inVirtual, inPriority};
    sThingMakers[inType]    = inPtr;
    sTypeAncestors[inType]  = sTypes.get_ancestors(inType);
    if(Console::GetVariable("ThingFactory.debug_type_msgs").int_value)
    {
        if(inType == ThingType::Thing.name())
        {
            print_debug("Registered Thing (priority: {})", cDefaultPriority);
            return OK;
        }
        std::string _log{std::format("{} (virtual: {}) (priority {: }) derives: ", inType.name(), inVirtual, inPriority)};
        auto _ancestors{sTypes.get_ancestors(inType)};
        std::vector<PID> _ancestors_v{_ancestors.begin(), _ancestors.end()};
        for(auto iter{_ancestors_v.rbegin()}; iter != _ancestors_v.rend(); ++iter)
        {
            if(iter != _ancestors_v.rbegin())
                { _log += " > "; }
            _log += iter->name();
        }
        print_debug("Registered {}", _log);
    }
    return OK;
}

bool ThingFactory::SetPriority(FPID type, int priority)
{
    if(!IsThing(type))
        { return false; }
    sThingTypes[type].init_priority = priority;
    return true;
}

int ThingFactory::GetPriority(FPID type)
{
    if(auto found_it{sThingTypes.find(type)}; found_it != sThingTypes.end())
        { return sThingTypes.at(type).init_priority; }
    return static_cast<int>(static_cast<uint>(-1) / 2); // Same as `INT_MAX`
}

PidSet_t ThingFactory::DerivedFrom(FPID inType) noexcept
{
    LOCK(sExternMutex)
    return sTypes.get_node(inType).children;
}

PID ThingFactory::BaseOf(FPID inType) noexcept
{
    LOCK(sExternMutex)
    return sTypes.get_node(inType).parent;
}

bool ThingFactory::IsVirtual(FPID inType) noexcept
{
    LOCK(sExternMutex)
    if(auto found_it{sThingTypes.find(inType)}; found_it != sThingTypes.end())
        { return found_it->second.is_virtual; }
    return true;
}

bool ThingFactory::IsThing(FPID inTypeID)
{ return sTypeAncestors.contains(inTypeID) or sTypeDeclarations.contains(inTypeID); }

bool ThingFactory::IsThinker(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Thinker); }

bool ThingFactory::IsResource(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Resource); }

bool ThingFactory::IsDerivedFrom(FPID inTypeID1, FPID inTypeID2)
{
    return inTypeID1 == inTypeID2
        or (sTypeAncestors.contains(inTypeID1) and sTypeAncestors.at(inTypeID1).contains(inTypeID2));
}
