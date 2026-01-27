#include "camera_3d.hpp"
#include "../viewport.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"

using namespace TheatreFile;

void Camera3D::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(Settings::Engine::IsEditorHint)
    {
        PID cam_mat{"camera-mat"};
        if(!my_theatre()->ThingExists(cam_mat))
        {
            ThingData mat_data{ThingType::Material,
                "camera-debug-material",
                {},
                cam_mat};
            mat_data.set_variable(true, "FullBright");
            mat_data.set_variable(true, "NoTextures");
            mat_data.set_variable(glm::vec3{200.0f, 80.0f, 255.0f}, "Color");
            my_theatre()->CreateThing(mat_data);
        }

        ThingData mesh_inst_data{ThingType::MeshInstance3D,{mName + "-debug-mesh-instance"}};
        mesh_inst_data.set_variable(UID::m_Camera3D, "Mesh");
        mesh_inst_data.set_variable(cam_mat, "MaterialOverride");
        mesh_inst_data.set_variable(0b1, "RenderLayers");
        mLayersMask.disable(0b1);
        my_theatre()->SetParent(my_theatre()->CreateThing(mesh_inst_data), mUID);
    }

    auto ancestors{my_theatre()->GetAllParents(mUID)};
    for(ID parent : ancestors)
    {
        if(my_theatre()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }

    if(mInitCurrent)
        { my_theatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera3D(mUID); }
}

void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    if(int bitmask; data.get_variable(bitmask, "RenderLayersMask", "LayersMask") == OK)
        { mLayersMask.set(bitmask); }
    data.get_variable(mFOV, "FOV");
    data.get_variable(mViewCutoffNear, "Near", "CutoffNear");
    data.get_variable(mViewCutoffFar, "Far", "CutoffFar");
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
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->set_variable(mLayersMask.get(), "RenderLayersMask");
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

ID Camera3D::ViewportID() const
{ return mViewportID; }

bool Camera3D::Current() const
{ return my_theatre()->GetThinker<Viewport>(mViewportID)->CurrentCamera3D() == mUID; }

Error Camera3D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current()) { return OK; }
    return my_theatre()
        ->GetThinker<Viewport>(mViewportID)
            ->SetCurrentCamera3D((isCurrent) ? mUID : ID::Invalid);
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
        static_cast<float>(my_theatre()->GetThinker<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}

void Camera3D::OnAncestorRemoved(Relative inAncestor)
{
    Actor3D::OnAncestorRemoved(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = UID::a_RootViewport; }
}

void Camera3D::OnAncestorAdded(Relative inAncestor)
{
    Actor3D::OnAncestorAdded(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
