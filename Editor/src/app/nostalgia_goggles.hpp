#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application/application.hpp"

#include <string>

class NostalgiaGoggles : public IApplication
{
public:
    int Main();
    void Stop();
    const char* GetAppName() { return "Nostalgia Goggles"; }
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
