#ifndef MESH_H
#define MESH_H

#include <Nostalgia/fwd/rendering.hpp>

class Mesh : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual Farg<Shared<VertexArray>> MeshData() const;
    virtual void MeshData(Shared<VertexArray>);

    virtual ID MaterialID() const;
    virtual void MaterialID(ID);

protected:
    Shared<VertexArray> mVertexArray{nullptr};
    ID mMaterialID{};
};

#endif // MESH_H
