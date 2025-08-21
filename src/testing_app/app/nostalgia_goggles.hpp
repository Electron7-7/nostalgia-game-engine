#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application.hpp"

class NostalgiaGoggles : public _Application
{
    typedef _Application BaseClass;

public:
    virtual bool Create();
    virtual bool PreInit();
    virtual int Main();
    virtual void Shutdown();
    virtual void PostShutdown();
    virtual const char* GetAppName() { return "Nostalgia Goggles"; };
};

extern NostalgiaGoggles* g_pApplication;

#endif // NOSTALGIA_GOGGLES_H
