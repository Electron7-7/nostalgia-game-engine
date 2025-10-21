#include "event_queue.hpp"
#include "common/colors.hpp"

#include <format>
#include <print>

EventQueue::EventQueue() = default;
EventQueue::EventQueue(const std::vector<InputEvent>& queue):
    mQueue{queue}
{}

void EventQueue::PrintQueueLog()
{
    std::print("{}--- Start InputEvent Queue Log ---{}\n", Style::Bold + Foreground::Blue, Style::Reset);
    for(const auto& event : mQueue)
        { std::print("{}\n", event.Log()); }
    std::print("{}--- Stop InputEvent Queue Log ---{}\n", Style::Bold + Foreground::Blue, Style::Reset);
}

void EventQueue::ReplaceQueue(const std::vector<InputEvent>& new_queue)
{ mQueue = new_queue; }

bool EventQueue::QueueEvent(const InputEvent& event)
{
    if(event.empty())
        { return false; }
    mQueue.push_back(event);
    return true;
}

bool EventQueue::GetNextEvent(InputEvent& output)
{
    if(mQueue.empty())
        { return false; }
    output = InputEvent{mQueue.back()};
    mQueue.pop_back();
    return true;
}

bool EventQueue::empty() const
{ return mQueue.empty(); }
