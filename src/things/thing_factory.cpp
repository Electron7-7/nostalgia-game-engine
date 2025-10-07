#include "thing_factory.hpp"
#include "types.hpp"
#include "thing.hpp"
#include "thinkers/thinker.hpp"
#include "actors/actor.hpp"
#include "actors/nostalgia_player.hpp"
#include "actors/light.hpp"
#include "devices/device.hpp"
#include "devices/material.hpp"
#include "devices/mesh_instance.hpp"
#include "resources/resource.hpp"
#include "resources/font.hpp"
#include "resources/texture.hpp"
#include "resources/mesh.hpp"

// template<IsThing T>
// std::shared_ptr<Thing> gThingMakerTemplate()
// { return std::make_shared<T>(); }

template<typename T>
std::shared_ptr<Thing> gThingMakerTemplate()
{
    static_assert(std::derived_from<T,Thing>);
    return std::make_shared<T>();
}

std::map<ID, pThingMaker_t>
ThingFactory::m_sThingMakers =
{
    { ThingType::Thing,            &gThingMakerTemplate<Thing>            },
    { ThingType::Thinker,          &gThingMakerTemplate<Thinker>          },
    { ThingType::Actor,            &gThingMakerTemplate<Actor>            },
    { ThingType::NostalgiaPlayer,  &gThingMakerTemplate<NostalgiaPlayer>  },
    { ThingType::PointLight,       &gThingMakerTemplate<PointLight>       },
    { ThingType::SpotLight,        &gThingMakerTemplate<SpotLight>        },
    { ThingType::DirectionalLight, &gThingMakerTemplate<DirectionalLight> },
    { ThingType::Resource,         &gThingMakerTemplate<Resource>         },
    { ThingType::Mesh,             &gThingMakerTemplate<Mesh>             },
    { ThingType::Texture,          &gThingMakerTemplate<Texture>          },
    { ThingType::Font,             &gThingMakerTemplate<Font>             },
    { ThingType::Device,           &gThingMakerTemplate<Device>           },
    { ThingType::Material,         &gThingMakerTemplate<Material>         },
    { ThingType::MeshInstance,     &gThingMakerTemplate<MeshInstance>     },
};

std::map<ID, std::string>
ThingFactory::m_sTypeNames =
{
    { ThingType::Thing,            "Thing"            },
    { ThingType::Resource,         "Resource"         },
    { ThingType::Mesh,             "Mesh"             },
    { ThingType::Texture,          "Texture"          },
    { ThingType::Font,             "Font"             },
    { ThingType::Device,           "Device"           },
    { ThingType::Material,         "Material"         },
    { ThingType::MeshInstance,     "MeshInstance"     },
    { ThingType::Thinker,          "Thinker"          },
    { ThingType::Actor,            "Actor"            },
    { ThingType::PointLight,       "PointLight"       },
    { ThingType::SpotLight,        "SpotLight"        },
    { ThingType::DirectionalLight, "DirectionalLight" },
    { ThingType::NostalgiaPlayer,  "NostalgiaPlayer"  },
};

std::map<ID, int>
ThingFactory::m_sTypePriorities =
{
    { ThingType::Mesh,            -2 },
    { ThingType::Texture,         -2 },
    { ThingType::Font,            -2 },
    { ThingType::Material,        -1 },
    { ThingType::MeshInstance,    -1 },
    { ThingType::PointLight,       0 },
    { ThingType::SpotLight,        0 },
    { ThingType::DirectionalLight, 0 },
    { ThingType::NostalgiaPlayer,  0 },
    { ThingType::Thinker,          1 },
    { ThingType::Actor,            1 },
    { ThingType::Resource,         1 },
    { ThingType::Thing,            1 },
};

bool ThingFactory::AddThing(pThingMaker_t maker_ptr, const std::string& type_name, int priority)
{
    ID type_id = ConstexprHash(type_name);
    if(m_sThingMakers.contains(type_id))
    {
        PRINT_WARNING("ThingFactory::AddThing - '{}' already added", type_name)
        return false;
    }
    m_sThingMakers[type_id] = maker_ptr;
    m_sTypeNames[type_id] = type_name;
    m_sTypePriorities[type_id] = priority;
    return true;
}

pThingMaker_t ThingFactory::MakeThing(ID type)
{
    if(!m_sThingMakers.contains(type))
    {
        PRINT_WARNING("ThingFactory::MakeThing - Type '{}' is an invalid type! An empty Thing will be returned", GetTypeName(type))
        return gThingMakerTemplate<Thing>;
    }
    return m_sThingMakers.at(type);
}

bool ThingFactory::SetPriority(ID type, int priority)
{
    if(!IsThing(type))
        { return false; }
    m_sTypePriorities[type] = priority;
    return true;
}

int ThingFactory::GetPriority(ID type)
{
    if(!m_sTypePriorities.contains(type))
        { return 9999; }
    return m_sTypePriorities.at(type);
}

const std::string& ThingFactory::GetTypeName(ID type)
{
    static std::string l_sInvalidTypeName = "Invalid";
    if(!m_sTypeNames.contains(type))
        { return l_sInvalidTypeName; }
    return m_sTypeNames.at(type);
}

bool ThingFactory::IsThing(ID type)
{ return m_sThingMakers.contains(type); }

bool ThingFactory::IsThing(const std::string& name)
{ return IsThing(ConstexprHash(name)); }

bool ThingFactory::IsResource(ID type)
{ return IsDerivedFrom<Resource>(type); }

bool ThingFactory::IsResource(const std::string& name)
{ return IsResource(ConstexprHash(name)); }

template<ThingDerived T>
bool ThingFactory::IsDerivedFrom(ID type)
{
    if(!IsThing(type))
        { return false; }
    return (std::dynamic_pointer_cast<T>(m_sThingMakers.at(type)()) != nullptr);
}
