#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application/application.hpp"
#include "components/event_handling.hpp"

#include <string>

class ApplicationEventsHandler final : public OnAppEvent, public OnInput
{
public:
    void Event(AppEvent*) final;
    void Input(InputEvent*) final;
};

class NostalgiaGoggles final : public IApplication
{
public:
    int Main() final;
    void Stop() final;
    const char* Name() final { return "Nostalgia Goggles"; }
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
