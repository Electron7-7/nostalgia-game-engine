#include "imgui_implementor.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_impl_glfw.h"
#include "thirdparty/DearImGui/imgui_impl_opengl3.h"
#include "thirdparty/DearImGui/imgui_impl_opengl3_loader.h"
#include <Nostalgia/application/window.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>

#define ErrorGraphicsAPI print_error("invalid/unknown window handler detected!"); return
#define ErrorWindowingLibrary print_error("invalid/unknown graphics API detected!"); return
#define ReturnIf(CONDITION, RETURN_VALUE...) if(CONDITION) { return RETURN_VALUE; }

void ImGui_Implementor::Attach()
{
    PRINT_PRETTY_FUNCTION;
    ReturnIf(mAttached)
    mState = STATE_ATTACHING;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::OPENGL:
        switch(MainWindow()->GetNativeWindowType())
        {
        case NATIVE_GLFW_WINDOW:
            ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(MainWindow()->GetNativeWindow()), true);
            break;
        default:
            ErrorGraphicsAPI;
        }
#pragma message("TODO: store the GLSL version in a global variable")
        ImGui_ImplOpenGL3_Init("#version 430");
        break;
    default:
        ErrorWindowingLibrary;
    }
    mAttached = true;
    mState = STATE_IDLE;
}

void ImGui_Implementor::Detach()
{
    ReturnIf(!mAttached)
    mState = STATE_DETACHING;
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::OPENGL:
        ImGui_ImplOpenGL3_Shutdown();
        switch(MainWindow()->GetNativeWindowType())
        {
        case NATIVE_GLFW_WINDOW:
            ImGui_ImplGlfw_Shutdown();
            break;
        default:
            ErrorGraphicsAPI;
        }
        break;
    default:
        ErrorWindowingLibrary;
    }
    ImGui::DestroyContext();
    mAttached = false;
    mState = STATE_INACTIVE;
}

void ImGui_Implementor::Begin()
{
    if(!mAttached)
        { print_warning("!mAttached"); }
    mState = STATE_BEGINNING_FRAME;
    ImGui::GetIO().WantCaptureKeyboard = mCanHandleEvents;
    ImGui::GetIO().WantCaptureMouse    = mCanHandleEvents;

    if(!mCanHandleEvents)
    {
        ImGui::GetIO().ClearEventsQueue();
        ImGui::GetIO().ClearInputMouse();
    }

    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::OPENGL:
        ImGui_ImplOpenGL3_NewFrame();
        break;
    default:
        ErrorGraphicsAPI;
    }

    switch(MainWindow()->GetNativeWindowType())
    {
    case NATIVE_GLFW_WINDOW:
        ImGui_ImplGlfw_NewFrame();
        break;
    default:
        ErrorWindowingLibrary;
    }

    ImGui::NewFrame();
    mState = STATE_PROCESSING_FRAME;
}

void ImGui_Implementor::End()
{
    if(!mAttached)
        { print_error("!mAttached"); }
    mState = STATE_ENDING_FRAME;
    ImGuiIO& io{ImGui::GetIO()};
    io.DisplaySize = ImVec2{static_cast<float>(MainWindow()->GetWidth()), static_cast<float>(MainWindow()->GetHeight())};

    ImGui::Render();
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::OPENGL:
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        break;
    default:
        ErrorGraphicsAPI;
    }
#pragma message("TODO: decide if I'm using ImGui's docking branch for multi-viewport support (multiple imgui windows)")
    /*if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }*/

    mState = STATE_IDLE;
}

#undef ErrorGraphicsAPI
#undef ErrorWindowingLibrary
#undef ReturnIf
