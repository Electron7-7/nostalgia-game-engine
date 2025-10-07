#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"

#include <vector>

class EventQueue
{
public:
    bool BeginQueueing();
    bool BeginProcessing();
    bool EndQueueing();
    bool EndProcessing();

    bool QueueEvent(const InputEvent& Event);
    bool GetNextEvent(InputEvent& Output);
    bool ReplaceQueue(const std::vector<InputEvent>& Queue);

    void PrintQueueLog();

private:
    std::vector<InputEvent> mQueue = {};
    bool mReplacingQueue   = false;
    bool mProcessingEvents = false;
    bool mQueueingEvents   = false;
};

#endif // EVENT_QUEUE_H
