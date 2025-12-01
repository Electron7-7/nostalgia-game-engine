#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application/application.hpp"

#include <string>

class NostalgiaGoggles : public IApplication
{
public:
    int Main() override;
    void Stop() override;
    const char* Name() override { return "Nostalgia Goggles"; }
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
