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
#include "things/thinkers/3d/nostalgia_player_3d.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "things/thinkers/3d/visual_3d.hpp"
#include "things/thinkers/3d/mesh_instance_3d.hpp"
#include <cassert>

#define ADD_THING(TYPE, BASE_TYPE, PRIORITY...) \
    AddThing(&ThingMakerTemplate<TYPE>, #TYPE, {#BASE_TYPE}, cDefaultPriority PRIORITY);

bool ThingFactory::m_sIsInitialized{false};
std::map<ID, pThingMakerTemplate_t> ThingFactory::m_sThingMakers{};
std::map<ID, int>                   ThingFactory::m_sTypePriorities{};
std::set<ThingType_t>               ThingFactory::m_sAllTypes{};

bool ThingFactory::Init()
{
    if(m_sIsInitialized)
        { return true; }

    ADD_THING(Thing, Thing)
        ADD_THING(Resource, Thing)
            ADD_THING(Font, Resource)
            ADD_THING(Mesh, Resource)
                ADD_THING(ArrayMesh, Resource)
            ADD_THING(Texture, Resource)
            ADD_THING(Image, Resource)
            ADD_THING(Cubemap, Resource)
            ADD_THING(ViewportTexture, Resource)
            ADD_THING(ImageTexture, Texture)
            ADD_THING(Material, Resource)
        ADD_THING(Thinker, Thing)
            ADD_THING(Viewport, Thinker)
            ADD_THING(Actor3D, Thinker)
                ADD_THING(Visual3D, Actor3D)
                    ADD_THING(MeshInstance3D, Visual3D)
                    ADD_THING(Sprite3D, Visual3D)
                    ADD_THING(Light3D, Visual3D)
                        ADD_THING(PointLight3D, Light3D)
                        ADD_THING(SpotLight3D, Light3D)
                        ADD_THING(DirectionalLight3D, Light3D)
                ADD_THING(NostalgiaPlayer3D, Actor3D)
                ADD_THING(Camera3D, Actor3D)
                ADD_THING(Collider3D, Actor3D)
        ADD_THING(Actor2D, Thinker)
            ADD_THING(Camera2D, Actor2D)
            ADD_THING(Visual2D, Actor2D)
                ADD_THING(Text2D, Visual2D)
                ADD_THING(Sprite2D, Visual2D)

    return m_sIsInitialized = true;
}

bool ThingFactory::IsInitialized()
{ return m_sIsInitialized; }

Error ThingFactory::AddThing(pThingMakerTemplate_t inPtr,
    Sarg inType,
    FPID inBaseType,
    int inPriority)
{
    ThingType_t type{inType};
    if(type.type() == ThingType::Thing)
    {
        type._all_base_types.emplace(type._base_type_id = ThingType::Thing);
        m_sAllTypes.emplace(type);
        m_sThingMakers[inType]    = inPtr;
        m_sTypePriorities[inType] = inPriority;
        if(Console::try_GetVariable("ThingFactory.debug_register_msgs")->int_value)
            { print_debug("Registered New ThingType_t: {}", type.log()); }
        return OK;
    }
    else if(m_sAllTypes.contains(type))
    {
        print_error("ThingType_t '{}' already exists!", inType);
        return ERR_ALREADY_EXISTS;
    }
    else if(!m_sAllTypes.contains(inBaseType))
    {
        print_error("Base ThingType_t '{}' is invalid!", inBaseType.name());
        return ERR_INVALID_TYPE;
    }
    type._all_base_types.emplace(type._base_type_id = inBaseType);
    PID next_base{type._base_type_id};
    while(next_base != ThingType::Thing)
    {
        if(auto found_it{m_sAllTypes.find(next_base)};
            found_it != m_sAllTypes.end())
        {
            type._all_base_types.emplace(next_base);
            next_base = found_it->_base_type_id;
        }
        else
            { next_base = ThingType::Thing; }
    }
    type._all_base_types.emplace(next_base);
    if(Console::try_GetVariable("ThingFactory.debug_register_msgs")->int_value)
        { print_debug("Registered New ThingType_t: {}", type.log()); }
    m_sAllTypes.emplace(type);

    m_sThingMakers[inType]    = inPtr;
    m_sTypePriorities[inType] = inPriority;
    return OK;
}

Error ThingFactory::RemoveThing(FPID inType) noexcept
{
    m_sAllTypes.erase(inType);
    m_sThingMakers.erase(inType);
    m_sTypePriorities.erase(inType);
    return OK;
}

Farg<ThingType_t> ThingFactory::GetType(FPID inType) noexcept
{
    static ThingType_t sBadType{ThingType::Invalid};
    if(auto found_it{m_sAllTypes.find(inType)};
        found_it != m_sAllTypes.end())
            { return *found_it; }
    return sBadType;
}

Shared<Thing> ThingFactory::MakeThing(FPID inType, Sarg inName)
{
    if(auto found_it{m_sThingMakers.find(inType)}; found_it != m_sThingMakers.end())
        { return found_it->second(inName); }
    print_warning("ThingType_t '{}' is an invalid type! An empty Thing will be returned",
        inType.name());
    return ThingMakerTemplate<Thing>(inName);
}

bool ThingFactory::SetPriority(FPID type, int priority)
{
    if(!IsThing(type))
        { return false; }
    m_sTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(FPID type)
{
    if(auto found_it{m_sTypePriorities.find(type)}; found_it != m_sTypePriorities.end())
        { return m_sTypePriorities.at(type); }
    return static_cast<int>(static_cast<uint>(-1) / 2); // Same as `INT_MAX`
}

bool ThingFactory::IsThing(FPID inTypeID)
{ return m_sAllTypes.contains(inTypeID); }

bool ThingFactory::IsThinker(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Thinker); }

bool ThingFactory::IsResource(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Resource); }

bool ThingFactory::IsDerivedFrom(FPID inTypeID1, FPID inTypeID2)
{
    if(auto found_it{m_sAllTypes.find(inTypeID1)}; found_it != m_sAllTypes.end())
        { return (found_it->type() == inTypeID2 or found_it->is_derived_from(inTypeID2)); }
    return false;
}

#undef ADD_THING
