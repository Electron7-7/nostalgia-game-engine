#include "imgui_editor.hpp"
#include "things/player.hpp"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/settings/world.hpp>
#include <Nostalgia/ui/implementor.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>
#include <Nostalgia/theatre/things/thinkers/viewport.hpp>
#include <Nostalgia/theatre/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/thirdparty/DearImGui/imgui.h>
#include <Nostalgia/thirdparty/DearImGui/imgui_stdlib.h>

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
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Editor::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Editor::TheatreEntered()
{
    if(!Settings::Engine::IsEditorHint)
    {
        auto player{g_pTheatreManager->CurrentTheatre()->GetThinker<EditorPlayer3D>(UID::a_Player)};
        UI_Implementor::SetGlobalCanHandleEvents(false);
        MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_DISABLED);
        player->mCaptureKeyboard = true;
        player->mCaptureMouse    = true;
        return;
    }
    TheatreFile::ThingData mat_dat{ThingType::Material, "ThingAdderSpawnLocation_Material"};
    mat_dat.set_variable(true, "NoTexture");
    mat_dat.set_variable(glm::vec3{1.0f, 0.0f, 0.0f}, "Color");
    mat_dat.set_variable(true, "mat_fullbright");
    sSpawnLocationMaterialID = g_pTheatreManager->CurrentTheatre()->CreateThing(mat_dat);

    TheatreFile::ThingData mesh_inst_dat{ThingType::MeshInstance3D,"ThingAdderSpawnLocation_MeshInstance"};
    mesh_inst_dat.set_variable(UID::m_Ramiel, "Mesh");
    mesh_inst_dat.set_variable(sSpawnLocationMaterialID, "MaterialOverride");
    mesh_inst_dat.set_variable(true, "Wireframe");
    sSpawnLocationMeshInstanceID = g_pTheatreManager->CurrentTheatre()->CreateThing(mesh_inst_dat);

    TheatreFile::ThingData spawn_dat{ThingType::Actor3D, "ThingAdderSpawnLocation"};
    spawn_dat.set_variable(sSpawnLocationMeshInstanceID, "Child");
    spawn_dat.set_variable(glm::vec3{1.0f}, "Scale");

    sSpawnLocationID = g_pTheatreManager->CurrentTheatre()->CreateThing(spawn_dat);

    g_pTheatreManager->CurrentTheatre()->CreateThing({ThingType::Viewport,
        "Editor Viewport",
        {},
        UID::a_EditorViewport});

    TheatreFile::ThingData cam_dat{ThingType::Camera3D,"Editor Camera"};
    cam_dat.set_variable(glm::vec3{0.0f, 5.0f, 1.0f}, "Position");
    cam_dat.set_variable(glm::vec3{-90.0f, 0.0f, 0.0f}, "RotationDegrees");
    cam_dat.set_variable(true, "UseDefaultSkybox");
    cam_dat.set_variable(UID::a_EditorViewport, "Parent");
    cam_dat.set_variable(true, "Current");
    g_pTheatreManager->CurrentTheatre()->CreateThing(cam_dat);
}

void ImGui_Editor::TheatreExited()
{
    sSpawnLocationMaterialID = ID{};
    sSpawnLocationMeshInstanceID = ID{};
    sSpawnLocationID = ID{};
    auto player{g_pTheatreManager->CurrentTheatre()->GetThinker<EditorPlayer3D>(UID::a_Player)};
    UI_Implementor::SetGlobalCanHandleEvents(true);
    MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
    player->mCaptureMouse    = false;
    player->mCaptureKeyboard = false;
}

