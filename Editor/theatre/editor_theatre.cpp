#include "./editor_theatre.hpp"
#include "things/editor_player_3d.hpp"
#include <Nostalgia/rendering/shader.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/resource_database.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/things/resources/material.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/thinkers/3d/mesh_instance_3d.hpp>
#include <Nostalgia/things/thinkers/3d/light_3d.hpp>
#include <Nostalgia/settings/graphics.hpp>

using namespace TheatreFile;

bool EditorTheatre::Startup()
{
    m_spCamera3DMesh = ResourceDatabase::GetResource<Mesh>(UID::m_Camera3D);
    m_spPointLight3DMesh = ResourceDatabase::GetResource<Mesh>(UID::m_PointLight3D);
    m_spSpotLight3DMesh = ResourceDatabase::GetResource<Mesh>(UID::m_SpotLight3D);
    m_spPlayerAxis3DMesh = ResourceDatabase::GetResource<Mesh>(UID::m_DebugAxis);

    return Theatre::Startup();
}

bool EditorTheatre::Shutdown()
{
    m_spCamera3DMesh.reset();
    m_spPointLight3DMesh.reset();
    m_spSpotLight3DMesh.reset();
    m_spPlayerAxis3DMesh.reset();
    return Theatre::Shutdown();
}

void EditorTheatre::Draw()
{
    bool _enable_3d_rendering = Console::GetVariable("Theatre.draw_3d")->int_value,
        _enable_2d_rendering = Console::GetVariable("Theatre.draw_2d")->int_value;

    LockGuard<RMutex> _things_lock{mThingsMutex};

    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(GetThinker<Light3D>(id)); }

    for(ID viewport_id : mViewportIDs)
    {
        auto viewport{GetThinker<Viewport>(viewport_id)};

        viewport->Attach();

        auto _camera3d{GetThinker<Camera3D>(viewport->CurrentCamera3D())};
        auto _camera2d{GetThinker<Camera2D>(viewport->CurrentCamera2D())};

        if(_enable_3d_rendering and not _camera3d->Invalid())
        {
            _camera3d->DrawBackground();
            for(ID _uid : mVisual3DIDs)
                { _camera3d->Draw(GetThinker<Visual3D>(_uid)); }
        }
        if(_enable_2d_rendering and not _camera2d->Invalid())
        {
            for(ID _uid : mVisual2DIDs)
                { _camera2d->Draw(GetThinker<Visual2D>(_uid)); }
        }

        for(auto& [uid, thing] : mThings)
        {
            if(uid == _camera3d->uid() or uid == _camera2d->uid())
                { continue; }
            FPID _type{thing->Type()};
            if(_enable_2d_rendering and not _camera2d->Invalid())
            {
                if(ThingFactory::IsDerivedFrom(_type, ThingType::Camera2D))
                    { DrawCamera2DHelper(_camera2d, DCast<Camera2D>(thing)); }
            }
            if(_enable_3d_rendering and not _camera3d->Invalid())
            {
                if(ThingFactory::IsDerivedFrom(_type, ThingType::Camera3D))
                    { DrawCamera3DHelper(_camera3d, DCast<Camera3D>(thing)); }
                else if(ThingFactory::IsDerivedFrom(_type, EditorPlayer3D::__editorplayer3d))
                    { DrawPlayer3DHelper(_camera3d, DCast<Actor3D>(thing)); }
                else if(ThingFactory::IsDerivedFrom(_type, ThingType::Light3D)
                        and not ThingFactory::IsDerivedFrom(_type, ThingType::DirectionalLight3D))
                    { DrawLight3DHelper(_camera3d, DCast<Light3D>(thing)); }
            }
        }

        viewport->Detach();
    }
}

void EditorTheatre::SetEditorViewports(ID in3DViewportUID, ID in2DViewportUID)
{
    LockGuard<RMutex> _things_lock{mThingsMutex};
    if(mViewportIDs.contains(in3DViewportUID))
    {
        m_pEditor3DViewport = GetThinker<Viewport>(in3DViewportUID);
        m_pEditorCamera3D = GetThinker<Camera3D>(m_pEditor3DViewport->CurrentCamera3D());
    }
    if(mViewportIDs.contains(in2DViewportUID))
    {
        m_pEditor2DViewport = GetThinker<Viewport>(in2DViewportUID);
        m_pEditorCamera2D = GetThinker<Camera2D>(m_pEditor2DViewport->CurrentCamera2D());
    }
}

