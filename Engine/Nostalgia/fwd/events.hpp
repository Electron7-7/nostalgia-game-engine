#ifndef EVENTS_FWD_H
#define EVENTS_FWD_H

#include <Nostalgia/fwd/event_types.hpp>

extern constinit const ushort MaxActions;
class InputAction;

struct KeyID;
namespace Key
{
    struct Modifier;
    struct Modifiers;
    extern constinit const size_t KeysCount;
}

using event_queue_t = std::vector<Shared<IEvent>>;
class EventQueue;

#endif // EVENTS_FWD_H
