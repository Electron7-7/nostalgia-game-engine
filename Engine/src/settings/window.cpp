#include "window.hpp"
#include "managers/backend_manager.hpp"

#pragma message("FIXME: Make this work with Wayland/other tiling managers")

using namespace Settings;

static std::string sName{"Nostalgia Game Engine"};
static bool sIsFullscreen{false};
static WindowInfo sWindowedInfo{sName, {320, 180}, {1280, 720}};
static WindowInfo sFullscreenInfo{sName, {0, 0}, {1920, 1080}};

static void s_UpdateWindow()
{ g_pBackendManager->Windowing()->UpdateState(); }

static WindowInfo& s_Info()
{ return (sIsFullscreen) ? sFullscreenInfo : sWindowedInfo; }

WindowInfo& WindowingBackend::s_vInfo(bool get_current_state_info)
{
    if(get_current_state_info)
        { return s_Info(); }
    return (sIsFullscreen) ? sWindowedInfo : sFullscreenInfo;
}

const WindowInfo& Window::Info()
{ return s_Info(); }

const WindowInfo& Window::WindowedInfo()
{ return sWindowedInfo; }

const WindowInfo& Window::FullscreenInfo()
{ return sFullscreenInfo; }

void Window::setAllInfo(const WindowInfo& info)
{ sFullscreenInfo = sWindowedInfo = info; s_UpdateWindow(); }

void Window::setCurrentInfo(const WindowInfo& info)
{ s_Info() = info; s_UpdateWindow(); }

void Window::setWindowedInfo(const WindowInfo& info)
{ sWindowedInfo = info; if(!Fullscreen()) { s_UpdateWindow(); } }

void Window::setFullscreenInfo(const WindowInfo& info)
{ sFullscreenInfo = info; if(Fullscreen()) { s_UpdateWindow(); } }

const std::string& Window::Name()
{ return sName; }

const char* Window::c_Name()
{ return sName.data(); }

bool Window::Fullscreen()
{ return sIsFullscreen; }

const position_t& Window::Position()
{ return Info().position; }

const scale_t& Window::Size()
{ return Info().size; }

const scale_t& Window::FramebufferSize()
{ return Info().framebuffer_size; }

const position_t& Window::FramebufferPosition()
{ return Info().framebuffer_position; }

bool Window::ToggleFullscreen()
{ setFullscreen(!sIsFullscreen); return sIsFullscreen; }

void Window::setFullscreen(bool fullscreen)
{ sIsFullscreen = fullscreen; s_UpdateWindow(); }

void Window::setName(const std::string& name)
{ sName = sFullscreenInfo.name = sWindowedInfo.name = name; s_UpdateWindow(); }

void Window::setPosition(const position_t& position)
{ s_Info().position = position; s_UpdateWindow(); }

void Window::setSize(const scale_t& size)
{ s_Info().size = size; s_UpdateWindow(); }

void Window::setFramebufferSize(const scale_t& size)
{ s_Info().framebuffer_size = size; s_UpdateWindow(); }

void Window::setFramebufferPosition(const position_t& position)
{ s_Info().framebuffer_position = position; s_UpdateWindow(); }
