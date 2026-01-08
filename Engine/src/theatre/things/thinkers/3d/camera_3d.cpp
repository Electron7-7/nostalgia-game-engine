#include "camera_3d.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"

void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.GetVariable(mFOV, "FOV");
    data.GetVariable(mViewCutoffNear, "Near", "CutoffNear");
    data.GetVariable(mViewCutoffFar, "Far", "CutoffFar");
    data.GetVariable(mViewportID, "Viewport", "ViewportID");
    if(bool use_env{false};
        data.GetVariable(use_env, "UseDefaultSkybox"))
    {
        mEnvironment.mType = Environment::BG_SKYBOX;
        mEnvironment.mSkyboxTextureID = UID::t_ShittySkybox;
    }
    data.GetVariable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");
    if(data.GetVariable(mEnvironment.mSkyboxTextureID, "EnvironmentSkybox"))
        { mEnvironment.mType = Environment::BG_SKYBOX; }
    if(glm::vec3 color{(glm::vec3)mEnvironment.mCustomColor};
        data.GetVariable(color, "EnvironmentColor"))
    {
        mEnvironment.mType = Environment::BG_SKYBOX;
        mEnvironment.mCustomColor = color;
    }
    data.GetVariable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha", "EnvironmentAlpha");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->AddVariable(mFOV, "FOV");
    data->AddVariable(mViewCutoffNear, "Near");
    data->AddVariable(mViewCutoffFar, "Far");
    data->AddVariable(mViewportID, "Viewport");
    data->AddVariable(mInitCurrent, "Current");
    if(mEnvironment.mType == Environment::BG_SKYBOX
        and mEnvironment.mSkyboxTextureID == UID::t_ShittySkybox)
        { data->AddVariable(true, "UseDefaultSkybox"); }
    if(mEnvironment.mType == Environment::BG_CUSTOM_COLOR)
        { data->AddVariable((glm::vec3)mEnvironment.mCustomColor, "EnvironmentColor"); }
    data->AddVariable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha");

    return data;
}

void Camera3D::Tick()
{}

void Camera3D::Shutdown()
{ g_pTheatreManager->GetThing<Viewport>(mViewportID)->EraseCamera(mUID); }

void Camera3D::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(mDebugMeshInstanceID.invalid() and Settings::Engine::IsEditorHint)
    {
        ID mat_id{"Camera3D-debug-material"};
        ID mesh_id{"Camera3D-debug-mesh-instance"};
        if(!g_pTheatreManager->ThingExists(mat_id))
        {
            g_pTheatreManager->CreateThing({
                "Camera3D-debug-material",
                ThingType::Material,
                mat_id,
                {
                    {true, "FullBright"},
                    {true, "NoTextures"},
                    {glm::vec3{200.0f, 80.0f, 255.0f}, "Color"},
                }
            });
        }
        if(!g_pTheatreManager->ThingExists(mesh_id))
        {
            mDebugMeshInstanceID = g_pTheatreManager->CreateThing({
                "Camera3D-debug-mesh-instance",
                ThingType::MeshInstance3D,
                mesh_id,
                {
                    {UID::m_Camera3D, "Mesh"},
                    {mat_id, "MaterialOverride"},
                }
            });
        }
        mDebugMeshInstanceID = mesh_id;
    }

    auto viewport{g_pTheatreManager->GetThing<Viewport>(mViewportID)};
    viewport->AddCamera(mUID);
    if(mInitCurrent)
        { viewport->SetCurrentCamera(mUID); }
}

bool Camera3D::Current() const
{ return g_pTheatreManager->GetThing<Viewport>(mViewportID)->IsCurrentCamera(mUID); }

ID Camera3D::ViewportID() const
{ return mViewportID; }

BitMask Camera3D::LayersMask() const
{ return mLayersMask; }

bool Camera3D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return isCurrent; }
    else if(isCurrent)
        { return g_pTheatreManager->GetThing<Viewport>(mViewportID)->SetCurrentCamera(mUID) == OK; }
    g_pTheatreManager->GetThing<Viewport>(mViewportID)->SetCurrentCamera(ID::Invalid);
    return false;
}

Error Camera3D::SetViewportID(ID inID)
{
    if(auto viewport{g_pTheatreManager->GetThing<Viewport>(inID)})
    {
        mViewportID = inID;
        viewport->AddCamera(mUID);
        return OK;
    }
    return ERR_INVALID_ID;
}

void Camera3D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

glm::mat4 Camera3D::ViewMatrix() const
{
    return glm::lookAt(mPosition,
        mPosition + (mQuaternion * Settings::World::Front()),
        mQuaternion * Settings::World::Up());
}

glm::mat4 Camera3D::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(mFOV),
        static_cast<float>(g_pTheatreManager->GetThing<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}
