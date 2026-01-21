#include "camera_3d.hpp"
#include "core/uid.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/theatre.hpp"
#include "theatre/thing_factory.hpp"
#include "settings/engine.hpp"
#include "settings/world.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Camera3D::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(Settings::Engine::IsEditorHint)
    {
        PID cam_mat{"camera-mat"};
        if(!m_pRootTheatre->ThingExists(cam_mat))
        {
            ThingData mat_data{ThingType::Material,
                "camera-debug-material",
                {},
                cam_mat};
            mat_data.set_variable(true, "FullBright");
            mat_data.set_variable(true, "NoTextures");
            mat_data.set_variable(glm::vec3{200.0f, 80.0f, 255.0f}, "Color");
            m_pRootTheatre->CreateThing(mat_data);
        }

        ThingData mesh_inst_data{ThingType::MeshInstance3D,{mName + "-debug-mesh-instance"}};
        print_error_enum(mesh_inst_data.set_variable(UID::m_Camera3D, "Mesh"));
        print_error_enum(mesh_inst_data.set_variable(cam_mat, "MaterialOverride"));
        ID mesh_id{m_pRootTheatre->CreateThing(mesh_inst_data)};
        m_pRootTheatre->SetParent(mesh_id, mUID);
    }

    auto ancestors{m_pRootTheatre->GetAllParents(mUID)};
    for(ID parent : ancestors)
    {
        if(m_pRootTheatre->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }

    if(mInitCurrent)
        { m_pRootTheatre->GetThinker<Viewport>(mViewportID)->SetCurrentCamera(mUID); }
}

void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

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

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

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
{ return m_pRootTheatre->GetThinker<Viewport>(mViewportID)->CurrentCamera() == mUID; }

Error Camera3D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current()) { return OK; }
    return m_pRootTheatre
        ->GetThinker<Viewport>(mViewportID)
            ->SetCurrentCamera((isCurrent) ? mUID : ID::Invalid);
}

BitMask Camera3D::LayersMask() const
{ return mLayersMask; }

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
        static_cast<float>(m_pRootTheatre->GetThinker<Viewport>(mViewportID)->Size().AspectRatio()),
        mViewCutoffNear,
        mViewCutoffFar
    );
}

void Camera3D::OnAncestorRemoved(Relative inAncestor)
{
    Actor3D::OnAncestorRemoved(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = UID::a_Global3DViewport; }
}

void Camera3D::OnAncestorAdded(Relative inAncestor)
{
    Actor3D::OnAncestorAdded(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
