#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application.hpp"

class NostalgiaGoggles : public _Application
{
    typedef _Application BaseClass;

public:
    bool Create();
    bool PreInit();
    int Main();
    void Shutdown();
    void PostShutdown();
    const char* GetAppName()
    { return "Nostalgia Goggles"; };
};

extern NostalgiaGoggles* g_pApplication;

#endif // NOSTALGIA_GOGGLES_H
