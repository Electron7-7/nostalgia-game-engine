#include "imgui_debugger.hpp"
#include "imgui_editor.hpp"
#include "tools/stopwatch_log.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/console/console.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/event_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/managers/theatre_manager.hpp>
#include <Nostalgia/theatre/theatre_file.hpp>
#include <Nostalgia/physics/engine.hpp>
#include <Nostalgia/settings/engine.hpp>
#include <Nostalgia/settings/graphics.hpp>
#include <Nostalgia/settings/player.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/rendering/debugging.hpp>
#include <Nostalgia/rendering/vertex_array.hpp>
#include <Nostalgia/rendering/mesh_buffers.hpp>
#include <Nostalgia/things/thinkers/2d/text_2d.hpp>
#include <Nostalgia/things/thinkers/2d/sprite_2d.hpp>
#include <Nostalgia/things/thinkers/2d/camera_2d.hpp>
#include <Nostalgia/things/thinkers/3d/light_3d.hpp>
#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/resources/array_mesh.hpp>
#include <Nostalgia/things/resources/material.hpp>
#include <Nostalgia/things/resources/font.hpp>
#include <Nostalgia/things/resources/image_texture.hpp>
#include <Nostalgia/things/thinkers/3d/mesh_instance_3d.hpp>
#include <Nostalgia/things/thinkers/3d/collider_3d.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/application/window.hpp>

#define QUICK_COMBO_THING_VAR(NAME, MAP, SELECTED_VAR, THING_SETTER) \
if(BeginCombo(#NAME, MAP.at(selected.SELECTED_VAR))) \
{ \
    for(FAUTO [Enum, Name] : MAP) \
    { \
        const bool is_selected{Enum == selected.SELECTED_VAR}; \
        if(Selectable(Name)) \
        { \
            selected.SELECTED_VAR = Enum; \
            THING_SETTER; \
        } \
        if(is_selected) \
            { SetItemDefaultFocus(); } \
    } \
    EndCombo(); \
}

using namespace ImGui;

static ImGui_Debugger sImGuiDebugger;
ImGui_Debugger* g_pImGuiDebugger{&sImGuiDebugger};

static bool sAutoStopwatchEnabled{true},
    sEnableFrameCounter{true},
    sEnableFps{true},
    sEnableFrametime{true},
    sEnableTickRate{false},
    sEnableTickInterval{false},
    sEnableRenderManagerFrametime{true};

static std::string               sTheatreFilePath{"theatres/HelloWorld.nt"};
static std::string               sTheatreFileSavePath{"theatres/SavedTheatre.nt"};
static std::string               sLastAttemptedTheatreFilePath{sTheatreFilePath};
static std::string               sDemoFileOut{"demo"};
static std::string               sDemoFileIn{std::format("{}.dem", sDemoFileOut)};
static std::vector<TheatreLog>   sTheatreLogs{};
static std::vector<StopwatchLog> sManualStopwatchLogs{};
static std::vector<StopwatchLog> sStopwatchLogs{};
static std::set<int>             sStopwatchLogIds{};
static Theatre::FileOverwriteAction sCurrentFileOverwriteAction{Theatre::RENAME};

static void s_GeneralDebuggingWindow();
static void s_FPSCounter();

void ImGui_Debugger::Init()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Debugger::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Debugger::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::Tilde))
        { gDebugConsoleOpened = !gDebugConsoleOpened; }
    else if(event->IsJustPressed(Key::F3))
    {
        gTheatreInspectorActive = (IManager::GetTheatreState() == ManagerEnums::IN_LEVEL)
            ? !gTheatreInspectorActive
            : false;
    }
    else if(event->IsJustPressed(Key::F4) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift)))
            { Manager::ShutdownTheatre(); MainWindow()->SetMouseMode(IWindow::MOUSE_MODE_VISIBLE); }
    else if(event->IsJustPressed(Key::F5) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control)))
    {
        gTheatreInspectorActive = false;
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadFromFile(sTheatreFilePath);
    }
    else if(event->IsJustPressed(Key::F6) or
        (event->IsJustPressed(Key::S) and event->IsModifierActive(Key::Mod_Control)))
            { g_pTheatreManager->Current()->Save(sTheatreFileSavePath, sCurrentFileOverwriteAction); }
}

