#include "settings.hpp"
#include "managers/backend_manager.hpp"

#pragma message("FIXME: Make this work with Wayland/other tiling managers")

using namespace Settings;

static std::string sName{"Nostalgia Game Engine"};
static bool sIsFullscreen{false};
static WindowInfo sWindowedInfo{sName, false};
static WindowInfo sFullscreenInfo{sName, true};

static void s_UpdateWindow()
{ g_pBackendManager->Windowing()->UpdateState(); }

static WindowInfo& s_Info()
{ return (sIsFullscreen) ? sFullscreenInfo : sWindowedInfo; }

const WindowInfo& Window::Info()
{ return s_Info(); }

void Window::setInfo(const WindowInfo& info)
{
    (sIsFullscreen) ? sFullscreenInfo = info : sWindowedInfo = info; // bc I don't trust `s_Info`...
    sName = info.name;
    sIsFullscreen = info.fullscreen;
    s_UpdateWindow();
}

const std::string& Window::Name()
{ return sName; }

const char* Window::c_Name()
{ return sName.data(); }

bool Window::Fullscreen()
{ return Info().fullscreen; }

const position_t& Window::Position()
{ return Info().position; }

const scale_t& Window::Size()
{ return Info().size; }

const scale_t& Window::FramebufferSize()
{ return Info().framebuffer_size; }

bool Window::ToggleFullscreen()
{ setFullscreen(!sIsFullscreen); return sIsFullscreen; }

void Window::setFullscreen(bool fullscreen)
{ sIsFullscreen = fullscreen; s_UpdateWindow(); }

void Window::setName(const std::string& name)
{ sName = sFullscreenInfo.name = sWindowedInfo.name = name; s_UpdateWindow(); }

void Window::setPosition(const position_t& position)
{ s_Info().position = position; }

void Window::setSize(const scale_t& size)
{ s_Info().size = size; }

void Window::setFramebufferSize(const scale_t& size)
{ s_Info().framebuffer_size = size; }
