#include "settings.hpp"

#pragma message("FIXME: These are incompatible with Wayland")
int Settings::Window::Width = 1280;
int Settings::Window::Height = 720;
int Settings::Window::FullscreenWidth = 1920;
int Settings::Window::FullscreenHeight = 1080;
int Settings::Window::XPosition = 0;
int Settings::Window::YPosition = 0;
bool Settings::Window::Fullscreen = false;
const char* Settings::Window::Name = "Nostalgia Game Engine";
