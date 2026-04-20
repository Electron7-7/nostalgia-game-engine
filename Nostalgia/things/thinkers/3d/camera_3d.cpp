#include "./camera_3d.hpp"
#include "../viewport.hpp"
#include "things/thing_data.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/array_mesh.hpp"
#include "theatre/theatre.hpp"
#include "settings/world.hpp"
#include "managers/render_manager.hpp"
#include "rendering/shader.hpp"
#include "theatre/theatre.hpp"
#include "things/resource_database.hpp"

using namespace TheatreFile;

void Camera3D::Ready()
{
    Super::Ready();

    auto parent{Theatre::Current()->GetParent(uid())};

    if(mViewportID.invalid())
    {
        if(not parent.invalid())
        {
            if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                { mViewportID = parent; }
            else
            {
                auto ancestors{Theatre::Current()->GetAllParents(uid())};
                for(ID parent : ancestors)
                {
                    if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                        { mViewportID = parent; break; }
                    else if(Theatre::Current()->DerivedFrom(parent, ThingType::NostalgiaPlayer))
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
        mEnvironment.mType = Environment::BG_SKYBOX;
        mEnvironment.mSkyboxTextureID = UID::t_ShittySkybox;
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
    data.get_variable(mEnvironment.mCustomColorAlpha,
        "EnvironmentColorAlpha", "EnvironmentAlpha");
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mLayersMask, "RenderLayersMask");
    data->set_variable(mFOV, "FOV");
    data->set_variable(mViewCutoffNear, "Near");
    data->set_variable(mViewCutoffFar, "Far");
    data->set_variable(mInitCurrent, "Current");
    if(mEnvironment.mType == Environment::BG_SKYBOX
        and mEnvironment.mSkyboxTextureID == UID::t_ShittySkybox)
        { data->set_variable(true, "UseDefaultSkybox"); }
    if(mEnvironment.mType == Environment::BG_CUSTOM_COLOR)
        { data->set_variable(mEnvironment.mCustomColor.glm(), "EnvironmentColor"); }
    data->set_variable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha");

    return data;
}

void Camera3D::DrawBackground() const
{
    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    switch(mEnvironment.mType)
    {
    case Environment::BG_SKYBOX:
        {
#pragma message("TODO: move all Resource creation to the ResourceDatabase, even the ones made by a Theatre")
            renderer_api->BindTexture(Theatre::Current()
                ->GetResource<Texture>(mEnvironment.mSkyboxTextureID), 0);
            renderer_api->GetShader(Shaders::SkyBox)->Bind();
            renderer_api->GetShader(Shaders::SkyBox)
                ->SetUniform("view_matrix", glm::mat4{glm::mat3{ViewMatrix()}});
            renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", ProjectionMatrix());
            renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
            renderer_api->SetDepthMask(false);
            auto _cube{ResourceDatabase::GetResource<ArrayMesh>(UID::m_Cube)};
            for(int i{0}; i < _cube->SurfaceCount(); ++i)
                { renderer_api->DrawIndexed(_cube->SurfaceGetVertexArray(i)); }
            renderer_api->SetDepthMask(true);
            return;
        }
    case Environment::BG_CUSTOM_COLOR:
        renderer_api->SetClearColor(mEnvironment.get_custom_color().glm());
        break;
    case Environment::BG_CLEAR_COLOR:
        renderer_api->SetClearColor(Settings::Graphics::ClearColor.glm());
        break;
    }
    renderer_api->Clear();
}

ID Camera3D::ViewportID() const
{ return mViewportID; }

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
        static_cast<float>(Theatre::Current()
            ->GetThinker<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}

void Camera3D::OnAncestorRemoved(Relative inAncestor)
{
    Super::OnAncestorRemoved(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = {}; }
}

void Camera3D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
