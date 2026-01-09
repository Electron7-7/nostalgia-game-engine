#include "thing_factory.hpp"
#include "things/thinkers/thinker.hpp"
#include "things/thinkers/2d/actor_2d.hpp"
#include "things/thinkers/3d/actor_3d.hpp"
#include "things/thinkers/3d/nostalgia_player_3d.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "things/thinkers/3d/visual_3d.hpp"
#include "things/thinkers/3d/mesh_instance_3d.hpp"
#include "things/thinkers/viewport.hpp"
#include "things/resources/resource.hpp"
#include "things/resources/material.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/mesh.hpp"
#include "core/printing.hpp"

#define ADD_THING(THING, PRIORITY...) AddThing(&ThingMakerTemplate<THING>, #THING, cDefaultPriority PRIORITY);

static ThingFactory sThingFactory;
ThingFactory* g_pThingFactory = &sThingFactory;

bool ThingFactory::Init()
{
    if(mIsInitialized)
        { return true; }

    ADD_THING(Thing,    +1)
    ADD_THING(Resource, +1)
    ADD_THING(Mesh,     -2)
    ADD_THING(Texture,  -2)
    ADD_THING(Material, -2)
    ADD_THING(Thinker,  +1)
    ADD_THING(Actor3D,  +1)
    ADD_THING(Visual3D)
    ADD_THING(MeshInstance3D, -1)
    ADD_THING(NostalgiaPlayer3D)
    ADD_THING(Camera3D, -2)
    ADD_THING(PointLight3D)
    ADD_THING(SpotLight3D)
    ADD_THING(DirectionalLight3D)
    ADD_THING(Viewport, +1)
    ADD_THING(Collider3D, +1)
    ADD_THING(Actor2D,  +1)

    return mIsInitialized = true;
}

bool ThingFactory::AddThing(pThingMakerTemplate_t maker_ptr, Farg<std::string> type_name, int priority)
{
    if(auto found_it{mThingMakers.find(type_name)}; found_it != mThingMakers.end())
        { return !print_warning("Thing type '{}' already added", type_name); }
    mThingMakers[type_name]    = maker_ptr;
    mTypePriorities[type_name] = priority;
    return true;
}

pThingMakerTemplate_t ThingFactory::MakeThing(Farg<PID> type)
{
    if(auto found_it{mThingMakers.find(type)}; found_it != mThingMakers.end())
        { return found_it->second; }
    print_warning("Type '{}' is an invalid type! An empty Thing will be returned", type.name());
    return ThingMakerTemplate<Thing>;
}

bool ThingFactory::SetPriority(Farg<PID> type, int priority)
{
    if(!IsThing(type))
        { return false; }
    mTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(Farg<PID> type) const
{
    if(auto found_it{mTypePriorities.find(type)}; found_it != mTypePriorities.end())
        { return mTypePriorities.at(type); }
    return static_cast<int>(static_cast<uint>(-1) / 2); // Same as `INT_MAX`
}

bool ThingFactory::IsThing(Farg<PID> type) const
{ return mThingMakers.contains(type); }

bool ThingFactory::IsResource(Farg<PID> type) const
{ return IsDerivedFrom<Resource>(type); }

#undef ADD_THING
