#include "settings.hpp"
#include "rendering/backends/backend.hpp"

int   Settings::Engine::TickRate = 70;
float Settings::Engine::TickInterval = (1.0f / TickRate);
int   Settings::Engine::GraphicsBackend  = gBackendIDs::gOpenGL;
int   Settings::Engine::WindowingBackend = gBackendIDs::wGLFW;
