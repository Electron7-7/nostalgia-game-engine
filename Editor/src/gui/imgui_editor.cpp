#include "imgui_editor.hpp"
#include "core/uid.hpp"
#include "application/application.hpp"
#include "events/event.hpp"
#include "managers/input_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "ui/implementor.hpp"
#include "rendering/renderer_api.hpp"
#include "things/devices/viewport.hpp"
#include "theatre/parser/thing_data.hpp"
#include "things/actors/camera.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"

using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

bool gShowDebugWindow{false};

static bool sShowDemoWindow{false};
static void s_ThingAdder();
static ID sSpawnLocationMaterialID{};
static ID sSpawnLocationMeshInstanceID{};
static ID sSpawnLocationID{};
static float sSpawnLocationRotationSpeed{1.0f};
static float sSpawnLocationScaleSpeedStore{0.0085f};
static float sSpawnLocationScaleSpeed{sSpawnLocationScaleSpeedStore};
static float sSpawnLocationScaleMax{1.0f};
static float sSpawnLocationScaleMin{0.6f};

void ImDrawCallback_ImplGL_EnableSRGB(const ImDrawList*, const ImDrawCmd*)
{ g_pRenderManager->GetAPI()->SetFramebufferSRGB(true); }

void ImDrawCallback_ImplGL_DisableSRGB(const ImDrawList*, const ImDrawCmd*)
{ g_pRenderManager->GetAPI()->SetFramebufferSRGB(false); }

void ImGui_Editor::Init()
{
    PRINT_PRETTY_FUNCTION;
}

void ImGui_Editor::Shutdown()
{ PRINT_PRETTY_FUNCTION; mTextureBuffer.reset(); }

void ImGui_Editor::TheatreEntered()
{
    sSpawnLocationMaterialID = g_pTheatreManager->CreateThing({
        "ThingAdderSpawnLocation_Material",
        ThingType::Material,
        UID::Generate(),
        {
            {true, "NoTexture"},
            {glm::vec3{1.0f, 0.0f, 0.0f}, "Color"},
            {true, "mat_fullbright"}
        }
    });
    sSpawnLocationMeshInstanceID = g_pTheatreManager->CreateThing({
        "ThingAdderSpawnLocation_MeshInstance",
        ThingType::MeshInstance3D,
        UID::Generate(),
        {{UID::m_Ramiel, "Mesh"}, {sSpawnLocationMaterialID, "Material"}}
    });
    sSpawnLocationID = g_pTheatreManager->CreateThing({
        "ThingAdderSpawnLocation",
        ThingType::Actor3D,
        UID::Generate(),
        {{sSpawnLocationMeshInstanceID, "DebugMeshInstance"}, {glm::vec3{1.0f}, "Scale"}, {true, "Wireframe"}}
    });

    g_pTheatreManager->CreateThing({"Editor Viewport",
        ThingType::Viewport,
        UID::a_EditorViewport});

    g_pTheatreManager->CreateThing({"Editor Camera",
        ThingType::Camera3D,
        UID::Generate(),
        {
            {true, "UseDefaultSkybox"},
            {UID::a_EditorViewport, "Viewport"},
            {true, "Current"},
        }});
}

void ImGui_Editor::TheatreExited()
{
    sSpawnLocationMaterialID = ID{};
    sSpawnLocationMeshInstanceID = ID{};
    sSpawnLocationID = ID{};
}

void ImGui_Editor::Input(InputEvent* event)
{
    auto player{g_pTheatreManager->GetThing<NostalgiaPlayer>(UID::a_Player)};
    if(player->mCaptureKeyboard)
    {
        if(event->IsJustPressed(Key::Escape))
        {
            UI_Implementor::SetGlobalCanHandleEvents(true);
            MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
            player->mCaptureMouse    = false;
            player->mCaptureKeyboard = false;
        }
        return;
    }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift))
    {
        UI_Implementor::SetGlobalCanHandleEvents(false);
        MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_DISABLED);
        player->mCaptureKeyboard = true;
        player->mCaptureMouse    = true;
    }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control))
        { gShowDebugWindow = !gShowDebugWindow; }
    else if(event->IsJustPressed(Key::G) and event->IsModifierActive(Key::Mod_Control))
        { sShowDemoWindow  = !sShowDemoWindow; }
}

enum ScaleDir
{
    SCALE_DIRECTION_UP   = 1,
    SCALE_DIRECTION_DOWN = -1
};
static ScaleDir sScaleDirection{SCALE_DIRECTION_DOWN};

