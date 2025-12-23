#include "imgui_editor.hpp"
#include "core/time.hpp"
#include "core/uid.hpp"
#include "application/application.hpp"
#include "events/event.hpp"
#include "managers/render_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "rendering/viewport.hpp"
#include "rendering/renderer_api.hpp"
#include "theatre/parser/thing_data.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"

#define DISABLED_IF(IS_TRUE, CODE) if(IS_TRUE) { BeginDisabled(); } CODE if(IS_TRUE) { EndDisabled(); }

using namespace ImGui;

static ImGui_Editor sImGuiEditor{};
ImGui_Editor* g_pImGuiEditor{&sImGuiEditor};

bool gShowDebugWindow{false};

static bool sShowDemoWindow{false};
static void s_ThingAdder();
static ID sSpawnLocationMaterialID{};
static ID sSpawnLocationMeshInstanceID{};
static ID sSpawnLocationID{};
static ID sEditorViewportID{Viewport::IDs::MainWindow};
// static ID sEditorFramebufferID{};
static float sSpawnLocationRotationSpeed{1.0f};
static float sSpawnLocationScaleSpeedStore{0.0085f};
static float sSpawnLocationScaleSpeed{sSpawnLocationScaleSpeedStore};
static float sSpawnLocationScaleMax{1.0f};
static float sSpawnLocationScaleMin{0.6f};

template<>
struct std::formatter<ImVec2> : public std::formatter<float[2]>
{
    auto format(const ImVec2& vec2, std::format_context& ctx) const
    { return std::formatter<float[2]>::format({vec2.x, vec2.y}, ctx); }
};

Error ImGui_Editor::Init()
{
    PRINT_PRETTY_FUNCTION;
    // sEditorViewportID = g_pRenderManager->GetAPI()->AddViewport({200, 200});
    // sEditorFramebufferID = g_pRenderManager->GetAPI()->GenerateFrameBuffer();
    mTextureBuffer = TextureBuffer::Create();
    mTextureBuffer->GenerateTexture(TextureFormat{TEXTURE_TYPE_2D, 500, 500});
    // g_pRenderManager->GetAPI()->GetFrameBuffer(sEditorFramebufferID)->SetOutputTexture(mTextureBuffer);
    return OK;
}

void ImGui_Editor::Shutdown()
{ mTextureBuffer.reset(); }

void ImGui_Editor::OnTheatreEntered()
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

void ImGui_Editor::OnTheatreExited()
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
            g_pTheatreManager->GetLocalPlayer()->mCaptureMouse    = false;
            g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard = false;
        }
        return;
    }
    else if(event->IsJustPressed(Key::Tab) or (event->IsJustPressed(Key::D) and event->IsModifierActive(Key::Mod_Control)))
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
    if(Begin("Nostalgia Editor", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
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
        BeginChild("Viewport", {720, 690}, sResizableChildWithBorder);
        BeginChild("ViewportObject", {200, 200}, sResizableChildWithBorder, ImGuiWindowFlags_NoBackground); EndChild();
        PopStyleVar();
        if(IsItemClicked())
        {
            MainWindow()->SetMouseMode(
                (MainWindow()->GetMouseMode() == IWindow::MOUSE_MODE_CAPTURED)
                    ? IWindow::MOUSE_MODE_DISABLED
                    : IWindow::MOUSE_MODE_VISIBLE);
            g_pTheatreManager->GetLocalPlayer()->mCaptureMouse    = !g_pTheatreManager->GetLocalPlayer()->mCaptureMouse;
            g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard = !g_pTheatreManager->GetLocalPlayer()->mCaptureKeyboard;
        }
        auto& editor_viewport = g_pRenderManager->GetAPI()->GetViewport(sEditorViewportID);
        editor_viewport.scale = (GetItemRectMax() - GetItemRectMin() - ImVec2{0,16});
        editor_viewport.position.x() = GetItemRectMin().x;
        editor_viewport.position.y() = window_height - GetItemRectMax().y;
        TextF("Editor Viewport Size: {}", editor_viewport.scale.data_log());
        TextF("Editor Viewport Pos: {}", editor_viewport.position.data_log());
        EndChild(); SameLine();
        BeginChild("AddThingMenu", {500, 690}, sResizableChildWithBorder);
            s_ThingAdder();
        EndChild();
    }
    End();
    SetNextWindowSize({122, 52});
    SetNextWindowPos({0, 0});
    if(Begin("Time::Current",
        nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        { TextF("{}", Time::Current()); }
    End();
}

void s_ThingAdder()
{
    static ThingData sThingData{};
    static std::string sNewThingName{""};
    static ID sNewThingID{ThingType::Thing};
    static std::string sNameBuffer{};
    static glm::vec3 sSpawnLocation{0.0f};
    static int sCurrentType{0};
    static const char* sTypeNames[]
    {
        ThingType::Thing.c_name(),
        ThingType::Actor.c_name(),
        ThingType::PointLight.c_name(),
        ThingType::SpotLight.c_name(),
        ThingType::DirectionalLight.c_name(),
        ThingType::Device.c_name(),
        ThingType::Collider.c_name(),
        ThingType::Material.c_name(),
        ThingType::MeshInstance.c_name(),
    };
    BeginChild("MakeNewThing");
    SeparatorText("Thing Spawner");
    InputTextWithHint("Name", "UntitledThing", &sNameBuffer);
    ListBox("Type", &sCurrentType, sTypeNames, 9);
    if(DragGLMv3("Spawn Location", &sSpawnLocation, 0.01f, 0.0f, 0.0f, "%.2f"))
        { g_pTheatreManager->GetThing<Actor>(sSpawnLocationID)->SetOrigin(sSpawnLocation); }
    SameLine();
    if(Button("Reset Spawn Location"))
        { sSpawnLocation = glm::vec3(0.0f); }
    DragFloat("Spawn Location Rotation Speed", &sSpawnLocationRotationSpeed, 0.001f, 0.0f, 0.0f, "%.2f");
    DragFloat("Spawn Location Scale Speed", &sSpawnLocationScaleSpeedStore, 0.1f, 0.0f, 0.0f, "%.2f");
    DragFloat("Spawn Location Scale Max", &sSpawnLocationScaleMax, 0.1f, 0.0f, 0.0f, "%.2f");
    DragFloat("Spawn Location Scale Min", &sSpawnLocationScaleMin, 0.1f, 0.0f, 0.0f, "%.2f");
    if(Button("Spawn Thing"))
    {
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
