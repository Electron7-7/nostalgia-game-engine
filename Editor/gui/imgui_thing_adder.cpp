#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/theatre/theatre.hpp>

using namespace ImGui;

static std::string _name{};
static int sSelectedType{0};
extern std::vector<PID> Types;

void ImGui_Editor::do_ThingAdder()
{
    if(mAddThing)
    {
        mAddThing = false;
        OpenPopup("Add Thing");
    }
    if(BeginPopup("Add Thing"))
    {
        InputText("Name", &_name);
        if(BeginCombo("Thing Type",
            Types[sSelectedType].c_name(),
            ImGuiComboFlags_HeightRegular | ImGuiComboFlags_WidthFitPreview))
        {
            for(uint i{0}; i < Types.size(); ++i)
            {
                if(Types[i] == ThingType::Resource)
                    { SeparatorText("Resources"); }
                else if(Types[i] == ThingType::Thinker)
                    { SeparatorText("Thinkers"); }
                const bool is_selected{sSelectedType == i};
                if(Selectable(Types[i].c_name(), is_selected))
                    { sSelectedType = i; }
                if(is_selected)
                    { SetItemDefaultFocus(); }
            }
            EndCombo();
        }
        BeginDisabled(_name.empty() or Theatre::Current()->Contains(_name));
            if(Button("Spawn Thing"))
            {
                Theatre::Current()->CreateThing({Types[sSelectedType], _name});
                _name.clear();
            }
        EndDisabled();
        EndPopup();
    }
}

std::vector<PID> Types{
//  Resources
    ThingType::Resource,
        ThingType::Font,
        ThingType::Mesh,
            ThingType::ArrayMesh,
        ThingType::Texture,
            ThingType::Cubemap,
            ThingType::ViewportTexture,
            ThingType::ImageTexture,
        ThingType::Material,
        ThingType::Image,
//  Thinkers
    ThingType::Thinker,
        ThingType::NostalgiaPlayer,
        ThingType::Viewport,
        ThingType::Actor3D,
            ThingType::Camera3D,
            ThingType::Collider3D,
            ThingType::Visual3D,
                ThingType::MeshInstance3D,
                ThingType::Sprite3D,
                ThingType::Light3D,
                    ThingType::PointLight3D,
                    ThingType::SpotLight3D,
                    ThingType::DirectionalLight3D,
        ThingType::Actor2D,
            ThingType::Camera2D,
            ThingType::Visual2D,
                ThingType::Sprite2D,
                ThingType::Text2D,
};
