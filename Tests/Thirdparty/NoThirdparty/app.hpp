#ifndef TESTING_APP_H
#define TESTING_APP_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/components/event_handling.hpp>

class TestApplication final : public IApplication
{
public:
    int Main() final;
    void Stop() final;
    const char* Name() final { return "Test Application"; }
    void Input(InputEvent*) final {}
    void Event(AppEvent*) final {}
};

extern std::string gToggleFullscreen;

#endif // TESTING_APP_H
