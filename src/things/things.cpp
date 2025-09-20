#include "things.hpp"
#include "managers/theatre_manager.hpp"
#include "printing.hpp"
#include "thinkers/thinker.hpp"
#include "actors/actor.hpp"
#include "actors/nostalgia_player.hpp"
#include "resources/basic/mesh.hpp"
#include "resources/basic/font.hpp"
#include "resources/basic/texture.hpp"
#include "resources/complex/material.hpp"
#include "resources/complex/mesh_instance.hpp"

#include <map>

template<IsThing T>
std::shared_ptr<Thing> l_MakeThing()
{ return std::make_shared<T>(); }

inline const std::map<size_t, std::shared_ptr<Thing>(*)()>
s_ThingConstructors =
{
    { Type::Thing,           &l_MakeThing<Thing>           },
    { Type::Thinker,         &l_MakeThing<Thinker>         },
    { Type::Actor,           &l_MakeThing<Actor>           },
    { Type::NostalgiaPlayer, &l_MakeThing<NostalgiaPlayer> },
    { Type::Resource,        &l_MakeThing<Resource>        },
    { Type::Mesh,            &l_MakeThing<Mesh>            },
    { Type::Font,            &l_MakeThing<Font>            },
    { Type::Material,        &l_MakeThing<Material>        },
    { Type::Texture,         &l_MakeThing<Texture>         },
    { Type::MeshInstance,    &l_MakeThing<MeshInstance>    },
};

std::shared_ptr<Thing>(*g_MakeThing(size_t type))()
{
    if(!s_ThingConstructors.contains(type))
    {
        PRINT_WARNING("g_MakeThing - Type '{}' is an invalid type! An empty Thing will be made, instead", type)
        return &l_MakeThing<Thing>;
    }
    return s_ThingConstructors.at(type);
}

template<typename T>
std::shared_ptr<T> g_GetThing(id_t id)
{
    if(!dynamic_pointer_cast<T>(TheatreManager::GetThing(id)))
        { return std::make_shared<T>(); }
    return dynamic_pointer_cast<T>(TheatreManager::GetThing(id));
}

template std::shared_ptr<Thing> g_GetThing<Thing>(id_t ID);
template std::shared_ptr<Thinker> g_GetThing<Thinker>(id_t ID);
template std::shared_ptr<Actor> g_GetThing<Actor>(id_t ID);
template std::shared_ptr<NostalgiaPlayer> g_GetThing<NostalgiaPlayer>(id_t ID);
template std::shared_ptr<Resource> g_GetThing<Resource>(id_t ID);
template std::shared_ptr<Mesh> g_GetThing<Mesh>(id_t ID);
template std::shared_ptr<Texture> g_GetThing<Texture>(id_t ID);
template std::shared_ptr<MeshInstance> g_GetThing<MeshInstance>(id_t ID);
template std::shared_ptr<Material> g_GetThing<Material>(id_t ID);
