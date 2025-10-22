#include "settings.hpp"
#include "backends/backend.hpp"

int   Settings::Engine::TickRate{70};
float Settings::Engine::TickInterval() { return (1.0f / TickRate); }
ID    Settings::Engine::GraphicsBackend{BackendIDs::gOpenGL};
ID    Settings::Engine::WindowingBackend{BackendIDs::wGLFW};
