#include "event_queue.hpp"
#include "colors.hpp"

#include <format>
#include <print>

void EventQueue::PrintQueueLog()
{
    std::print("{}--- Start InputEvent Queue Log ---{}\n", Style::Bold + Foreground::Blue, Style::Reset);
    for(const auto& event : mQueue)
        { std::print("{}\n", event.Log()); }
    std::print("{}--- Stop InputEvent Queue Log ---{}\n", Style::Bold + Foreground::Blue, Style::Reset);
}

bool EventQueue::ReplaceQueue(const std::vector<InputEvent>& new_queue)
{
    if(mQueueingEvents || mReplacingQueue)
        { return false; }
    mReplacingQueue = true;
    mQueue = new_queue;
    return !(mReplacingQueue = false);
}

bool EventQueue::BeginQueueing()
{
    if(mReplacingQueue || mQueueingEvents || mProcessingEvents)
        { return false; }
    mQueue.clear();
    return mQueueingEvents = true;
}

bool EventQueue::BeginProcessing()
{
    if(mProcessingEvents || mReplacingQueue || mQueueingEvents)
        { return false; }
    return mProcessingEvents = true;
}

bool EventQueue::EndQueueing()
{
    if(!mQueueingEvents)
        { return false; }
    return !(mQueueingEvents = false);
}

bool EventQueue::EndProcessing()
{
    if(!mProcessingEvents)
        { return false; }
    return !(mProcessingEvents = false);
}

bool EventQueue::QueueEvent(const InputEvent& event)
{
    if(!event.Valid() || mReplacingQueue || !mQueueingEvents)
        { return false; }
    mQueue.push_back(event);
    return true;
}

bool EventQueue::GetNextEvent(InputEvent& output)
{
    if(!mProcessingEvents || mQueue.empty())
        { return false; }
    output = InputEvent(mQueue.back());
    mQueue.pop_back();
    return true;
}
