#ifndef APPLICATION_H
#define APPLICATION_H

#include <Nostalgia/application/window.hpp>

class IApplication
{
public:
    virtual int  Main() = 0;
    virtual void Stop() = 0;
    virtual const char* Name() { return "IApplication"; }

    virtual void Input(InputEvent*) {}
    virtual void Event(IEvent*) {}
    virtual void Update() {}
    virtual void Tick() {}
    virtual void TheatreEntered() {}
    virtual void TheatreExited() {}

    virtual const IWindow* MainWindow() const = 0;
    virtual IWindow* MainWindow() = 0;

    // virtual const IWindow* GetWindow(ID inWindowUID) const;
    // virtual IWindow* GetWindow(ID inWindowUID);

    IApplication();
    virtual ~IApplication();
};

IApplication* Application();

#endif // APPLICATION_H