void ImGui_Debugger::TheatreEntered()
{}

void ImGui_Debugger::TheatreExited()
{}

void ImGui_Debugger::Update()
{
    s_FPSCounter();
    static bool sPopOutStopwatches{false};
    SetNextWindowSize({840,530}, ImGuiCond_FirstUseEver);
    if(gDebugConsoleOpened)
        { DebugConsoleWindow(); }
    if(gShowDebugWindow)
    {
        if(not Begin("Debugging", &gShowDebugWindow))
            { End(); }
        else if(BeginTabBar("Debug Tools"))
        {
            if(BeginTabItem("General"))
            {
                s_GeneralDebuggingWindow();
                EndTabItem();
            }
            if(sPopOutStopwatches)
            {
                SetNextWindowSize({780,458}, ImGuiCond_FirstUseEver);
                if(Begin("Stopwatches"))
                {
                    float width = (GetWindowWidth() / 2.0f) - 12.0f;
                    m_AutomaticStopwatchWindow(width);
                    SameLine();
                    m_ManualStopwatchWindow(width);
                }
                End();
            }
            if(BeginTabItem("Stopwatches"))
            {
                const char* pop_out_label = (sPopOutStopwatches) ? "Bring Back Window" : "Pop Out Window";
                if(Button(pop_out_label))
                    { sPopOutStopwatches = !sPopOutStopwatches; }
                if(!sPopOutStopwatches)
                {
                    float width = (GetWindowWidth() / 2.0f) - 12.0f;
                    m_AutomaticStopwatchWindow(width);
                    SameLine();
                    m_ManualStopwatchWindow(width);
                }
                EndTabItem();
            }
        }
        EndTabBar();
        End();
    }
}

void s_HandleAutomaticStopwatchToggle()
{
    sAutoStopwatchEnabled = !sAutoStopwatchEnabled;
    if(sAutoStopwatchEnabled)
        { return; }
    for(StopwatchLog& log : sStopwatchLogs)
    {
        if(log.Stop())
            { log.OverrideMessage("Interrupted by disabling automatic stopwatches"); }
    }
}

StopwatchLog& ImGui_Debugger::StartStopwatch(const std::string& message)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    if(!sAutoStopwatchEnabled)
        { return StopwatchLog::Invalid; }
    return m_StartStopwatch(message);
}

bool ImGui_Debugger::StopStopwatch(StopwatchLog& stopwatch)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    if(!sAutoStopwatchEnabled)
        { return false; }
    return m_StopStopwatch(stopwatch);
}

StopwatchLog& ImGui_Debugger::m_StartStopwatch(const std::string& message)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    StopwatchLog& stopwatch = sStopwatchLogs.emplace_back();
    stopwatch.Start(message);
    return stopwatch;
}

bool ImGui_Debugger::m_StopStopwatch(StopwatchLog& stopwatch)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    if(stopwatch == StopwatchLog::Invalid || !stopwatch.Running())
        { return false; }
    else if(std::find(sStopwatchLogs.begin(), sStopwatchLogs.end(), stopwatch) == sStopwatchLogs.end())
        { return false; }
    return stopwatch.Stop();
}

void ImGui_Debugger::StartTheatreTiming(bool loading)
{
    if(loading)
    {
        TheatreLog& log = sTheatreLogs.emplace_back();
        log.load_time.Start();
        log.name_or_file_path = sLastAttemptedTheatreFilePath;
    }
    else
    {
        if(sTheatreLogs.empty())
            { return; }
        TheatreLog& log = sTheatreLogs.back();
        log.unload_time.Start();
        if(log.load_time.Running())
            { log.load_time.Stop(); }
    }
}

void ImGui_Debugger::StopTheatreTiming(bool loading)
{
    if(sTheatreLogs.empty())
        { return; }
    TheatreLog& log = sTheatreLogs.back();
    if(loading)
        { log.load_time.Stop(); }
    else
        { log.unload_time.Stop(); }
}

