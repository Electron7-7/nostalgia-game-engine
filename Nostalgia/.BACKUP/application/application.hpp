#ifndef APPLICATION_H
#define APPLICATION_H

#include <Nostalgia/application/window.hpp>
#include <Nostalgia/components/event_handling.hpp>
#include <Nostalgia/components/game_loop.hpp>

class IApplication : public OnInput, public OnEvent, public OnUpdate, public OnTick, public OnTheatreChanged
{
public:
    virtual int  Main() = 0;
    virtual void Stop() = 0;
    virtual const char* Name() { return "IApplication"; }

    virtual void Input(InputEvent*) override {}
    virtual void Event(IEvent*) override {}
    virtual void Update() override {}
    virtual void Tick() override {}
    virtual void TheatreEntered() override {}
    virtual void TheatreExited() override {}

    virtual const IWindow* MainWindow() const = 0;
    virtual IWindow* MainWindow() = 0;

    // virtual const IWindow* GetWindow(ID inWindowUID) const;
    // virtual IWindow* GetWindow(ID inWindowUID);

    IApplication();
    virtual ~IApplication();
};

IApplication* Application();

#endif // APPLICATION_H
