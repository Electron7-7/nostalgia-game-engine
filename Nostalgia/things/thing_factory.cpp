#include "things/thing_factory.hpp"
#include "things/resources/material.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/cubemap.hpp"
#include "things/resources/viewport_texture.hpp"
#include "things/resources/image_texture.hpp"
#include "things/resources/image.hpp"
#include "things/resources/mesh.hpp"
#include "things/resources/array_mesh.hpp"
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

#define ADD_THING(TYPE, BASE_TYPE, PRIORITY...) \
    AddThing(&ThingMakerTemplate<TYPE>, #TYPE, {#BASE_TYPE}, cDefaultPriority PRIORITY);

static std::map<PID, PID> sTypeDeclarations{};

static bool sIsInitialized{false};
static std::map<ID, pThingMakerTemplate_t> sThingMakers{};
static std::map<ID, int> sTypePriorities{};
static std::map<ID, std::unordered_set<PID>> sTypeAncestors{};
static Tree<PID> sTypes{};

bool ThingFactory::Init()
{
    if(sIsInitialized)
        { return true; }

    AddThing(&ThingMakerTemplate<Thing>, "Thing", PID{ID::Invalid, ""}, cDefaultPriority);
        ADD_THING(Resource, Thing, +1)
            ADD_THING(Image, Resource, +2)
            ADD_THING(Font, Resource, +2)
            ADD_THING(Mesh, Resource)
                ADD_THING(ArrayMesh, Mesh)
            ADD_THING(Texture, Resource)
                ADD_THING(Cubemap, Texture)
                ADD_THING(ViewportTexture, Texture)
                ADD_THING(ImageTexture, Texture)
            ADD_THING(Material, Resource)
        ADD_THING(Thinker, Thing)
            ADD_THING(Viewport, Thinker, +1)
            ADD_THING(NostalgiaPlayer, Thinker)
            ADD_THING(Actor3D, Thinker)
                ADD_THING(Ramiel, Actor3D)
                ADD_THING(Visual3D, Actor3D)
                    ADD_THING(MeshInstance3D, Visual3D)
                    ADD_THING(Sprite3D, Visual3D)
                    ADD_THING(Light3D, Visual3D)
                        ADD_THING(PointLight3D, Light3D)
                        ADD_THING(SpotLight3D, Light3D)
                        ADD_THING(DirectionalLight3D, Light3D)
                ADD_THING(Camera3D, Actor3D)
                ADD_THING(Collider3D, Actor3D)
        ADD_THING(Actor2D, Thinker)
            ADD_THING(Camera2D, Actor2D)
            ADD_THING(Visual2D, Actor2D)
                ADD_THING(Text2D, Visual2D)
                ADD_THING(Sprite2D, Visual2D)

    return sIsInitialized = true;
}

bool ThingFactory::IsInitialized()
{ return sIsInitialized; }

Shared<Thing> ThingFactory::MakeThing(FPID inType, Sarg inName)
{
    if(auto found_it{sThingMakers.find(inType)}; found_it != sThingMakers.end())
        { return found_it->second(inName); }
    print_warning("'{}' is an invalid type! An empty Thing will be returned", inType.name());
    return ThingMakerTemplate<Thing>(inName);
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

Error ThingFactory::AddThing(pThingMakerTemplate_t inPtr,
    Sarg inType,
    FPID inBaseType,
    int inPriority)
{
    if(Error _status{sTypes.add_node({inType}, inBaseType)}; not _status)
        { return _status; }
    else if(inPriority == cDefaultPriority)
    {
        auto _ancestors{sTypes.get_ancestors(inType)};
        for(auto iter{_ancestors.begin()}; iter != _ancestors.end(); ++iter)
        {
            if(auto found_it{sTypePriorities.find(*iter)}; found_it != sTypePriorities.end()
                and found_it->second != inPriority)
            {
                inPriority = found_it->second;
                break;
            }
        }
    }
    sTypePriorities[inType] = inPriority;
    sThingMakers[inType]    = inPtr;
    sTypeAncestors[inType]  = sTypes.get_ancestors(inType);
    if(Console::GetVariable("ThingFactory.debug_register_msgs").int_value)
    {
        if(inType == ThingType::Thing.name())
        {
            print_debug("Registered Thing (priority: {})", cDefaultPriority);
            return OK;
        }
        std::string _log{std::format("{} (priority {: }) derives: ", inType, inPriority)};
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
    sTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(FPID type)
{
    if(auto found_it{sTypePriorities.find(type)}; found_it != sTypePriorities.end())
        { return sTypePriorities.at(type); }
    return static_cast<int>(static_cast<uint>(-1) / 2); // Same as `INT_MAX`
}

std::set<PID> ThingFactory::DerivedFrom(FPID inType) noexcept
{ return sTypes.get_node(inType).children; }

PID ThingFactory::BaseOf(FPID inType) noexcept
{ return sTypes.get_node(inType).parent; }

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
