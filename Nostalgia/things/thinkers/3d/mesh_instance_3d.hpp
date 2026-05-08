#ifndef MESH_INSTANCE_3D_H
#define MESH_INSTANCE_3D_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>
#include <Nostalgia/things/resources/material.hpp>
#include <Nostalgia/things/resources/mesh.hpp>

class MeshInstance3D : public Visual3D
{
public:
	THING_CLASS(MeshInstance3D, Visual3D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<Mesh> GetMesh() const;
    virtual Shared<Material> GetMaterialOverride() const;
    virtual bool Wireframe() const;

    virtual void SetMesh(Shared<Mesh>);
    virtual void SetMaterialOverride(Shared<Material>);
    virtual void SetWireframe(bool);

protected:
    Shared<Mesh> mMesh{nullptr};
    Shared<Material> mMaterialOverride{nullptr};
    bool mWireframe{false};
};

#endif // MESH_INSTANCE_3D_H
