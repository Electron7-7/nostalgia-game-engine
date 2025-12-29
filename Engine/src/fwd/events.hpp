#ifndef EVENTS_FWD_H
#define EVENTS_FWD_H

#include "fwd/event_types.hpp"
#include "core/smart_pointers.hpp"
#include <vector>

extern constinit const unsigned short MaxActions;
class InputAction;

struct KeyID;
namespace Key
{
    struct Modifier;
    struct Modifiers;
    extern constinit const unsigned long KeysCount;
}

using event_queue_t = std::vector<Shared<IEvent>>;
class EventQueue;

#endif // EVENTS_FWD_H
