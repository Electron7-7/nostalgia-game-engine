#include "camera_3d.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    data.get_variable(mFOV, "FOV");
    data.get_variable(mViewCutoffNear, "Near", "CutoffNear");
    data.get_variable(mViewCutoffFar, "Far", "CutoffFar");
    data.get_variable(mViewportID, "Viewport", "ViewportID");
    if(bool use_env{false};
        data.get_variable(use_env, "UseDefaultSkybox") == OK)
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
        mEnvironment.mType = Environment::BG_SKYBOX;
        mEnvironment.mCustomColor = color;
    }
    data.get_variable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha", "EnvironmentAlpha");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->set_variable(mFOV, "FOV");
    data->set_variable(mViewCutoffNear, "Near");
    data->set_variable(mViewCutoffFar, "Far");
    data->set_variable(mViewportID, "Viewport");
    data->set_variable(mInitCurrent, "Current");
    if(mEnvironment.mType == Environment::BG_SKYBOX
        and mEnvironment.mSkyboxTextureID == UID::t_ShittySkybox)
        { data->set_variable(true, "UseDefaultSkybox"); }
    if(mEnvironment.mType == Environment::BG_CUSTOM_COLOR)
        { data->set_variable(mEnvironment.mCustomColor.glm(), "EnvironmentColor"); }
    data->set_variable(mEnvironment.mCustomColorAlpha, "EnvironmentColorAlpha");

    return data;
}

void Camera3D::Tick()
{}

void Camera3D::Shutdown()
{ g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->EraseCamera(mUID); }

void Camera3D::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(mDebugMeshInstanceID.invalid() and Settings::Engine::IsEditorHint)
    {
        ThingData mat_data{ThingType::Material,{mName + "-debug-material"}};
        mat_data.set_variable(true, "FullBright");
        mat_data.set_variable(true, "NoTextures");
        mat_data.set_variable(glm::vec3{200.0f, 80.0f, 255.0f}, "Color");
        ID mat_id{g_pTheatreManager->CurrentTheatre()->CreateThing(mat_data)};

        ThingData mesh_inst_data{ThingType::MeshInstance3D,{mName + "-debug-mesh-instance"}};
        print_error_enum(mesh_inst_data.set_variable(UID::m_Camera3D, "Mesh"));
        print_error_enum(mesh_inst_data.set_variable(mat_id, "MaterialOverride"));
        ID mesh_id{g_pTheatreManager->CurrentTheatre()->CreateThing(mesh_inst_data)};
        mDebugMeshInstanceID = mesh_id;
    }
    add_child({mDebugMeshInstanceID, ThingType::MeshInstance3D});
    mVisible = false;

    auto viewport{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)};
    viewport->AddCamera(mUID);
    if(mInitCurrent)
        { viewport->SetCurrentCamera(mUID); }
}

bool Camera3D::Current() const
{ return g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->IsCurrentCamera(mUID); }

ID Camera3D::ViewportID() const
{ return mViewportID; }

BitMask Camera3D::LayersMask() const
{ return mLayersMask; }

bool Camera3D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return isCurrent; }
    else if(isCurrent)
        { return g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera(mUID) == OK; }
    g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera(ID::Invalid);
    return false;
}

Error Camera3D::SetViewportID(ID inID)
{
    if(auto viewport{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(inID)})
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
        static_cast<float>(g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}
