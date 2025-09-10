#include "resources.hpp"
#include "printing.hpp"
#include "basic/mesh.hpp"
#include "basic/font.hpp"
#include "basic/texture.hpp"
#include "complex/material.hpp"
#include "complex/mesh_instance.hpp"

#include <map>

template<IsResource T>
std::shared_ptr<Resource> l_MakeResource()
{ return std::make_shared<T>(); }

inline const std::map<size_t, std::shared_ptr<Resource>(*)()>
s_ResourceConstructors =
{
    { Type::Resource,     &l_MakeResource<Resource>     },
    { Type::Mesh,         &l_MakeResource<Mesh>         },
    { Type::Font,         &l_MakeResource<Font>         },
    { Type::Material,     &l_MakeResource<Material>     },
    { Type::Texture,      &l_MakeResource<Texture>      },
    { Type::MeshInstance, &l_MakeResource<MeshInstance> },
};

std::shared_ptr<Resource>(*g_MakeResource(size_t type))()
{
    if(!s_ResourceConstructors.contains(type))
    {
        PRINT_WARNING("g_MakeResource - Type '{}' is an invalid type! An empty Resource will be made, instead", type)
        return &l_MakeResource<Resource>;
    }
    return s_ResourceConstructors.at(type);
}
