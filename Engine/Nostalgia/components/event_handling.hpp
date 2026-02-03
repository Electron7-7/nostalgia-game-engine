#ifndef EVENT_HANDLING_COMPONENTS_H
#define EVENT_HANDLING_COMPONENTS_H

#include <Nostalgia/fwd/event_types.hpp>

class OnInput
{
public:
    virtual void Input(InputEvent*) = 0;
};

class OnAppEvent
{
public:
    virtual void Event(AppEvent*) = 0;
};

class OnEngineEvent
{
public:
    virtual void Event(EngineEvent*) = 0;
};

#endif // EVENT_HANDLING_COMPONENTS_H
