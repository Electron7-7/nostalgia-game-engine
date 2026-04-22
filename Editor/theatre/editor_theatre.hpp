#ifndef EDITOR_THEATRE_H
#define EDITOR_THEATRE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/theatre/theatre.hpp>
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

    void SetEditorViewports(ID in3DViewportUID, ID in2DViewportUID);

    Shared<Camera3D> m_pEditorCamera3D{MakeShared<Camera3D>("Editor-Camera3D")};
    Shared<Camera2D> m_pEditorCamera2D{MakeShared<Camera2D>("Editor-Camera2D")};
    Shared<Viewport> m_pEditor3DViewport{MakeShared<Viewport>("Editor3D-Viewport")},
        m_pEditor2DViewport{MakeShared<Viewport>("Editor2D-Viewport")};

private:
    inline static Shared<Mesh> m_spCamera3DMesh{nullptr},
        m_spPointLight3DMesh{nullptr},
        m_spSpotLight3DMesh{nullptr},
        m_spPlayerAxis3DMesh{nullptr};

    void DrawCamera2DHelper(Shared<Camera2D>, Shared<Actor2D>);
    void DrawCamera3DHelper(Shared<Camera3D>, Shared<Actor3D>);
    void DrawPlayer3DHelper(Shared<Camera3D>, Shared<Actor3D>);
    void DrawLight3DHelper(Shared<Camera3D>, Shared<Light3D>);
};

#endif // EDITOR_THEATRE_H
