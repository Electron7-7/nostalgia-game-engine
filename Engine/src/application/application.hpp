#ifndef APPLICATION_H
#define APPLICATION_H

#include "window.hpp"

#include <memory>


class IApplication
{
public:
    virtual int  Main() = 0;
    virtual void Stop() = 0;
    virtual const char* GetAppName() { return "Untitled Application"; }

    IApplication();
    virtual ~IApplication() = default;

    IWindow& GetWindow() const { return *mMainWindow; }

protected:
    std::unique_ptr<IWindow> mMainWindow{nullptr};
};

extern IApplication* g_pApplication;

#endif // APPLICATION_H
