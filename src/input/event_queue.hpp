#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"

#include <vector>

class EventQueue
{
public:
    EventQueue();
    EventQueue(const std::vector<InputEvent>&);

    bool QueueEvent(const InputEvent& Event);
    bool GetNextEvent(InputEvent& Output);
    void ReplaceQueue(const std::vector<InputEvent>& Queue);

    void PrintQueueLog();

    bool empty() const;

private:
    std::vector<InputEvent> mQueue = {};
};

#endif // EVENT_QUEUE_H
