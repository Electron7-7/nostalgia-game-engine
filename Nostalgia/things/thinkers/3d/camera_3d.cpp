#include "./camera_3d.hpp"
#include "./light_3d.hpp"
#include "./mesh_instance_3d.hpp"
#include "./sprite_3d.hpp"
#include "../viewport.hpp"
#include "../../thing_data.hpp"
#include "../../resources/texture.hpp"
#include "../../resources/array_mesh.hpp"
#include "../../thing_factory.hpp"
#include "theatre/theatre.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "settings/world.hpp"
#include "application/application.hpp"

using namespace TheatreFile;

void Camera3D::InitVariables()
{
    Super::InitVariables();
}

void Camera3D::Ready()
{
    Super::Ready();

    auto parent{Theatre::Current()->GetParent(uid())};

    if(mViewportID.invalid())
    {
        if(not parent.invalid())
        {
            if(ThingFactory::DerivedFrom(parent, ThingType::Viewport))
                { mViewportID = parent; }
            else
            {
                auto ancestors{Theatre::Current()->GetAllParents(uid())};
                for(ID parent : ancestors)
                {
                    if(ThingFactory::DerivedFrom(parent, ThingType::Viewport))
                        { mViewportID = parent; break; }
                    else if(ThingFactory::DerivedFrom(parent, ThingType::NostalgiaPlayer))
                        { mInitCurrent = true; }
                }
            }
        }
    }

    if(mInitCurrent)
        { Theatre::Current()->SetCurrentCamera(uid(), mViewportID); }
}

void Camera3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mLayersMask, "RenderLayersMask", "LayersMask", "RenderLayers");
    data.get_variable(mFOV, "FOV");
    data.get_variable(mViewCutoffNear, "Near", "CutoffNear");
    data.get_variable(mViewCutoffFar, "Far", "CutoffFar");
    if(bool use_default_skybox{false}; data.get_variable(use_default_skybox, "UseDefaultSkybox") == OK)
    {
        if(use_default_skybox)
        {
            mEnvironment.mType = Environment::BG_SKYBOX;
            mEnvironment.mSkyboxTextureID = UID::t_ShittySkybox;
        }
        else
        { mEnvironment.mType = Environment::BG_CLEAR_COLOR; }
    }
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");
    if(data.get_variable(mEnvironment.mSkyboxTextureID, "EnvironmentSkybox") == OK)
        { mEnvironment.mType = Environment::BG_SKYBOX; }
    if(glm::vec3 color{mEnvironment.mCustomColor.glm()};
        data.get_variable(color, "EnvironmentColor") == OK)
    {
        mEnvironment.mType = Environment::BG_CUSTOM_COLOR;
        mEnvironment.mCustomColor = color;
    }
    data.get_variable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha", "EnvironmentAlpha");
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mLayersMask, "RenderLayersMask");
    data->set_variable(mFOV, "FOV");
    data->set_variable(mViewCutoffNear, "Near");
    data->set_variable(mViewCutoffFar, "Far");
    data->set_variable(mInitCurrent, "Current");
    data->set_variable((mEnvironment.mType == Environment::BG_SKYBOX
        and mEnvironment.mSkyboxTextureID == UID::t_ShittySkybox), "UseDefaultSkybox");
    if(mEnvironment.mType == Environment::BG_CUSTOM_COLOR)
        { data->set_variable(mEnvironment.mCustomColor.glm(), "EnvironmentColor"); }
    data->set_variable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha");

    return data;
}

void Camera3D::DrawBackground() const
{
    bool _nodraw(Console::GetVariable("nodraw_3d").int_value);
    bool _drawbg(Console::GetVariable("debug.draw_3d_bg").int_value);
    if(_nodraw and not _drawbg)
        { return; }
    FAUTO renderer_api{RendererAPI::Get()};
    if(mEnvironment.mType == Environment::BG_SKYBOX)
    {
#pragma message("TODO: move all Resource creation to the ResourceDatabase, even the ones made by a Theatre")
        renderer_api->BindTexture(ThingFactory::GetThing<Texture>(mEnvironment.mSkyboxTextureID), 0);
        renderer_api->GetShader(Shaders::SkyBox)->Bind();
        renderer_api->GetShader(Shaders::SkyBox)
            ->SetUniform("view_matrix", glm::mat4{glm::mat3{ViewMatrix()}});
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", ProjectionMatrix());
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
        renderer_api->SetWireframe(false);
        renderer_api->SetDepthMask(false);
        auto _cube{ThingFactory::GetThing<ArrayMesh>(UID::m_Cube)};
        for(int i{0}; i < _cube->SurfaceCount(); ++i)
            { renderer_api->DrawIndexed(_cube->SurfaceGetVertexArray(i)); }
        renderer_api->SetDepthMask(true);
        return;
    }
    renderer_api->SetClearColor((mEnvironment.mType == Environment::BG_CUSTOM_COLOR)
        ? mEnvironment.get_custom_color() : Settings::Graphics::ClearColor);
    renderer_api->Clear();
}

