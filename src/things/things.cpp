#include "things.hpp"
#include "printing.hpp"
// #include "thinkers/thinker.hpp"
#include "actors/actor.hpp"
#include "actors/nostalgia_player.hpp"
#include "actors/prototype_actor.hpp"
#include "actors/camera_3d.hpp"
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
    // { Type::Thinker,         &l_MakeThing<Thinker>         },
    { Type::Actor,           &l_MakeThing<Actor>           },
    { Type::NostalgiaPlayer, &l_MakeThing<NostalgiaPlayer> },
    { Type::PrototypeActor,  &l_MakeThing<PrototypeActor>  },
    { Type::Camera3D,        &l_MakeThing<Camera3D>        },
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
