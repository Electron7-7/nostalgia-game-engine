#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.hpp"

#include <memory>

class IApplication
{
public:
    virtual int  Main() { return 1; }
    virtual void Stop() {}
    virtual const char* Name() { return "IApplication"; }

    IApplication();
    virtual ~IApplication() = default;

    IWindow& GetWindow() const { return *mMainWindow; }

protected:
    std::unique_ptr<IWindow> mMainWindow{nullptr};
};

IApplication* Application();
IWindow& MainWindow();

#endif // APPLICATION_H
