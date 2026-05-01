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

bool ThingFactory::m_sIsInitialized{false};
std::map<ID, pThingMakerTemplate_t> ThingFactory::m_sThingMakers{};
std::map<ID, int>                   ThingFactory::m_sTypePriorities{};
std::set<ThingType>                 ThingFactory::m_sAllTypes{};

bool ThingFactory::Init()
{
    if(m_sIsInitialized)
        { return true; }

    ADD_THING(Thing, Thing)
        ADD_THING(Resource, Thing, +1)
            ADD_THING(Image, Resource, +2)
            ADD_THING(Font, Resource, +2)
            ADD_THING(Mesh, Resource)
                ADD_THING(ArrayMesh, Resource)
            ADD_THING(Texture, Resource)
                ADD_THING(Cubemap, Resource)
                ADD_THING(ViewportTexture, Resource)
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

    return m_sIsInitialized = true;
}

bool ThingFactory::IsInitialized()
{ return m_sIsInitialized; }

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
    ThingType type{inType};
    if(type.type() == ThingType::Thing)
    {
        type._all_base_types.emplace(type._base_type_id = ThingType::Thing);
        m_sAllTypes.emplace(type);
        m_sThingMakers[inType]    = inPtr;
        m_sTypePriorities[inType] = inPriority;
        if(Console::GetVariable("ThingFactory.debug_register_msgs").int_value)
            { print_debug("Registered New ThingType: {}", type.log()); }
        return OK;
    }
    else if(m_sAllTypes.contains(type))
    {
        print_error("ThingType '{}' already exists!", inType);
        return ERR_ALREADY_EXISTS;
    }
    else if(!m_sAllTypes.contains(inBaseType))
    {
        print_error("Base ThingType '{}' is invalid!", inBaseType.name());
        return ERR_INVALID_TYPE;
    }
    int _priority{inPriority};
    type._all_base_types.emplace(type._base_type_id = inBaseType);
    PID next_base{type._base_type_id};
    while(next_base != ThingType::Thing)
    {
        if(auto found_it{m_sAllTypes.find(next_base)};
            found_it != m_sAllTypes.end())
        {
            type._all_base_types.emplace(next_base);
            if(_priority == cDefaultPriority)
            {
                PID _next_base{next_base};
                while(_next_base != ThingType::Thing)
                {
                    if(auto found_it_again{m_sTypePriorities.find(_next_base)};
                        found_it_again != m_sTypePriorities.end() and found_it_again->second != cDefaultPriority)
                    {
                        _priority = found_it_again->second;
                        break;
                    }
                    _next_base = m_sAllTypes.find(_next_base)->_base_type_id;
                }
            }
            next_base = found_it->_base_type_id;
        }
        else
            { next_base = ThingType::Thing; }
    }
    type._all_base_types.emplace(next_base);
    if(Console::GetVariable("ThingFactory.debug_register_msgs").int_value)
        { print_debug("Registered New ThingType w/ priority {: }: {}", _priority, type.log()); }
    m_sAllTypes.emplace(type);

    m_sThingMakers[inType]    = inPtr;
    m_sTypePriorities[inType] = _priority;
    return OK;
}

Farg<std::map<PID, PID>> ThingFactory::GetThingDeclarations()
{ return sTypeDeclarations; }

std::set<ThingType> ThingFactory::GetAllTypes()
{ return m_sAllTypes; }

IdVec_t ThingFactory::GetAllTypeIDs()
{
    IdVec_t _types{};
    for(auto iter{m_sAllTypes.rbegin()}; iter != m_sAllTypes.rend(); ++iter)
        { _types.push_back(iter->type()); }
    return _types;
}

Error ThingFactory::RemoveThing(FPID inType) noexcept
{
    m_sAllTypes.erase(inType);
    m_sThingMakers.erase(inType);
    m_sTypePriorities.erase(inType);
    return OK;
}

Farg<ThingType> ThingFactory::GetType(FPID inType) noexcept
{
    static ThingType sBadType{ThingType::Invalid};
    if(auto found_it{m_sAllTypes.find(inType)}; found_it != m_sAllTypes.end())
        { return *found_it; }
    return sBadType;
}

PID ThingFactory::GetClosestType(FPID inType) noexcept
{
    if(auto found_it{m_sAllTypes.find(inType)}; found_it != m_sAllTypes.end())
        { return found_it->type(); }
    else if(auto found_it_again{sTypeDeclarations.find(inType)}; found_it_again != sTypeDeclarations.end())
    {
        if(m_sAllTypes.contains(found_it_again->second))
            { return found_it_again->second; }
    }
    return GetType(inType).base_type();
}

Shared<Thing> ThingFactory::MakeThing(FPID inType, Sarg inName)
{
    if(auto found_it{m_sThingMakers.find(inType)}; found_it != m_sThingMakers.end())
        { return found_it->second(inName); }
    print_warning("ThingType '{}' is an invalid type! An empty Thing will be returned",
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
{ return m_sAllTypes.contains(inTypeID) or sTypeDeclarations.contains(inTypeID); }

bool ThingFactory::IsThinker(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Thinker); }

bool ThingFactory::IsResource(FPID inTypeID)
{ return IsDerivedFrom(inTypeID, ThingType::Resource); }

bool ThingFactory::IsDerivedFrom(FPID inTypeID1, FPID inTypeID2)
{
    if(auto found_it{m_sAllTypes.find(inTypeID1)}; found_it != m_sAllTypes.end())
        { return (found_it->type() == inTypeID2 or found_it->is_derived_from(inTypeID2)); }
    else if(auto found_it{sTypeDeclarations.find(inTypeID1)}; found_it != sTypeDeclarations.end())
        { return IsDerivedFrom(found_it->second, inTypeID2); }
    return false;
}

#undef ADD_THING
