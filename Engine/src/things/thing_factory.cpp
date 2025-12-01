#include "thing_factory.hpp"
#include "thing.hpp"
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

static ThingFactory sThingFactory;
ThingFactory* g_pThingFactory = &sThingFactory;

template<typename T> requires(std::derived_from<T,Thing>)
std::shared_ptr<Thing> gThingMakerTemplate()
{ return std::make_shared<T>(); }

bool ThingFactory::Init()
{
    if(mIsInitialized)
        { return true; }

    AddThing( &gThingMakerTemplate<Mesh>,                  "Mesh",                  cDefaultPriority - 2 );
    AddThing( &gThingMakerTemplate<Texture>,               "Texture",               cDefaultPriority - 2 );
    AddThing( &gThingMakerTemplate<Font>,                  "Font",                  cDefaultPriority - 2 );
    AddThing( &gThingMakerTemplate<Material>,              "Material",              cDefaultPriority - 1 );
    AddThing( &gThingMakerTemplate<MeshInstance>,          "MeshInstance",          cDefaultPriority - 1 );
    AddThing( &gThingMakerTemplate<NostalgiaPlayer>,       "NostalgiaPlayer",       cDefaultPriority     );
    AddThing( &gThingMakerTemplate<PointLight>,            "PointLight",            cDefaultPriority     );
    AddThing( &gThingMakerTemplate<SpotLight>,             "SpotLight",             cDefaultPriority     );
    AddThing( &gThingMakerTemplate<DirectionalLight>,      "DirectionalLight",      cDefaultPriority     );
    AddThing( &gThingMakerTemplate<Device>,                "Device",                cDefaultPriority + 1 );
    AddThing( &gThingMakerTemplate<Collider>,              "Collider",              cDefaultPriority + 1 );
    AddThing( &gThingMakerTemplate<Resource>,              "Resource",              cDefaultPriority + 1 );
    AddThing( &gThingMakerTemplate<Actor>,                 "Actor",                 cDefaultPriority + 1 );
    AddThing( &gThingMakerTemplate<Thinker>,               "Thinker",               cDefaultPriority + 1 );
    AddThing( &gThingMakerTemplate<Thing>,                 "Thing",                 cDefaultPriority + 1 );

    return mIsInitialized = true;
}

bool ThingFactory::AddThing(pThingMaker_t maker_ptr, const std::string& type_name, int priority)
{
    ID type_id = ConstexprHash(type_name);
    if(mThingMakers.contains(type_id))
    {
        print_warning("'{}' already added", type_name);
        return false;
    }
    mThingMakers[type_id] = maker_ptr;
    mTypeNames[type_id] = type_name;
    mTypePriorities[type_id] = priority;
    return true;
}

pThingMaker_t ThingFactory::MakeThing(ID type)
{
    if(!mThingMakers.contains(type))
    {
        print_warning("Type '{}' is an invalid type! An empty Thing will be returned", GetTypeName(type));
        return gThingMakerTemplate<Thing>;
    }
    return mThingMakers.at(type);
}

bool ThingFactory::SetPriority(ID type, int priority)
{
    if(!IsThing(type))
        { return false; }
    mTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(ID type) const
{
    if(!mTypePriorities.contains(type))
        { return 9999; }
    return mTypePriorities.at(type);
}

const std::string& ThingFactory::GetTypeName(ID type) const
{
    static std::string l_sInvalidTypeName = "Invalid";
    if(!mTypeNames.contains(type))
        { return l_sInvalidTypeName; }
    return mTypeNames.at(type);
}

bool ThingFactory::IsThing(ID type) const
{ return mThingMakers.contains(type); }

bool ThingFactory::IsResource(ID type) const
{ return IsDerivedFrom<Resource>(type); }

bool ThingFactory::IsDevice(ID type) const
{ return IsDerivedFrom<Device>(type); }
