#ifndef APPLICATION_H
#define APPLICATION_H

#include <Nostalgia/application/window.hpp>
#include <Nostalgia/components/event_handling.hpp>
#include <Nostalgia/components/game_loop.hpp>

class IApplication : public OnInput, public OnEvent, public OnUpdate, public OnTick, public OnTheatreChanged
{
public:
    virtual int  Main() { return 1; }
    virtual void Stop() {}
    virtual const char* Name() { return "IApplication"; }

    virtual void Input(InputEvent*) override {}
    virtual void Event(IEvent*) override {}
    virtual void Update() override {}
    virtual void Tick() override {}
    virtual void TheatreEntered() override {}
    virtual void TheatreExited() override {}

    IApplication();
    virtual ~IApplication();

    Unique<IWindow>& GetWindow();

protected:
    Unique<IWindow> mMainWindow{nullptr};
};

IApplication* Application();
Unique<IWindow>& MainWindow();

#endif // APPLICATION_H
