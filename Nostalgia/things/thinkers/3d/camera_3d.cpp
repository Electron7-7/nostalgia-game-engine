#include "./camera_3d.hpp"
#include "../viewport.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"

using namespace TheatreFile;

void Camera3D::Ready()
{
    Super::Ready();
    // the debug mesh/material shouldn't override a manually specificed one
    if(Settings::Engine::IsEditorHint)
    {
        std::string cam_mat_name{"camera-debug-material"};
        ID cam_mat_id{Theatre::Current()->GetUID(cam_mat_name)};
        if(cam_mat_id.invalid())
        {
            ThingData mat_data{ThingType::Material, cam_mat_name};
            mat_data.set_variable(true, "FullBright");
            mat_data.set_variable(true, "NoTextures");
            mat_data.set_variable(glm::vec3{200.0f, 80.0f, 255.0f}, "Color");
            cam_mat_id = Theatre::Current()->CreateThing(mat_data);
        }

        std::string mesh_inst_name{name() + "-debug-mesh-instance"};
        if(!Theatre::Current()->ThingExists(mesh_inst_name))
        {
            ThingData mesh_inst_data{ThingType::MeshInstance3D, mesh_inst_name};
            mesh_inst_data.set_variable(UID::m_Camera3D, "Mesh");
            mesh_inst_data.set_variable(cam_mat_id, "MaterialOverride");
            mEditorMeshInstanceID = Theatre::Current()->CreateThing(mesh_inst_data);
        }
        else
            { mEditorMeshInstanceID = Theatre::Current()->GetThing(mesh_inst_name)->uid(); }
        Theatre::Current()->SetParent(mEditorMeshInstanceID, uid());
    }

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

    BitMask::StatusArray _render_layers{mLayersMask.status()};

    data.get_variable(mLayersMask, "RenderLayersMask", "LayersMask", "RenderLayers");
    for(uint i{0}; i < BitMask::max; ++i)
        { data.get_variable(_render_layers[i], std::format("RenderLayer{}", i+1), std::format("Layer{}", i+1)); }
    mLayersMask.set(_render_layers);
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

ID Camera3D::EditorMeshInstanceID() const
{ return mEditorMeshInstanceID; }

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
