#include "imgui_editor.hpp"
#include "core/uid.hpp"
#include "application/application.hpp"
#include "events/event.hpp"
#include "managers/render_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "theatre/parser/thing_data.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"

using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

bool gShowDebugWindow{true};

static bool sShowDemoWindow{false};
static void s_ThingAdder();
static ID sSpawnLocationMaterialID{};
static ID sSpawnLocationMeshInstanceID{};
static ID sSpawnLocationID{};
static ID sEditorFramebufferID{};
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
    sEditorFramebufferID = g_pRenderManager->GetAPI()->AddFrameBuffer(FrameBuffer::Create({1920, 1080}));
}

void ImGui_Editor::Shutdown()
{ PRINT_PRETTY_FUNCTION; mTextureBuffer.reset(); }

void ImGui_Editor::TheatreEntered()
{
    sSpawnLocationMaterialID = g_pTheatreManager->CreateThing(ThingData{
        "ThingAdderSpawnLocation_Material",
        ThingType::Material,
        UID::Generate(),
        {
            ThingVar{true, "NoTexture"},
            ThingVar{glm::vec3(1.0f, 0.0f, 0.0f), "Color"},
            ThingVar{true, "mat_fullbright"}
        }
    });
    sSpawnLocationMeshInstanceID = g_pTheatreManager->CreateThing({
        "ThingAdderSpawnLocation_MeshInstance",
        ThingType::MeshInstance,
        UID::Generate(),
        {{ID{UID::m_Ramiel}, "Mesh"}, {sSpawnLocationMaterialID, "Material"}}
    });
    sSpawnLocationID = g_pTheatreManager->CreateThing({
        "ThingAdderSpawnLocation",
        ThingType::Actor,
        UID::Generate(),
        {{sSpawnLocationMeshInstanceID, "MeshInstance"}, {glm::vec3(1.0f), "Scale"}, {true, "Wireframe"}}
    });
}

void ImGui_Editor::TheatreExited()
{
    sSpawnLocationMaterialID = ID{};
    sSpawnLocationMeshInstanceID = ID{};
    sSpawnLocationID = ID{};
}

void ImGui_Editor::Input(InputEvent* event)
{
    if(g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard)
    {
        if(event->IsJustPressed(Key::Escape))
        {
            MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE);
            g_pTheatreManager->GetLocalPlayer()->mCaptureMouse    = false;
            g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard = false;
        }
        return;
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
    if(!sSpawnLocationID.invalid())
    {
        auto thingy = g_pTheatreManager->GetThing<Actor>(sSpawnLocationID);
        if(thingy->Scale().y > sSpawnLocationScaleMax)
            { sScaleDirection = SCALE_DIRECTION_DOWN; }
        else if(thingy->Scale().y < sSpawnLocationScaleMin)
            { sScaleDirection = SCALE_DIRECTION_UP; }
        sSpawnLocationScaleSpeed = (int)sScaleDirection * sSpawnLocationScaleSpeedStore;
        thingy->SetEuler(thingy->Euler(true) + glm::vec3{0.0f, sSpawnLocationRotationSpeed, 0.0f}, true);
        thingy->SetScale(thingy->Scale() + glm::vec3{0.0f, sSpawnLocationScaleSpeed, 0.0f});
    }
    if(sShowDemoWindow)
        { ShowDemoWindow(&sShowDemoWindow); }
    float window_width  = MainWindow()->GetWidth();
    float window_height = MainWindow()->GetHeight();
    SetNextWindowSize({window_width, window_height});
    SetNextWindowPos({0, 0});
    if(Begin("Nostalgia Editor",
        nullptr,
        ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus))
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
        static ImGuiChildFlags sResizableChildWithBorder{ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY | ImGuiChildFlags_ResizeX};
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
        if(BeginChild("Viewport",
            {720, 690},
            sResizableChildWithBorder,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_EnableSRGB, nullptr);
            Image((ImTextureID)g_pRenderManager->GetAPI()
                ->GetFrameBuffer(sEditorFramebufferID)
                    ->TextureID(),
                static_cast<ImVec2>(MainWindow()->GetScale()), {0, 1}, {1, 0}); /*g_pRenderManager->GetAPI()->GetFrameBuffer(sEditorFramebufferID)->TextureSize() * 0.5f*/
            GetWindowDrawList()->AddCallback(ImDrawCallback_ImplGL_DisableSRGB, nullptr);
            if(IsItemClicked())
            {
                MainWindow()->SetMouseMode(
                    (MainWindow()->GetMouseMode() != IWindow::MOUSE_MODE_DISABLED)
                        ? IWindow::MOUSE_MODE_DISABLED
                        : IWindow::MOUSE_MODE_VISIBLE);
                g_pTheatreManager->GetLocalPlayer()->mCaptureMouse    = !g_pTheatreManager->GetLocalPlayer()->mCaptureMouse;
                g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard = !g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard;
            }
        }
        EndChild();
        if(BeginChild("AddThingMenu", {500, 690}, sResizableChildWithBorder))
            { s_ThingAdder(); }
        EndChild(); PopStyleVar();
    }
    End();
}

static std::string s_GetComboString(std::string* inArray, int inSize)
{
    std::string output{};
    for(int i{0}; i < inSize; ++i)
        { output += inArray[i] + '\0'; }
    return output;
}

void s_ThingAdder()
{
    static ThingData sThingData{};
    static std::string sNewThingName{""};
    static ID sNewThingID{ThingType::Thing};
    static std::string sNameBuffer{};
    static glm::vec3 sSpawnLocation{0.0f};
    static int sCurrentType{0};
    static std::string sTypeNames[]
    {
        ThingType::Thing.c_name(),
        ThingType::Actor.c_name(),
        ThingType::PointLight.c_name(),
        ThingType::SpotLight.c_name(),
        ThingType::DirectionalLight.c_name(),
        ThingType::Device.c_name(),
        ThingType::Collider.c_name(),
        ThingType::Material.c_name(),
        ThingType::MeshInstance.c_name()
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
    Combo("Type", &sCurrentType, s_GetComboString(sTypeNames, 9).data());
    if(DragGLMv3("Spawn Location", &sSpawnLocation, 0.01f, 0.0f, 0.0f, "%.2f"))
        { g_pTheatreManager->GetThing<Actor>(sSpawnLocationID)->SetOrigin(sSpawnLocation); }
    SameLine();
    if(Button("Reset Spawn Location"))
        { sSpawnLocation = glm::vec3(0.0f); }
    if(Button("Spawn Thing"))
    {
        sNewThingName = sNameBuffer;
        sThingData = {sNewThingName, sTypeNames[sCurrentType], UID::Generate()};
        sThingData.AddVariable(sSpawnLocation, "Origin");
        if(sNewThingName.empty()) { sNewThingName = "UntitledSpawnedThing"; }
        g_pTheatreManager->CreateThing(sThingData);
        sNewThingID = ThingType::Thing;
        sNewThingName.clear();
        sNameBuffer.clear();
        sThingData.clear();
    }

    EndChild();
}
