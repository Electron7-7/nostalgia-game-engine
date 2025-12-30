#include "camera_3d.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "settings/engine.hpp"

//////////////
// Camera3D //
//////////////
void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor::SetVariables(data);

    data.GetVariable(mCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor::GetVariables()};

    data->AddVariable(mCurrent, "Current");

    return data;
}

void Camera3D::Tick()
{
    if(mCurrent)
        { g_pTheatreManager->SetCurrentCamera(mUID); }
}

void Camera3D::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(mMeshInstanceID.invalid() and Settings::Engine::IsEditorHint)
    {
        ID material_id{g_pTheatreManager->CreateThing({
            mName + "_DebugMat",
            ThingType::Material,
            UID::Generate(),
            {
                {true, "FullBright"},
                {true, "NoTextures"},
                {glm::vec3{200.0f, 80.0f, 255.0f}, "Color"},
            }
        })};
        mMeshInstanceID = g_pTheatreManager->CreateThing({
            mName + "_DebugMeshInst",
            ThingType::MeshInstance,
            UID::Generate(),
            {
                {UID::m_Camera3D, "Mesh"},
                {material_id, "Material", mName + "_DebugMat"}
            }
        });
    }

    if(mCurrent)
        { g_pTheatreManager->SetCurrentCamera(mUID); }
}

void Camera3D::SetRenderLayers(Farg<RenderLayers> inLayers)
{ mRenderLayers = inLayers; }

Farg<RenderLayers> Camera3D::GetRenderLayers() const
{ return mRenderLayers; }

void Camera3D::SetCurrent(bool isCurrent)
{ mCurrent = true; }

bool Camera3D::IsCurrent() const
{ return mCurrent; }

glm::mat4 Camera3D::ViewMatrix() const
{ return glm::lookAt(mOrigin, mOrigin + Front(), Up());; }
