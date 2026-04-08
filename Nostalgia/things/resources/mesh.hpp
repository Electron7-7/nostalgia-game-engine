#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/things/resources/resource.hpp>

class Mesh : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Mesh)

    virtual int SurfaceCount() const { return 0; }
};

#endif // MESH_H
