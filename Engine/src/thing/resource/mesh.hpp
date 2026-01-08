#ifndef MESH_H
#define MESH_H

#include "fwd/rendering.hpp"
#include "thing/resource/resource.hpp"

class Mesh : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual Farg<Shared<VertexArray>> MeshData() const;
    virtual void MeshData(Shared<VertexArray>);

    virtual ID MaterialID() const;
    virtual void MaterialID(ID);

protected:
    Shared<VertexArray> mVertexArray{nullptr};
    ID mMaterialID{};
};

#endif // MESH_H
