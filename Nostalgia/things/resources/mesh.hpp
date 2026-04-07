#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/things/resources/resource.hpp>

class Mesh : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Mesh)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    enum ModelType : int
    { MODEL_OBJ };

    // All Mesh-derived classes MUST override this function (pure virtual classes aren't supported, yet)
    virtual Shared<VertexArray> MeshData() const;

    virtual ID MaterialID() const;
    virtual void MaterialID(ID);

protected:
    ID mMaterialID{};
};

#endif // MESH_H
