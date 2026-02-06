#ifndef EVENT_TYPES_FWD_H
#define EVENT_TYPES_FWD_H

enum class EventPriority : unsigned int;
enum class EventType : unsigned int;
#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

class IEvent;
template<EventPriority _Priority>
    class CEvent;
class AppEvent;
class EngineEvent;
class InputEvent;
class InputEventMouseMotion;
class InputEventAction;
class InputEventBinding;

#endif // EVENT_TYPES_FWD_H
