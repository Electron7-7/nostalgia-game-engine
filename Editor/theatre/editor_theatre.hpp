#ifndef EDITOR_THEATRE_H
#define EDITOR_THEATRE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/things/thinkers/2d/camera_2d.hpp>

class EditorTheatre : public Theatre
{
public:
    virtual bool Startup() final;
    virtual bool Shutdown() final;
    virtual void Update() final {}
    virtual void Tick() final {}
    virtual void Input(InputEvent*) final {}
    virtual void Draw() final;

    Shared<Camera3D> m_pEditorCamera3D{ThingFactory::MakeThing<Camera3D>("Editor-Camera3D")};
    Shared<Camera2D> m_pEditorCamera2D{ThingFactory::MakeThing<Camera2D>("Editor-Camera2D")};
    Shared<Viewport> m_pEditor3DViewport{ThingFactory::MakeThing<Viewport>("Editor3D-Viewport")},
        m_pEditor2DViewport{ThingFactory::MakeThing<Viewport>("Editor2D-Viewport")};

private:
    inline static Shared<Mesh> m_spCamera3DMesh{nullptr},
        m_spPointLight3DMesh{nullptr},
        m_spSpotLight3DMesh{nullptr},
        m_sp3DAxisMesh{nullptr},
        m_spPlayer3DMesh{nullptr};
};

#endif // EDITOR_THEATRE_H
