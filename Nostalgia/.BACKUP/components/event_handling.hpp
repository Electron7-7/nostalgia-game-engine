#ifndef EVENT_HANDLING_COMPONENTS_H
#define EVENT_HANDLING_COMPONENTS_H

#include <Nostalgia/fwd/event_types.hpp>

class OnInput
{
public:
    virtual void Input(InputEvent*) = 0;
};

class OnEvent
{
public:
    virtual void Event(IEvent*) = 0;
};

#endif // EVENT_HANDLING_COMPONENTS_H