void EditorTheatre::DrawCamera2DHelper(Shared<Camera2D> inCamera, Shared<Actor2D> inOtherCamera)
{
    // LockGuard<RMutex> _things_lock{mThingsMutex};
    return;
}

void EditorTheatre::DrawCamera3DHelper(Shared<Camera3D> inCamera, Shared<Actor3D> inOtherCamera)
{
    LockGuard<RMutex> _things_lock{mThingsMutex};
    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    shader->SetUniform("current_material.use_diffuse", 0);
    shader->SetUniform("current_material.use_specular", 0);
    shader->SetUniform("current_material.diffuse_color", {200.0f, 80.0f, 255.0f});
    shader->SetUniform("current_material.alpha", 1.0f);
    shader->SetUniform("model_matrix", inOtherCamera->GlobalTransform().model_matrix());
    shader->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
    shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
    shader->SetUniform("view_matrix", inCamera->ViewMatrix());
    shader->SetUniform("view_position", inCamera->GlobalPosition());
    shader->SetUniform("debug_highlight", inOtherCamera->DebugHighlight());
    shader->Bind();
    for(int i{0}; i < m_spCamera3DMesh->SurfaceCount(); ++i)
        { renderer_api->DrawIndexed(m_spCamera3DMesh->SurfaceGetVertexArray(i)); }
}

void EditorTheatre::DrawPlayer3DHelper(Shared<Camera3D> inCamera, Shared<Actor3D> inPlayer)
{
    LockGuard<RMutex> _things_lock{mThingsMutex};
    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    shader->SetUniform("current_material.use_diffuse", 0);
    shader->SetUniform("current_material.use_specular", 0);
    shader->SetUniform("current_material.diffuse_color", glm::vec3{1.0f});
    shader->SetUniform("current_material.alpha", 1.0f);
    shader->SetUniform("model_matrix", inPlayer->GlobalTransform().model_matrix());
    shader->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
    shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
    shader->SetUniform("view_matrix", inCamera->ViewMatrix());
    shader->SetUniform("view_position", inCamera->GlobalPosition());
    shader->SetUniform("debug_highlight", inPlayer->DebugHighlight());
    shader->Bind();
    for(int i{0}; i < m_spPlayerAxis3DMesh->SurfaceCount(); ++i)
        { renderer_api->DrawIndexed(m_spPlayerAxis3DMesh->SurfaceGetVertexArray(i)); }
}

void EditorTheatre::DrawLight3DHelper(Shared<Camera3D> inCamera, Shared<Light3D> inLight)
{
    LockGuard<RMutex> _things_lock{mThingsMutex};
    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    auto _transform{inLight->GlobalTransform()};
    _transform.scale *= glm::vec3{0.4f};
    shader->SetUniform("current_material.use_diffuse", 0);
    shader->SetUniform("current_material.use_specular", 0);
    shader->SetUniform("current_material.diffuse_color", inLight->mColor);
    shader->SetUniform("current_material.alpha", 1.0f);
    shader->SetUniform("model_matrix", _transform.model_matrix());
    shader->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
    shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
    shader->SetUniform("view_matrix", inCamera->ViewMatrix());
    shader->SetUniform("view_position", inCamera->GlobalPosition());
    shader->SetUniform("debug_highlight", inLight->DebugHighlight());
    shader->Bind();
    Shared<Mesh> _mesh{m_spPointLight3DMesh};
    if(ThingFactory::IsDerivedFrom(inLight->Type(), ThingType::SpotLight3D))
        { _mesh = m_spSpotLight3DMesh; }
    for(int i{0}; i < _mesh->SurfaceCount(); ++i)
        { renderer_api->DrawIndexed(_mesh->SurfaceGetVertexArray(i)); }
}
