#include "./nostalgia_player_3d.hpp"
#include "../viewport.hpp" // IWYU pragma: keep
#include "things/thing_data.hpp"
#include "settings/engine.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"

using namespace TheatreFile;

void NostalgiaPlayer3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    data.get_variable(mCreateDefaultCamera, "NoDefaultCamera", "NoCamera");
}

Shared<ThingData> NostalgiaPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    data->set_variable(mCreateDefaultCamera, "NoDefaultCamera");
    return data;
}

void NostalgiaPlayer3D::Ready()
{
    Super::Ready();

    for(ID child : Children())
    {
        if(ThingFactory::IsDerivedFrom(Theatre::Current()->TypeOf(child), ThingType::Camera3D))
            { mCreateDefaultCamera = false; }
    }

    if(mCreateDefaultCamera)
    {
        std::string cam_name{"DefaultPlayerCam"};
        if(!Theatre::Current()->ThingExists(cam_name))
        {
            TheatreFile::ThingData cam_dat{ThingType::Camera3D, cam_name};
            cam_dat.set_variable(glm::vec3{0.0f, 0.75f, 0.0f}, "Position");
            cam_dat.set_variable(true, "Current");
            cam_dat.set_variable(true, "UseDefaultSkybox");
            if(Settings::Engine::IsEditorHint)
            {
                cam_dat.set_variable(BitMask::all_enabled & ~0b10, "LayersMask");
                std::string axis_name{"PlayerDebugAxisMesh"};
                if(!Theatre::Current()->ThingExists(axis_name))
                {
                    TheatreFile::ThingData axis_dat{ThingType::MeshInstance3D, axis_name};
                    axis_dat.set_variable(uid(), "Parent");
                    axis_dat.set_variable(UID::m_DebugAxis, "Mesh");
                    axis_dat.set_variable(0b10, "Layers");
                    Theatre::Current()->SetParent(Theatre::Current()->CreateThing(axis_dat), uid());
                }
            }
            mCameraID = Theatre::Current()->CreateThing(cam_dat);
        }
        else
            { mCameraID = Theatre::Current()->GetThing(cam_name)->uid(); }
        Theatre::Current()->SetParent(mCameraID, uid());
    }

    if(not mCameraID.invalid())
        { Theatre::Current()->SetCurrentCamera(mCameraID); }
}
