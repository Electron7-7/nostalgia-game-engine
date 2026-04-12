#include "./imgui_editor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/theatre/theatre.hpp>
#include <Nostalgia/things/resources/font.hpp>
#include <Nostalgia/things/resources/array_mesh.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/resources/viewport_texture.hpp>
#include <Nostalgia/things/resources/cubemap.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/thinkers/2d/camera_2d.hpp>
#include <Nostalgia/things/thinkers/2d/sprite_2d.hpp>
#include <Nostalgia/things/thinkers/2d/text_2d.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/things/thinkers/3d/collider_3d.hpp>
#include <Nostalgia/things/thinkers/3d/light_3d.hpp>
#include <Nostalgia/things/thinkers/3d/mesh_instance_3d.hpp>
#include <Nostalgia/things/thinkers/3d/sprite_3d.hpp>
#include <Nostalgia/things/thinkers/3d/nostalgia_player_3d.hpp>

using namespace ImGui;

void ImGui_Editor::InspectResource()
{
    if(Begin("Resource Inspector"))
    {
        if(Button("Exit"))
        {
            mInspectingResourceUID = ID::Invalid;
            End();
            return;
        }

        switch(Theatre::Current()->TypeOf(mInspectingResourceUID)())
        {
        }

        End();
    }
}

void ImGui_Editor::InspectThinker()
{
    if(Begin("Thinker Inspector"))
    {
        if(Button("Exit"))
        {
            mInspectingThinkerUID = ID::Invalid;
            End();
            return;
        }

        switch(Theatre::Current()->TypeOf(mInspectingThinkerUID)())
        {
        }

        End();
    }
}
