#ifndef TESTAPP_H
#define TESTAPP_H

#include "../Nostalgia/Nostalgia.hpp"
#include "../Nostalgia/application/application.hpp"
#include "../Nostalgia/components/event_handling.hpp"

class TestApplication final : public IApplication
{
public:
    int Main() final;
    void Stop() final;
    const char* Name() final;
    void Input(InputEvent*) final;
    void Event(IEvent*) final;
};

extern std::string gToggleFullscreen;

#endif // TESTAPP_H
