#include "nostalgia_player_3d.hpp"
#include "settings/engine.hpp"
#include "theatre/theatre.hpp"
#include "theatre/thing_factory.hpp"

using namespace TheatreFile;

void NostalgiaPlayer3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);
}

Shared<ThingData> NostalgiaPlayer3D::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};
    return data;
}

void NostalgiaPlayer3D::Ready()
{
    Actor3D::Ready();

    bool _debug_axis_mesh_made{false},
        _has_camera{false};
    for(ID child : Children())
    {
        if(ThingFactory::IsDerivedFrom(my_theatre()->TypeOf(child), ThingType::Camera3D))
            { _has_camera = true; }
    }
    if(!_has_camera)
    {
        TheatreFile::ThingData cam_dat{ThingType::Camera3D, "DefaultPlayerCam"};
        cam_dat.set_variable(glm::vec3{0.0f, 0.75f, 0.0f}, "Position");
        cam_dat.set_variable(true, "Current");
        cam_dat.set_variable(true, "UseDefaultSkybox");
        if(Settings::Engine::IsEditorHint)
        {
            cam_dat.set_variable(BitMask::all_enabled & ~0b10, "LayersMask");
            if(!_debug_axis_mesh_made)
            {
                TheatreFile::ThingData axis_dat{ThingType::MeshInstance3D, "PlayerDebugAxisMesh"};
                axis_dat.set_variable(mUID, "Parent");
                axis_dat.set_variable(UID::m_DebugAxis, "Mesh");
                axis_dat.set_variable(0b10, "Layers");
                my_theatre()->SetParent(my_theatre()->CreateThing(axis_dat), mUID);
                _debug_axis_mesh_made = true;
            }
        }
        my_theatre()->SetParent(mCameraID = my_theatre()->CreateThing(cam_dat), mUID);
    }
}
