#include "event_system.hpp"
#include "demo_parser.hpp"
#include "printing.hpp"

#include <climits>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>

EventSystem singleton_EventSystem;
EventSystem* global_EventSystem = &singleton_EventSystem;

std::vector<Event> EventSystem::_queue = {};
bool EventSystem::do_demo_recording = false;
std::string EventSystem::demo_recording_storage = "";
std::string EventSystem::demo_recording_name = "demo";
int EventSystem::demo_recording_number = 1;
std::string EventSystem::demo_recording_extension = ".demo";
#define DEMO_FILENAME (demo_recording_name + std::to_string(demo_recording_number) + demo_recording_extension)

void EventSystem::DebugPrintQueueLog()
{
    PRINTDEBUG("EventSystem::PanicClearQueue called. Printing the queue log...")

    size_t queue_size = _queue.size();
    std::string queue_printout = "";

    for(size_t i = 0 ; i < queue_size ; i++)
        queue_printout += _queue.at(i).EventLog() + "\n";

    std::print("{0}-- Start of Queue Log --{1}\n{2}\n{0}--- End of Queue Log ---{1}", COLOR_BOLD(Color::Blue), Color::Reset, queue_printout);
}

EventSystem::~EventSystem()
{
    if(do_demo_recording)
        StopRecordingDemo();
}

void EventSystem::LoadQueue(const std::vector<Event>& event_queue)
{
    ClearQueue();
    _queue = event_queue;
}

bool EventSystem::LoadDemoFromFile(const std::string &demo_file_path)
{
    DemoParser demo_parser;

    if(!demo_parser.LoadDemoFromFile(demo_file_path))
        return false;

    DebugPrintQueueLog();

    _queue = demo_parser.GetEventQueue();
    return true;
}

bool EventSystem::LoadDemoFromMemory(const std::string &demo_file)
{
    DemoParser demo_parser;

    if(!demo_parser.LoadDemoFromMemory(demo_file))
        return false;

    DebugPrintQueueLog();

    _queue = demo_parser.GetEventQueue();
    return true;
}

void EventSystem::AddEvent(const Event& new_event)
{
    _queue.insert(_queue.end(), new_event);
}

Event EventSystem::GetNextEvent()
{
    Event next_event(_queue.back());
    if(do_demo_recording)
        RecordEventToDemo(next_event);
    _queue.pop_back();
    return next_event;
}

unsigned int EventSystem::GetQueueSize()
{ return _queue.size(); }

void EventSystem::ClearQueue()
{
    DebugPrintQueueLog();

    _queue.clear();
}

void EventSystem::StartRecordingDemo()
{ do_demo_recording = true; }

#define AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES INT_MAX

bool EventSystem::StopRecordingDemo()
{
    do_demo_recording = false;
    while(std::filesystem::is_regular_file(std::filesystem::path(DEMO_FILENAME)))
    {
        if(demo_recording_number == AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES)
        {
            PRINTERROR("EventSystem::StopRecordingDemo - you either have an unreasonable amount of demo files, or something else went wrong")
            PRINT_DEBUG("The demo file that attempted to save:\n{}\n", demo_recording_storage)
            return false;
        }

        demo_recording_number++;
    }

    std::ofstream demo_file(DEMO_FILENAME);
    demo_file << demo_recording_storage;
    demo_file.close();

    return true;
}

void EventSystem::RecordEventToDemo(const Event& event)
{ demo_recording_storage += "Creation Time: [" + std::to_string(event.GetCreationTime()) + "] Command: <" + event.GetCommand().GetCommand() + ">\n"; }