static void s_GeneralDebuggingWindow()
{
    BeginChild("General Debugging");
    if(CollapsingHeader("Frame Profiler##0"))
    {
        Checkbox("Frame Profiler", &sEnableFrameCounter);
        BeginDisabled(not sEnableFrameCounter);
            if(Checkbox("FPS Counter", &sEnableFps))
                { Manager::FPSCounter(sEnableFps); }
            Checkbox("Frametime (in ms)", &sEnableFrametime);
            Checkbox("Tick Rate", &sEnableTickRate);
            Checkbox("Tick Interval", &sEnableTickInterval);
            if(Checkbox("Theatre & UI Render Frametime (in ms)", &sEnableRenderManagerFrametime))
                { g_pRenderManager->CalculateFrameTime(sEnableRenderManagerFrametime); }
        EndDisabled();
    }
#ifdef NOSTALGIA_DEBUGGING
    if(CollapsingHeader("Message Labels"))
    {
        ushort i{0};
        for(auto label : __all_labels_for_debugging)
        {
            if(TreeNode(label->name()))
            {
                Checkbox(std::format("Use ANSI Codes##{}",i).data(), &label->enable_ansi_sequence);
                SetNextItemWidth((label->label().size() * 7) + 8);
                InputText(std::format("Label##{}",i).data(), &label->label(),
                    ImGuiInputTextFlags_AutoSelectAll |
                    ImGuiInputTextFlags_NoHorizontalScroll);
                TreePop();
            }
            ++i;
        }
    }
    if(CollapsingHeader("Messages"))
    {
        SeparatorText("General");
            Checkbox("Print Event Logs", &gPrintEventLogs);
            Checkbox("Print Input Logs", &gPrintInputLogs);
        SeparatorText("Theatre");
            static auto save_msgs{Console::try_GetVariable("Theatre.debug_save_msgs")};
            bool save_msgs_b{(bool)save_msgs->int_value};
            if(Checkbox("Print TheatreFile Save Progress", &save_msgs_b))
                { Console::SetVariable("Theatre.debug_save_msgs", save_msgs_b); }
            Checkbox("Print TheatreFile Lexer Logs", &TheatreFile::gDebugPrintLexerLogs);
            Checkbox("Print TheatreFile Parser Logs", &TheatreFile::gDebugPrintParserLogs);
            Checkbox("Disable Whitespace In Lexer Logs", &TheatreFile::gDebugDontPrintWhitespaceInLexerLogs);
            Checkbox("Disable Comments In Lexer Logs", &TheatreFile::gDebugDontPrintCommentsInLexerLogs);
        SeparatorText("Rendering");
            Checkbox("Draw Command", &gPrintDrawLogs);
        if(RendererAPI::GetAPI() == GraphicsAPI::OpenGL)
        {
            SeparatorText("OpenGL");
            Checkbox("Enable Notification Messages", &gOpenGLEnableNotificationMesssages);
            const char* gl_debug_severities{"None\0Low\0Medium\0High\0"};
            static int gl_debug_selection{(int)gOpenGLMessageFilter};
            if(Combo("Minimum Message Severity Filter", &gl_debug_selection, gl_debug_severities))
                { gOpenGLMessageFilter = static_cast<DebugMessageSeverityFilter>(gl_debug_selection); }
        }
        SeparatorText("Jolt Physics");
            Checkbox("Contact Validate",  &gJoltDebugMessageAllow_ContactValidate);  SameLine();
            Checkbox("Contact Added",     &gJoltDebugMessageAllow_ContactAdded);     SameLine();
            Checkbox("Contact Persisted", &gJoltDebugMessageAllow_ContactPersisted); SameLine();
            Checkbox("Contact Removed",   &gJoltDebugMessageAllow_ContactRemoved);
            Checkbox("Body Activated",    &gJoltDebugMessageAllow_BodyActivated);    SameLine();
            Checkbox("Body Deactivated",  &gJoltDebugMessageAllow_BodyDeactivated);
    }
#endif // NOSTALGIA_DEBUGGING
    if(CollapsingHeader("Rendering"))
    {
        static auto text_rendering{Console::try_GetVariable("Theatre.draw_text_new")},
            enable3d{Console::try_GetVariable("Theatre.draw_3d")},
            enable2d{Console::try_GetVariable("Theatre.draw_2d")};
        bool text_rendering_b{(bool)text_rendering->int_value},
            enable3d_b{(bool)enable3d->int_value},
            enable2d_b{(bool)enable2d->int_value};
        if(Checkbox("Use New WIP Text Rendering Method", &text_rendering_b))
            { Console::SetVariable("Theatre.draw_text_new", text_rendering_b); }
        if(Checkbox("Render 3D Thinkers", &enable3d_b))
            { Console::SetVariable("Theatre.draw_3d", enable3d_b); }
        if(Checkbox("Render 2D Thinkers", &enable2d_b))
            { Console::SetVariable("Theatre.draw_2d", enable2d_b); }
        Checkbox("Global Wireframe Mode", &Settings::Graphics::GlobalWireframe);
        static int sSelected{0};
        static const char* sSelectableNames{"Default\0Vertex Colors\0Vertex Normals\0Vertex UVs\0"};
        if(Combo("Shader Output", &sSelected, sSelectableNames))
            { gShaderDebugOutput = static_cast<ushort>(sSelected); }
    }
    if(CollapsingHeader("Engine"))
    {
        DragInt("Tick Rate", &Settings::Engine::TickRate);
        Text("Tick Interval: %f", Settings::Engine::TickInterval());
    }
    if(CollapsingHeader("Window"))
    {
        if(TreeNode("Window Info"))
        {
            Text("Title: %s\nPosition: [%d, %d]Size: [%d, %d]",
                MainWindow()->GetTitle(),
                MainWindow()->GetXPosition(),
                MainWindow()->GetYPosition(),
                MainWindow()->GetWidth(),
                MainWindow()->GetHeight());
            TreePop();
        }
        bool fullscreen{MainWindow()->IsFullscreen()};
        if(Checkbox("Fullscreen", &fullscreen))
        {
            MainWindow()->SetWindowMode((MainWindow()->IsFullscreen())
                ? IWindow::WINDOW_MODE_WINDOWED
                : IWindow::WINDOW_MODE_FULLSCREEN);
        }
        static const std::map<Settings::Graphics::StretchMode, const char*>
            stretch_mode{
                { Settings::Graphics::StretchMode::Disabled, "Disabled"},
                { Settings::Graphics::StretchMode::Viewport, "Viewport"},
            };
        if(BeginCombo("Stretch Mode", stretch_mode.at(Settings::Graphics::Stretch::Mode)))
        {
            for(FAUTO [mode, name] : stretch_mode)
            {
                const bool is_selected{mode == Settings::Graphics::Stretch::Mode};
                if(Selectable(name))
                    { Settings::Graphics::Stretch::Mode = mode; }
                if(is_selected)
                    { SetItemDefaultFocus(); }
            }
            EndCombo();
        }
        static const std::map<Settings::Graphics::StretchAspect, const char*>
            stretch_aspect{
                { Settings::Graphics::StretchAspect::Ignore,     "Ignore"},
                { Settings::Graphics::StretchAspect::Keep,       "Keep"},
                { Settings::Graphics::StretchAspect::KeepWidth,  "KeepWidth"},
                { Settings::Graphics::StretchAspect::KeepHeight, "KeepHeight"},
            };
        if(BeginCombo("Stretch Aspect", stretch_aspect.at(Settings::Graphics::Stretch::Aspect)))
        {
            for(FAUTO [aspect, name] : stretch_aspect)
            {
                const bool is_selected{aspect == Settings::Graphics::Stretch::Aspect};
                if(Selectable(name))
                    { Settings::Graphics::Stretch::Aspect = aspect; }
                if(is_selected)
                    { SetItemDefaultFocus(); }
            }
            EndCombo();
        }

#       ifndef WAYLAND_DISPLAY
            vector<2,int,VectorMembers::XYZ> position{MainWindow()->GetPosition()};
            if(DragInt2("Position", &position.x(), &position.y()))
                { MainWindow()->SetPosition(position); }
            int scale[2] {
                static_cast<int>(MainWindow()->GetScale().width()),
                static_cast<int>(MainWindow()->GetScale().height()),
            };
            if(DragInt2("Size", scale))
                { MainWindow()->SetScale({(uint)scale[0], (uint)scale[1]}); } // Evil c-style cast
            NewLine();
#       else
            Text("Position: [%d, %d]",
                MainWindow()->GetXPosition(),
                MainWindow()->GetYPosition());
            Text("Size: [%d, %d]",
                MainWindow()->GetWidth(),
                MainWindow()->GetHeight());
#       endif // WAYLAND_DISPLAY
    }
    if(CollapsingHeader("Player"))
    {
        SeparatorText("Movement");
        SliderFloat("Movement Speed", &Settings::Player::MovementSpeed, 0.0f, 10.0f);
        DragFloat("Movement Acceleration", &Settings::Player::MovementAcceleration, 0.01f, 0.0f);
        SeparatorText("Mouse");
#pragma message("TODO: re-implement raw mouse motion")
        if(Checkbox("Raw Mouse Motion", &Settings::Player::RawMouseMotion))
            { print_debug("Raw mouse motion toggle not yet implemented"); }
        SliderFloat("Mouse Sensitivity", &Settings::Player::MouseSensitivity, 0.0f, 5.0f);
        SliderFloat("Mouse Sensitivity Multiplier", &Settings::Player::MouseSensitivityScale, 0.0f, 1.0f);
    }
    EndChild();
}

