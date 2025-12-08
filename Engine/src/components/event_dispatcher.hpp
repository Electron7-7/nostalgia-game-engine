#ifndef EVENT_DISPATCHER_COMPONENT_H
#define EVENT_DISPATCHER_COMPONENT_H

class IEvent; // Forward Declaration

class EventDispatcher
{ public: virtual void DispatchEvent(IEvent*); };

#endif // EVENT_DISPATCHER_COMPONENT_H