void ImGui_Editor::Update()
{
    static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders |
        ImGuiChildFlags_ResizeY |
        ImGuiChildFlags_ResizeX};
    if(!sSpawnLocationID.invalid())
    {
        auto thingy = g_pTheatreManager->GetThing<Actor3D>(sSpawnLocationID);
        if(thingy->Scale().y > sSpawnLocationScaleMax)
            { sScaleDirection = SCALE_DIRECTION_DOWN; }
        else if(thingy->Scale().y < sSpawnLocationScaleMin)
            { sScaleDirection = SCALE_DIRECTION_UP; }
        sSpawnLocationScaleSpeed = (int)sScaleDirection * sSpawnLocationScaleSpeedStore;
        thingy->Euler(thingy->Euler(true) + glm::vec3{0.0f, sSpawnLocationRotationSpeed, 0.0f}, true);
        thingy->Scale(thingy->Scale() + glm::vec3{0.0f, sSpawnLocationScaleSpeed, 0.0f});
    }
    if(sShowDemoWindow)
        { ShowDemoWindow(&sShowDemoWindow); }
    if(Begin("Nostalgia Editor"))
    {
        if(BeginMenuBar())
        {
            if(BeginMenu("Menu"))
            {
                if(MenuItem("Show Debug Menu", "CTRL+D"))
                    { gShowDebugWindow = true; }
                else if(MenuItem("Show ImGui Demo Window", "CTRL+G"))
                    { sShowDemoWindow = true; }
                else if(MenuItem("Quit", "CTRL+Q"))
                    { Application()->Stop(); }
                EndMenu();
            }
            EndMenuBar();
        }
        auto viewport{g_pTheatreManager->GetThing<Viewport>(UID::a_EditorViewport)};
        if(!viewport->Framebuffer())
            { End(); return; }
        BeginChild("Viewport",
            {500, 500},
            sResizableChildWithBorder,
            ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse);
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
            Image((ImTextureID)viewport->Framebuffer()->TextureID(),
                (ImVec2)viewport->Size(),
                {0, 1},
                {1, 0});
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
        EndChild();
        static bool camera_moving{false};
        if((IsItemHovered() or camera_moving)
            and !g_pTheatreManager->GetPlayer()->mCaptureMouse)
        {
            if(InputManager::IsKeyDown(Key::MouseLeft) and !camera_moving)
                { UI_Implementor::SetGlobalCanHandleEvents(false); camera_moving = true; MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED); }
            if(InputManager::IsKeyDown(Key::MouseLeft))
            {
                auto camera{g_pTheatreManager->GetThing<Camera3D>(viewport->CurrentCamera())};
                auto motion{InputManager::MouseMotion()};
                camera->Euler(camera->Euler(true) - (0.1f * glm::vec3{motion.y(), motion.x(), 0.0f}), true);
                glm::vec2 movement_direction{InputManager::IsActionDown("+right") - InputManager::IsActionDown("+left"),
                    InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward")};

                camera->Origin(camera->Origin() + (0.1f * ((camera->Front() * movement_direction[1]) + (camera->Right() * movement_direction[0]))));
            }
        }
        if(InputManager::IsKeyUp(Key::MouseLeft)
            and camera_moving
            and !player->mCaptureMouse)
            { UI_Implementor::SetGlobalCanHandleEvents(true); camera_moving = false; MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_VISIBLE); }
        BeginChild("AddThingMenu", {500, 690}, sResizableChildWithBorder);
            s_ThingAdder();
        EndChild();
    }
    End();
}

void s_ThingAdder()
{
    static std::string sNewThingName{};
    static std::string sNameBuffer{};
    static glm::vec3 sSpawnLocation{0.0f};
    static int sSelectedType{0};
    static std::vector<const char*> sTypeNames
    {
        ThingType::Thing.c_name(),
        ThingType::Actor3D.c_name(),
        ThingType::Actor2D.c_name(),
        ThingType::Camera3D.c_name(),
        ThingType::PointLight.c_name(),
        ThingType::SpotLight.c_name(),
        ThingType::DirectionalLight.c_name(),
        ThingType::Device.c_name(),
        ThingType::Collider.c_name(),
        ThingType::Material.c_name(),
        ThingType::MeshInstance3D.c_name(),
        ThingType::Resource.c_name(),
        ThingType::Texture.c_name(),
        ThingType::Mesh.c_name(),
    };
    BeginChild("MakeNewThing");
    if(CollapsingHeader("Thing Spawner Widget"))
    {
        DragFloat("Spawn Location Rotation Speed", &sSpawnLocationRotationSpeed, 0.001f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Speed", &sSpawnLocationScaleSpeedStore, 0.1f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Max", &sSpawnLocationScaleMax, 0.1f, 0.0f, 0.0f, "%.2f");
        DragFloat("Spawn Location Scale Min", &sSpawnLocationScaleMin, 0.1f, 0.0f, 0.0f, "%.2f");
    }
    SeparatorText("Thing Spawner");
    InputTextWithHint("Name", "UntitledThing", &sNameBuffer);
    if(BeginCombo("Type", sTypeNames[sSelectedType], ImGuiComboFlags_WidthFitPreview))
    {
        for(uint i{0}; i < sTypeNames.size(); ++i)
        {
            const bool is_selected{sSelectedType == i};
            if(Selectable(sTypeNames[i], is_selected))
                { sSelectedType = i; }
            if(is_selected)
                { SetItemDefaultFocus(); }
        }
        EndCombo();
    }
    if(DragGLMv3("Spawn Location", &sSpawnLocation, 0.01f, 0.0f, 0.0f, "%.2f"))
        { g_pTheatreManager->GetThing<Actor3D>(sSpawnLocationID)->Origin(sSpawnLocation); }
    SameLine();
    if(Button("Reset Spawn Location"))
        { sSpawnLocation = glm::vec3{0.0f}; g_pTheatreManager->GetThing<Actor3D>(sSpawnLocationID)->Origin(sSpawnLocation); }
    if(Button("Spawn Thing"))
    {
        sNewThingName = sNameBuffer;
        if(sNewThingName.empty()) { sNewThingName = "UntitledSpawnedThing"; }
        g_pTheatreManager->CreateThing({(sNewThingName.empty()) ? "New Thing" : sNewThingName,
            sTypeNames[sSelectedType],
            UID::Generate(),
            { {sSpawnLocation, "Origin"} }
        });
        sNewThingName.clear();
        sNameBuffer.clear();
    }

    EndChild();
}