enum _fps_counter_position_enum : int { FPS_COUNTER_UL, FPS_COUNTER_BL, FPS_COUNTER_UR, FPS_COUNTER_BR, FPS_COUNTER_NA };
static const std::map<_fps_counter_position_enum, const char*> _counter_pos_map
{
    {FPS_COUNTER_UL, "Upper-Left"},
    {FPS_COUNTER_BL, "Bottom-Left"},
    {FPS_COUNTER_UR, "Upper-Right"},
    {FPS_COUNTER_BR, "Bottom-Right"},
    {FPS_COUNTER_NA, "N/A"},
};

static void s_FPSCounter()
{
    static ImVec2 _fps_counter_pos{0, 0};
    static _fps_counter_position_enum _fps_counter_position{FPS_COUNTER_UL};
    static bool _fps_position_changed{true};
    static uint _graph_max_size{60};
    static uint _current_graph_index{0};
    static std::vector<float> _fps_values(_graph_max_size),
        _frametime_values(_graph_max_size),
        _tps_values(_graph_max_size),
        _tickint_values(_graph_max_size),
        _theatre_frametime_values(_graph_max_size),
        _ui_frametime_values(_graph_max_size);
    if(sEnableFrameCounter)
    {
        if(_fps_position_changed)
        {
            SetNextWindowPos(_fps_counter_pos);
            _fps_position_changed = false;
        }
        Begin("FPS & Frametime",
            nullptr,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
            if(++_current_graph_index >= _graph_max_size)
            {
                _current_graph_index = 0;
                _fps_values = std::vector<float>(_graph_max_size);
                _frametime_values = std::vector<float>(_graph_max_size);
                _tps_values = std::vector<float>(_graph_max_size);
                _tickint_values = std::vector<float>(_graph_max_size);
                _theatre_frametime_values = std::vector<float>(_graph_max_size);
                _ui_frametime_values = std::vector<float>(_graph_max_size);
            }
            if(sEnableFps)
            {
                auto _fps_val{Manager::GetFPS()};
                _fps_values[_current_graph_index] = _fps_val;
                TextF("FPS: {}", _fps_val);
            }
            if(sEnableFrametime)
            {
                auto _frametime_val{Manager::DeltaTime() * 1000.0};
                _frametime_values[_current_graph_index] = _frametime_val;
                TextF("Frametime: {:7.5f}ms", _frametime_val);
            }
            if(sEnableTickRate)
            {
                auto _tps_val{Settings::Engine::TickRate};
                _tps_values[_current_graph_index] = _tps_val;
                TextF("TPS: {}", _tps_val);
            }
            if(sEnableTickInterval)
            {
                auto _tickint_val{Settings::Engine::TickInterval() * 1000.0};
                _tickint_values[_current_graph_index] = _tickint_val;
                TextF("Tick Interval: {:7.5f}ms", _tickint_val);
            }
            if(sEnableRenderManagerFrametime)
            {
                auto _theatre_val{g_pRenderManager->GetTheatreFrameTime() * 1000.0};
                auto _ui_val{g_pRenderManager->GetUIFrameTime() * 1000.0};
                _theatre_frametime_values[_current_graph_index] = _theatre_val;
                _ui_frametime_values[_current_graph_index] = _ui_val;
                TextF("Theatre Render Frametime: {:7.5f}ms", _theatre_val);
                TextF("UI Render Frametime: {:7.5f}ms", _ui_val);
            }
            if(CollapsingHeader("Graphs"))
            {
                if(DragUInt("Max Plot Points", &_graph_max_size))
                {
                    _current_graph_index = 0;
                    _fps_values = std::vector<float>(_graph_max_size);
                    _frametime_values = std::vector<float>(_graph_max_size);
                    _tps_values = std::vector<float>(_graph_max_size);
                    _tickint_values = std::vector<float>(_graph_max_size);
                    _theatre_frametime_values = std::vector<float>(_graph_max_size);
                    _ui_frametime_values = std::vector<float>(_graph_max_size);
                }
                if(sEnableFps)
                    { PlotLines("FPS", _fps_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50}); }
                if(sEnableFrametime)
                    { PlotLines("Frametime", _frametime_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50}); }
                if(sEnableTickRate)
                    { PlotLines("TPS", _tps_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50}); }
                if(sEnableTickInterval)
                    { PlotLines("Tick Interval", _tickint_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50}); }
                if(sEnableRenderManagerFrametime)
                {
                    PlotLines("Theatre Render Frametime", _theatre_frametime_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50});
                    PlotLines("UI Render Frametime", _ui_frametime_values.data(), _graph_max_size, 0, nullptr, FLT_MAX, FLT_MAX, {0, 50});
                }
            }
            ImVec2 _window_size{GetWindowSize()};
            if(BeginCombo("Position", _counter_pos_map.at(_fps_counter_position)))
            {
                for(FAUTO [value, name] : _counter_pos_map)
                {
                    if(value == FPS_COUNTER_NA)
                        { continue; }
                    const bool is_selected{value == _fps_counter_position};
                    if(Selectable(name))
                    {
                        switch(value)
                        {
                        case FPS_COUNTER_UL:
                            _fps_counter_pos = {0, 0};
                            break;
                        case FPS_COUNTER_BL:
                            _fps_counter_pos = {0, static_cast<float>(MainWindow()->GetHeight() - _window_size[1])};
                            break;
                        case FPS_COUNTER_UR:
                            _fps_counter_pos = {static_cast<float>(MainWindow()->GetWidth() - _window_size[0]), 0};
                            break;
                        case FPS_COUNTER_BR:
                            _fps_counter_pos = {static_cast<float>(MainWindow()->GetWidth() - _window_size[0]), static_cast<float>(MainWindow()->GetHeight() - _window_size[1])};
                            break;
                        default:
                            break;
                        }
                        _fps_counter_position = value;
                        _fps_position_changed = true;
                    }
                    if(is_selected)
                        { SetItemDefaultFocus(); }
                }
                EndCombo();
            }
            if(not _fps_position_changed and
                _fps_counter_position != FPS_COUNTER_NA and
                _fps_counter_pos != GetWindowPos())
                    { _fps_counter_position = FPS_COUNTER_NA; }
        End();
    }
}

