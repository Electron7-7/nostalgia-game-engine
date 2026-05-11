#include "./editor_theatre.hpp"
#include "editor_models.hpp"
#include "things/editor_player_3d.hpp"
#include <Nostalgia/rendering/shader.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/resource_database.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>
#include <Nostalgia/things/resources/material.hpp>
#include <Nostalgia/things/resources/array_mesh.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/thinkers/3d/mesh_instance_3d.hpp>
#include <Nostalgia/things/thinkers/3d/light_3d.hpp>
#include <Nostalgia/settings/graphics.hpp>

using namespace TheatreFile;

bool EditorTheatre::Startup()
{
    m_spCamera3DMesh =
        ArrayMesh::CreateFromMemory(_EditorModels::Camera,
            std::size(_EditorModels::Camera), ArrayMesh::MODEL_OBJ);
    m_spPointLight3DMesh =
        ArrayMesh::CreateFromMemory(_EditorModels::PointLight,
            std::size(_EditorModels::PointLight), ArrayMesh::MODEL_OBJ);
    m_spSpotLight3DMesh  =
        ArrayMesh::CreateFromMemory(_EditorModels::SpotLight,
            std::size(_EditorModels::SpotLight), ArrayMesh::MODEL_OBJ);
    m_sp3DAxisMesh =
        ArrayMesh::CreateFromMemory(_EditorModels::DebugAxis,
            std::size(_EditorModels::DebugAxis), ArrayMesh::MODEL_OBJ);
    m_spPlayer3DMesh =
        ArrayMesh::CreateFromMemory(_EditorModels::Player3D,
            std::size(_EditorModels::Player3D), ArrayMesh::MODEL_OBJ);

    TheatreFile::ThingData cam_dat{ThingType::Camera3D, "EditorCamera3D"};
    cam_dat.set_variable(glm::vec3{1.2f, 6.3f, 5.6f}, "Position");
    cam_dat.set_variable(glm::vec3{-26.8f, 7.7f, 0.0f}, "RotationDegrees");
    cam_dat.set_variable(true, "UseDefaultSkybox");
    cam_dat.set_variable(true, "Current");
    m_pEditorCamera3D->SetVariables(cam_dat);
    m_pEditorCamera3D->Ready();
    m_pEditorCamera2D->Ready();
    m_pEditor3DViewport->Ready();
    m_pEditor2DViewport->Ready();
    m_pEditor3DViewport->SetCurrentCamera3D(m_pEditorCamera3D);
    m_pEditor2DViewport->SetCurrentCamera2D(m_pEditorCamera2D);
    mViewports.insert(m_pEditor3DViewport);
    mViewports.insert(m_pEditor2DViewport);

    return Theatre::Startup();
}

bool EditorTheatre::Shutdown()
{
    m_spCamera3DMesh.reset();
    m_spPointLight3DMesh.reset();
    m_spSpotLight3DMesh.reset();
    m_sp3DAxisMesh.reset();
    m_spPlayer3DMesh.reset();
    return Theatre::Shutdown();
}

void EditorTheatre::Draw()
{
    bool _enable_3d_rendering = Console::GetVariable("Theatre.draw_3d").int_value,
        _enable_2d_rendering = Console::GetVariable("Theatre.draw_2d").int_value,
        _enable_3d_helpers = not Console::GetVariable("Editor.nodraw_3d_helpers").int_value;

    LockGuard<RMutex> _things_lock{mThingsMutex};
    auto* _renderer_api{RendererAPI::Get()};
    auto _fullbright_shader{_renderer_api->GetShader(Shaders::Fullbright)};
    auto _helper_mesh{m_sp3DAxisMesh};

    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { _renderer_api->SetLight_TempBlinnPhongSolution(GetThinker<Light3D>(id)); }

    for(auto& viewport : mViewports)
    {
        viewport->Attach();

        if(_enable_3d_rendering and not viewport->CurrentCamera3D()->invalid())
        {
            viewport->CurrentCamera3D()->DrawBackground();
            for(ID _uid : mVisual3DIDs)
                { viewport->CurrentCamera3D()->Draw(GetThinker<Visual3D>(_uid)); }
        }
        if(_enable_2d_rendering and not viewport->CurrentCamera2D()->invalid())
        {
            for(ID _uid : mVisual2DIDs)
                { viewport->CurrentCamera2D()->Draw(GetThinker<Visual2D>(_uid)); }
        }
        if(not _enable_3d_helpers)
        {
            viewport->Detach();
            continue;
        }

        bool _wireframe{RendererAPI::Get()->GetWireframe()};
        RendererAPI::Get()->SetWireframe(false);
        _fullbright_shader->Bind();
        _fullbright_shader->SetUniform("current_material.use_diffuse", 0);
        _fullbright_shader->SetUniform("current_material.use_specular", 0);
        _fullbright_shader->SetUniform("current_material.diffuse_color", glm::vec3{1.0f});
        _fullbright_shader->SetUniform("current_material.alpha", 1.0f);
        _fullbright_shader->SetUniform("projection_matrix", viewport->CurrentCamera3D()->ProjectionMatrix());
        _fullbright_shader->SetUniform("debug_output",Console::GetVariable("BlinnPhong.debug_visual").int_value);
        _fullbright_shader->SetUniform("view_matrix", viewport->CurrentCamera3D()->ViewMatrix());
        _fullbright_shader->SetUniform("view_position", viewport->CurrentCamera3D()->GlobalPosition());

        for(auto& [uid, thing] : mThings)
        {
            auto _thing3d{DCast<Actor3D>(thing)};
            if(uid == viewport->CurrentCamera3D()->uid()
                or not _thing3d
                or not _enable_3d_rendering
                or viewport->CurrentCamera3D()->invalid())
                { continue; }
            _fullbright_shader->SetUniform("model_matrix", _thing3d->GlobalTransform().model_matrix());
            _fullbright_shader->SetUniform("debug_highlight", _thing3d->DebugHighlight());
            if(thing->DerivedFrom(ThingType::Camera3D))
                { _helper_mesh = m_spCamera3DMesh; }
            else if(thing->DerivedFrom(EditorPlayer3D::sClassType()))
                { _helper_mesh = m_spPlayer3DMesh; }
            else if(thing->DerivedFrom(ThingType::Light3D)
                and not thing->DerivedFrom(ThingType::DirectionalLight3D))
            {
                _helper_mesh = (thing->DerivedFrom(ThingType::SpotLight3D))
                    ? m_spSpotLight3DMesh : m_spPointLight3DMesh;
                auto _light{DCast<Light3D>(thing)};
                auto _transform{_light->GlobalTransform()};
                _transform.scale *= glm::vec3{0.4f};
                _fullbright_shader->SetUniform("model_matrix", _transform.model_matrix());
                _fullbright_shader->SetUniform("current_material.diffuse_color", _light->mColor);
            }
            else
                { continue; }
            for(int i{0}; i < _helper_mesh->SurfaceCount(); ++i)
                { _renderer_api->DrawIndexed(_helper_mesh->SurfaceGetVertexArray(i)); }
        }

        _fullbright_shader->Unbind();
        RendererAPI::Get()->SetWireframe(_wireframe);
        viewport->Detach();
    }
}
