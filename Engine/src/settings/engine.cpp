#include "engine.hpp"

int   Settings::Engine::TickRate{70};
float Settings::Engine::TickInterval() { return (1.0f / TickRate); }