static void s_PrintStopwatchLog(const StopwatchLog& stopwatch, size_t i)
{
    if(!stopwatch.Finished())
        { return; }
    Separator();
    Text("Stopwatch #%zu", i);
    Text("Time: %f", stopwatch.Duration());
    if(IsItemHovered())
        { SetTooltip("Started @ %s (hi-res: %f)\nStopped @ %s (hi-res: %f)", stopwatch.StartTime().c_str(), stopwatch.HiResStartTime(), stopwatch.StopTime().c_str(), stopwatch.HiResStopTime()); }
    Text("Message: '%s'", stopwatch.Message().c_str());
}

void ImGui_Debugger::m_AutomaticStopwatchWindow(float width)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    BeginChild("Automatic Stopwatch", {width, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
    Text("%s", "Automatic Stopwatch");
    static bool s_DummyAutoStopwatchEnabled = true;
    Checkbox("Enable Automatic Stopwatch", &s_DummyAutoStopwatchEnabled);
    if(IsItemHovered())
        { SetTooltip("%s", "This enables/disables the program's ability to create new stopwatches"); }
    if(s_DummyAutoStopwatchEnabled != sAutoStopwatchEnabled)
        { s_HandleAutomaticStopwatchToggle(); }

    if(Button("Clear Logs"))
        { sStopwatchLogs.clear(); }

    BeginGroup();
        static bool sort_newest_first = false;
        ImGuiDir button_arrow_direction = (sort_newest_first) ? ImGuiDir_Down : ImGuiDir_Up;

        Text("Sorting by: %s", (sort_newest_first) ? "Newest" : "Oldest");
        SameLine();
        if(ArrowButton("SortingDirection", button_arrow_direction))
            { sort_newest_first = !sort_newest_first; }
    EndGroup();

    Text("Logs:");
    if(!sStopwatchLogs.empty())
    {
        std::vector<StopwatchLog> logs = {sStopwatchLogs.cbegin(), sStopwatchLogs.cend()};

        if(sort_newest_first)
        {
            for(size_t i = (logs.size() - 1) ; i > 0 ; --i)
                { s_PrintStopwatchLog(logs.at(i), i); }
        }
        else
        {
            for(size_t i = 0 ; i < logs.size() ; ++i)
                { s_PrintStopwatchLog(logs.at(i), i); }
        }
    }
    EndChild();
}

void ImGui_Debugger::m_ManualStopwatchWindow(float width)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    BeginChild("Manual Stopwatch", {width, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
    Text("%s", "Manual Stopwatch");
    if(Button("Start Stopwatch"))
    {
        if(sManualStopwatchLogs.empty() || !sManualStopwatchLogs.back().Running())
            { sManualStopwatchLogs.emplace_back().Start(); }
    }
    if(Button("Stop Stopwatch"))
    {
        if(!sManualStopwatchLogs.empty() && sManualStopwatchLogs.back().Running())
            { sManualStopwatchLogs.back().Stop(); }
    }

    std::string last_stopwatch_time = (sManualStopwatchLogs.empty()) ? "N/A" : std::to_string(sManualStopwatchLogs.back().Duration());
    Text("Most Recent Stopwatch: %s", last_stopwatch_time.c_str());

    if(Button("Clear Logs"))
        { sManualStopwatchLogs.clear(); }
    Text("Logs:");

    if(!sManualStopwatchLogs.empty())
    {
        Separator();
        for(size_t i = 0 ; i < sManualStopwatchLogs.size() ; ++i)
        {
            const StopwatchLog& stopwatch = sManualStopwatchLogs.at(i);
            Text("Manual Stopwatch #%zu", i);
            Text("Time: %f", stopwatch.Duration());
            if(IsItemHovered())
            {
                std::string tooltip_extra = (stopwatch.Finished()) ? std::format("{}", stopwatch.StopTime()) : "N/A";
                SetTooltip("Started @ %s\nStopped @ %s", stopwatch.StartTime().c_str(), tooltip_extra.c_str());
            }
        }
    }
    EndChild();
}

void ImGui_Debugger::DebugConsoleWindow()
{
    static std::vector<std::string> history{};
    static std::string buffer{};
    if(buffer.ends_with('`'))
        { buffer.clear(); }
    if(!gDebugConsoleOpened)
        { return; }
    if(Begin("Debug Console"))
    {
        InputText("##InputLine", &buffer);
        if(IsItemDeactivatedAfterEdit())
        {
            Console::ProcessLine(buffer);
            history.push_back(buffer);
            buffer.clear();
            SetKeyboardFocusHere(-1);
        }
        Separator();
        const float footer_height_to_reserve{GetStyle().ItemSpacing.y + GetFrameHeightWithSpacing()};
        auto width{GetWindowSize().x};
        if(BeginChild("##History", {0, -footer_height_to_reserve}, ImGuiChildFlags_NavFlattened))
        {
            PushTextWrapPos(GetCursorPosX() + width - 10);
            for(FAUTO line : history)
                { Text("%s", line.data()); }
            PopTextWrapPos();
        }
        EndChild();
    }
    End();
}
