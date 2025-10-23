#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application.hpp"

#include <string>

class NostalgiaGoggles : public _Application
{
    typedef _Application BaseClass;

public:
    bool Create();
    bool PreInit();
    int Main();
    void Shutdown();
    void PostShutdown();
    const char* GetAppName() { return "Nostalgia Goggles"; }
};

extern std::string gToggleFullscreen;
extern std::string gToggleMouseCapture;
extern NostalgiaGoggles* g_pApplication;

#endif // NOSTALGIA_GOGGLES_H
