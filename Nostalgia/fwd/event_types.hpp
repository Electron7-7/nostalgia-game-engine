#ifndef EVENT_TYPES_FWD_H
#define EVENT_TYPES_FWD_H

enum class EventPriority : uint;
enum class EventType : uint;
class IEvent;
template<EventPriority _Priority>
    class CEvent;
class WindowEvent;
class InputEvent;
class InputEventMouseMotion;
class InputEventAction;
class InputEventBinding;

#endif // EVENT_TYPES_FWD_H
