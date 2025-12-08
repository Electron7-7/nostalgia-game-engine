#ifndef NOSTALGIA_GOGGLES_H
#define NOSTALGIA_GOGGLES_H

#include "application/application.hpp"

#include <string>

class NostalgiaGoggles : public IApplication, public OnInput
{
public:
    int Main() override;
    void Stop() override;
    const char* Name() override { return "Nostalgia Goggles"; }
    void Event(AppEvent*) override;
    void Input(InputEvent*) override;
};

extern std::string gToggleFullscreen;

#endif // NOSTALGIA_GOGGLES_H
