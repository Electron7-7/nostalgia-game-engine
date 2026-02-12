#ifndef APPLICATION_H
#define APPLICATION_H

#include <Nostalgia/application/window.hpp>
#include <Nostalgia/components/event_handling.hpp>

class IApplication : public OnInput, public OnAppEvent
{
public:
    virtual int  Main() { return 1; }
    virtual void Stop() {}
    virtual const char* Name() { return "IApplication"; }

    IApplication();
    virtual ~IApplication();

    Unique<IWindow>& GetWindow();

protected:
    Unique<IWindow> mMainWindow{nullptr};
};

IApplication* Application();
Unique<IWindow>& MainWindow();

#endif // APPLICATION_H
