#include "things.hpp"
#include "printing.hpp"
// #include "thinkers/thinker.hpp"
#include "actors/actor.hpp"
#include "actors/nostalgia_player.hpp"
#include "actors/prototype_actor.hpp"
#include "actors/camera_3d.hpp"

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