void Camera3D::Draw(Shared<Visual3D> inVisual3D) const
{
    if(Console::GetVariable("nodraw_3d").int_value)
        { return; }
    FAUTO renderer_api{RendererAPI::Get()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    auto view_matrix{ViewMatrix()};
    auto projection_matrix{ProjectionMatrix()};

    auto missing_texture{ThingFactory::GetThing<Texture>(UID::t_Missing)};
    auto mesh{ThingFactory::GetThing<ArrayMesh>(UID::m_Error)};

    if(not inVisual3D->Visible()
        or inVisual3D->DerivedFrom(ThingType::Light3D)
        or not LayersMask().contains(inVisual3D->Layers()))
            { return; }

    glm::vec3 scale_vector{inVisual3D->GlobalScale()};

    if(inVisual3D->DerivedFrom(ThingType::MeshInstance3D))
    {
        auto mesh_instance{DCast<MeshInstance3D>(inVisual3D)};
        if(DCast<ArrayMesh>(mesh_instance->GetMesh()))
            { mesh = DCast<ArrayMesh>(mesh_instance->GetMesh()); }
        auto material{mesh->mMaterial};

        if(auto _mat{mesh_instance->GetMaterialOverride()}; _mat
            and not _mat->invalid())
            { material = mesh_instance->GetMaterialOverride(); }

        auto diffuse_texture{material->DiffuseTexture()};
        auto specular_texture{material->SpecularTexture()};

        if(not material->DiffuseTexture()->invalid() and not diffuse_texture->Buffer())
            { diffuse_texture = missing_texture; }

        if(not material->SpecularTexture()->invalid() and not specular_texture->Buffer())
            { specular_texture = missing_texture; }

        shader = renderer_api->GetShader((material->mFullBright)
            ? Shaders::Fullbright
            : Shaders::BlinnPhong);

        bool use_diffuse  {renderer_api->BindTexture(diffuse_texture,  0)};
        bool use_specular {renderer_api->BindTexture(specular_texture, 1)};

        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or mesh_instance->Wireframe());
        renderer_api->SetFramebufferSRGB(use_diffuse or use_specular);

        shader->SetUniform("current_material.texture_diffuse",  0);
        shader->SetUniform("current_material.texture_specular", 1);
        shader->SetUniform("current_material.use_diffuse",  use_diffuse);
        shader->SetUniform("current_material.use_specular", use_specular);
        shader->SetUniform("current_material.diffuse_color", material->mColor);
        shader->SetUniform("current_material.alpha", material->mAlpha);
        shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
        shader->SetUniform("current_material.specular_strength", material->SpecularStrength());
    }
    else if(inVisual3D->DerivedFrom(ThingType::Sprite3D))
    {
        auto sprite{DCast<Sprite3D>(inVisual3D)};
        mesh = ThingFactory::GetThing<ArrayMesh>(UID::m_Quad);

        auto texture{(not sprite->GetTexture())
            ? missing_texture
            : sprite->GetTexture()};

        shader = renderer_api->GetShader(Shaders::Fullbright);

        renderer_api->BindTexture(texture, 0);
        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe);
        renderer_api->SetFramebufferSRGB(true);

        shader->SetUniform("current_material.texture_diffuse", 0);
        shader->SetUniform("current_material.use_diffuse", true);
        shader->SetUniform("current_material.diffuse_color", {1.0f,1.0f,1.0f});

        auto height{texture->Buffer()->GetFormat().height};
        auto width{texture->Buffer()->GetFormat().width};
        if(width > height)
            { scale_vector *= glm::vec3{1.0f / ((float)height / width), 1.0f, 1.0f}; }
        else
            { scale_vector *= glm::vec3{1.0f, 1.0f / ((float)height / width), 1.0f}; }
    }

    if(not mesh->SurfaceCount())
        { mesh = ThingFactory::GetThing<ArrayMesh>(UID::m_Error); }

    glm::mat4 model_matrix{inVisual3D->GlobalTransform().model_matrix()};

    shader->SetUniform("model_matrix", model_matrix);
    shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
    shader->SetUniform("projection_matrix", projection_matrix);
    shader->SetUniform("debug_output", Console::GetVariable("BlinnPhong.debug_visual").int_value);
    shader->SetUniform("point_lights_count", PointLight3D::GetCount());
    shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
    shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
    shader->SetUniform("view_matrix", view_matrix);
    shader->SetUniform("view_position", GlobalPosition());
    shader->SetUniform("debug_highlight", inVisual3D->DebugHighlight());

    shader->Bind();
    for(int i{0}; i < mesh->SurfaceCount(); ++i)
        { renderer_api->DrawIndexed(mesh->SurfaceGetVertexArray(i)); }
    renderer_api->SetFramebufferSRGB(false);
}

ID Camera3D::ViewportID() const
{ return mViewportID; }

void Camera3D::SetViewportID(ID inUID)
{ mViewportID = inUID; }

bool Camera3D::Current() const
{ return Theatre::Current()->GetCurrentCamera3D(mViewportID) == uid(); }

Error Camera3D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return OK; }
    return Theatre::Current()->SetCurrentCamera(uid(), mViewportID);
}

BitMask Camera3D::LayersMask() const
{ return mLayersMask; }

void Camera3D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

glm::mat4 Camera3D::ViewMatrix() const
{
    glm::quat global_quat{GlobalRotation()};
    return glm::lookAt(GlobalPosition(),
        GlobalPosition() + (global_quat * Settings::World::Front()),
        global_quat * Settings::World::Up());
}

glm::mat4 Camera3D::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(mFOV),
        static_cast<float>((mViewportID.invalid())
            ? MainWindow()->GetScale().AspectRatio()
            : ThingFactory::GetThing<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}

void Camera3D::OnAncestorRemoved(Relative inAncestor)
{
    Super::OnAncestorRemoved(inAncestor);
    if(ThingFactory::DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = {}; }
}

void Camera3D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(ThingFactory::DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