void ImGui_Editor::Input(InputEvent* event)
{
    if(MainWindow()->GetMouseMode() == IWindow::MOUSE_MODE_DISABLED)
    {
        if(event->IsJustPressed(Key::Escape))
        {
            auto player{g_pTheatreManager->CurrentTheatre()->GetThinker<EditorPlayer3D>(UID::a_Player)};
            UI_Implementor::SetGlobalCanHandleEvents(true);
            MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
            player->mCaptureMouse    = false;
            player->mCaptureKeyboard = false;
        }
        return;
    }
    else if(event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift)
        and Manager::GetTheatreState() == ManagerEnums::IN_LEVEL)
    {
        auto player{g_pTheatreManager->CurrentTheatre()->GetThinker<EditorPlayer3D>(UID::a_Player)};
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
    if(!Settings::Engine::IsEditorHint)
        { return; }
    static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders |
        ImGuiChildFlags_ResizeY |
        ImGuiChildFlags_ResizeX};
    if(!sSpawnLocationID.invalid())
    {
        auto thingy{g_pTheatreManager->CurrentTheatre()->GetThinker<Actor3D>(sSpawnLocationMeshInstanceID)};
        if(thingy->Scale().y > sSpawnLocationScaleMax)
            { sScaleDirection = SCALE_DIRECTION_DOWN; }
        else if(thingy->Scale().y < sSpawnLocationScaleMin)
            { sScaleDirection = SCALE_DIRECTION_UP; }
        sSpawnLocationScaleSpeed = (int)sScaleDirection * sSpawnLocationScaleSpeedStore;
        thingy->SetRotationDegrees(thingy->RotationDegrees() + glm::vec3{0.0f, sSpawnLocationRotationSpeed, 0.0f});
        thingy->SetScale(thingy->Scale() + glm::vec3{0.0f, sSpawnLocationScaleSpeed, 0.0f});
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
        auto viewport{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(UID::a_EditorViewport)};
        if(viewport->uid().invalid())
            { End(); return; }
        BeginChild("Viewport",
            {500, 500},
            sResizableChildWithBorder,
            ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse);
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
            Image((ImTextureID)viewport->Framebuffer()->TextureID(),
                {(float)viewport->Size().w(), (float)viewport->Size().h()},
                {0, 1},
                {1, 0});
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
        EndChild();
        static bool camera_moving{false};
        auto player{g_pTheatreManager->CurrentTheatre()->GetThinker<EditorPlayer3D>(UID::a_Player)};
        if((IsItemHovered() or camera_moving)
            and !player->mCaptureMouse)
        {
            if(InputManager::IsKeyDown(Key::MouseLeft) and !camera_moving)
                { UI_Implementor::SetGlobalCanHandleEvents(false); camera_moving = true; MainWindow()->SetMouseMode(IWindow::MouseMode::MOUSE_MODE_DISABLED); }
            if(InputManager::IsKeyDown(Key::MouseLeft))
            {
                auto camera{g_pTheatreManager->CurrentTheatre()->GetThinker<Camera3D>(viewport->CurrentCamera3D())};
                auto motion{InputManager::MouseMotion()};
                camera->SetRotationDegrees(camera->RotationDegrees() - (glm::vec3{motion.y(), motion.x(), 0.0f} * 0.1f));
                glm::vec2 movement_direction{InputManager::IsActionDown("+right") - InputManager::IsActionDown("+left"),
                    InputManager::IsActionDown("+forward") - InputManager::IsActionDown("+backward")};

                camera->SetPosition(camera->Position() +
                    ((
                        ((camera->Quaternion() * Settings::World::Front()) *
                            movement_direction[1]) +
                        ((camera->Quaternion() * Settings::World::Right()) *
                            movement_direction[0])
                    ) * 0.1f)
                );
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
    if(!Settings::Engine::IsEditorHint)
        { return; }
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
        ThingType::PointLight3D.c_name(),
        ThingType::SpotLight3D.c_name(),
        ThingType::DirectionalLight3D.c_name(),
        ThingType::Collider3D.c_name(),
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
        { g_pTheatreManager->CurrentTheatre()->GetThinker<Actor3D>(sSpawnLocationID)->SetPosition(sSpawnLocation); }
    SameLine();
    if(Button("Reset Spawn Location"))
        { sSpawnLocation = glm::vec3{0.0f}; g_pTheatreManager->CurrentTheatre()->GetThinker<Actor3D>(sSpawnLocationID)->SetPosition(sSpawnLocation); }
    if(Button("Spawn Thing"))
    {
        sNewThingName = sNameBuffer;
        if(sNewThingName.empty()) { sNewThingName = "UntitledSpawnedThing"; }
        TheatreFile::ThingData thing_dat{sTypeNames[sSelectedType], (sNewThingName.empty()) ? "New Thing" : sNewThingName};
        thing_dat.set_variable(sSpawnLocation, "Position");
        g_pTheatreManager->CurrentTheatre()->CreateThing(thing_dat);
        sNewThingName.clear();
        sNameBuffer.clear();
    }

    EndChild();
}
