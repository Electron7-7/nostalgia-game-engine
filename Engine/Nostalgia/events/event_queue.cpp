#include "event_queue.hpp"

std::vector<Shared<IEvent>>& EventQueue::get() noexcept
{ return mEvents; }

bool EventQueue::empty() noexcept
{ return mEvents.empty(); }

void EventQueue::clear() noexcept
{ mEvents.clear(); }

std::recursive_mutex& EventQueue::get_mutex() noexcept
{ return mEventsMutex; }
