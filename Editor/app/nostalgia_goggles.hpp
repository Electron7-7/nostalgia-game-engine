#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/components/event_handling.hpp>

class NostalgiaGoggles final : public IApplication
{
public:
    int Main() final;
    void Stop() final;
    const char* Name() final { return "Nostalgia Goggles"; }
    void Input(InputEvent*) final;
    void Event(AppEvent*) final;
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
