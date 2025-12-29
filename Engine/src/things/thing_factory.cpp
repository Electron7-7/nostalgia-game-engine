#include "thing_factory.hpp"
#include "thinkers/thinker.hpp"
#include "actors/actor.hpp"
#include "actors/nostalgia_player.hpp"
#include "actors/light.hpp"
#include "devices/device.hpp"
#include "devices/collider.hpp"
#include "devices/material.hpp"
#include "devices/mesh_instance.hpp"
#include "resources/resource.hpp"
#include "resources/font.hpp"
#include "resources/texture.hpp"
#include "resources/mesh.hpp"
#include "core/printing.hpp"

static ThingFactory sThingFactory;
ThingFactory* g_pThingFactory = &sThingFactory;

bool ThingFactory::Init()
{
    if(mIsInitialized)
        { return true; }

    AddThing( &ThingMakerTemplate<Mesh>,                  "Mesh",                  cDefaultPriority - 2 );
    AddThing( &ThingMakerTemplate<Texture>,               "Texture",               cDefaultPriority - 2 );
    AddThing( &ThingMakerTemplate<Font>,                  "Font",                  cDefaultPriority - 2 );
    AddThing( &ThingMakerTemplate<Material>,              "Material",              cDefaultPriority - 1 );
    AddThing( &ThingMakerTemplate<MeshInstance>,          "MeshInstance",          cDefaultPriority - 1 );
    AddThing( &ThingMakerTemplate<NostalgiaPlayer>,       "NostalgiaPlayer",       cDefaultPriority     );
    AddThing( &ThingMakerTemplate<PointLight>,            "PointLight",            cDefaultPriority     );
    AddThing( &ThingMakerTemplate<SpotLight>,             "SpotLight",             cDefaultPriority     );
    AddThing( &ThingMakerTemplate<DirectionalLight>,      "DirectionalLight",      cDefaultPriority     );
    AddThing( &ThingMakerTemplate<Device>,                "Device",                cDefaultPriority + 1 );
    AddThing( &ThingMakerTemplate<Collider>,              "Collider",              cDefaultPriority + 1 );
    AddThing( &ThingMakerTemplate<Resource>,              "Resource",              cDefaultPriority + 1 );
    AddThing( &ThingMakerTemplate<Actor>,                 "Actor",                 cDefaultPriority + 1 );
    AddThing( &ThingMakerTemplate<Thinker>,               "Thinker",               cDefaultPriority + 1 );
    AddThing( &ThingMakerTemplate<Thing>,                 "Thing",                 cDefaultPriority + 1 );

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

pThingMakerTemplate_t ThingFactory::MakeThing(Farg<TTID> type)
{
    if(auto found_it{mThingMakers.find(type)}; found_it != mThingMakers.end())
        { return found_it->second; }
    print_warning("Type '{}' is an invalid type! An empty Thing will be returned", type.name());
    return ThingMakerTemplate<Thing>;
}

bool ThingFactory::SetPriority(Farg<TTID> type, int priority)
{
    if(!IsThing(type))
        { return false; }
    mTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(Farg<TTID> type) const
{
    if(auto found_it{mTypePriorities.find(type)}; found_it != mTypePriorities.end())
        { return mTypePriorities.at(type); }
    return static_cast<int>(static_cast<uint>(-1) / 2); // Same as `INT_MAX`
}

bool ThingFactory::IsThing(Farg<TTID> type) const
{ return mThingMakers.contains(type); }

bool ThingFactory::IsResource(Farg<TTID> type) const
{ return IsDerivedFrom<Resource>(type); }

bool ThingFactory::IsDevice(Farg<TTID> type) const
{ return IsDerivedFrom<Device>(type); }
