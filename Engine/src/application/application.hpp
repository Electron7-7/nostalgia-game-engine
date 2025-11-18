#ifndef APPLICATION_H
#define APPLICATION_H

#include "fwd.hpp"

/*
Big note on IApplication:
---------------------------------------
    I have no fucking idea what I'm doing and I'm just winging it here. After digging through the source code of Source, I
    can't for the fucking life of me figure out where, when, or how `IApplication` is declared, defined or even fucking written down.
    That means I'm just fucking winging the whole entire Application system.
    Time to see if I know what I'm doing and/or if I've learned anything at all...
*/

class IApplication
{
public:
    virtual int  Main() = 0;
    virtual void Stop() = 0;
    virtual const char* GetAppName() { return "Untitled Application"; }

    IApplication();
    virtual ~IApplication() = default;

    IWindow* GetWindow() const { return mMainWindow; }

protected:
    IWindow* mMainWindow{nullptr};
};

extern IApplication* g_pApplication;

#endif // APPLICATION_H
