#ifndef MESH_INSTANCE_3D_H
#define MESH_INSTANCE_3D_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>

class MeshInstance3D : public Visual3D
{
public:
	SET_SUPER(Visual3D)
	SET_TYPEID(ThingType::MeshInstance3D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual ID MeshID() const;
    virtual ID MaterialOverrideID() const;
    virtual bool Wireframe() const;

    virtual void SetMeshID(ID);
    virtual void SetMaterialOverrideID(ID);
    virtual void SetWireframe(bool);

protected:
    ID mMeshID{};
    ID mMaterialOverrideID{};
    bool mWireframe{false};
};

#endif // MESH_INSTANCE_3D_H
