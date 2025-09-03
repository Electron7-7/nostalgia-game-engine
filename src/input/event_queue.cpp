#include "event_queue.hpp"
#include "demo/demo_parser.hpp"
#include "printing.hpp"
#include "colors.hpp"
#include "keybind.hpp"

#include <climits>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>

#define DEMO_FILENAME (demo_recording_name + std::to_string(demo_recording_number) + demo_recording_extension)

std::vector<Event> EventQueue::_active_queue = {};
std::vector<Event> EventQueue::_safe_queue = {};
unsigned int EventQueue::_last_processed_event_index = 0;

bool EventQueue::can_queue_events = false;
bool EventQueue::is_processing_events = false;
bool EventQueue::is_queueing_events = false;
bool EventQueue::is_copying_queue = false;
bool EventQueue::do_demo_recording = false;

std::string EventQueue::demo_recording_storage = "";
std::string EventQueue::demo_recording_name = "demo";
int EventQueue::demo_recording_number = 1;
std::string EventQueue::demo_recording_extension = ".demo";

void EventQueue::DebugPrintQueueLog()
{
    size_t active_queue_size = _active_queue.size();
    std::string queue_printout = "";

    for(size_t i = 0 ; i < active_queue_size ; i++)
    {
        std::vector<Event>::iterator it = _active_queue.begin();
        std::advance(it, i);
        queue_printout += it->EventLog() + "\n";
    }

    std::print("{0}-- Start of Queue Log --{1}\n{2}\n{0}--- End of Queue Log ---{1}", Style::Bold + Foreground::Blue, Style::Reset, queue_printout);
}

void EventQueue::LoadQueue(const std::vector<Event>& event_queue)
{
    ClearQueue();
    _active_queue = event_queue;
}

bool EventQueue::LoadDemoFromFile(const std::string &demo_file_path)
{
    DemoParser demo_parser;

    if(!demo_parser.LoadDemoFromFile(demo_file_path))
    { return false; }

    ClearQueue();

    _active_queue = demo_parser.GetEventQueue();
    return true;
}

bool EventQueue::LoadDemoFromMemory(const std::string &demo_file)
{
    DemoParser demo_parser;

    if(!demo_parser.LoadDemoFromMemory(demo_file))
    { return false; }

    ClearQueue();

    _active_queue = demo_parser.GetEventQueue();
    return true;
}

bool EventQueue::EnableEventQueue()
{
    if(can_queue_events)
    { return false; }

    can_queue_events = true;
    return true;
}

bool EventQueue::DisableEventQueue()
{
    if(!can_queue_events)
    { return false; }

    can_queue_events = false;
    return true;
}

SafeStatus EventQueue::try_BeginProcessing()
{
    if(is_processing_events)
    { return Status::ERROR_ALREADY_ACTIVE; }

    if(_active_queue.empty() || _last_processed_event_index >= _active_queue.size())
    { return Status::EventQueueEMPTY; }

    WAIT(is_queueing_events, 0.3); // *stares at '_active_queue' reeeeeally hard*

    _safe_queue.clear(); // Just to be, well, safe

    std::vector<Event>::iterator queue_begin = _active_queue.begin();
    std::advance(queue_begin, _last_processed_event_index);

    _safe_queue.insert(_safe_queue.cend(), queue_begin, _active_queue.end());
    _last_processed_event_index += _safe_queue.size(); // Note: This variable should be one index ahead, because we don't want to process the same event twice

    is_processing_events = true;
    return Status::NO_ERR;
}

bool EventQueue::EndProcessing()
{
    if(!is_processing_events)
    { return false; }

    is_processing_events = false;
    return true;
}

SafeStatus EventQueue::try_QueueEvents(KeyID key, bool is_released)
{
    if(!can_queue_events)
    { return Status::EventQueueNOT_ENABLED; }

    SafeReturn<KeyBinds> keybinds = GetBindings(key);

    if(keybinds.Status() != Status::NO_ERR)
    { return keybinds.Status(); }

    is_queueing_events = true;

    for(const KeyBind& keybind : keybinds.Data())
        if(keybind.OnRelease() == is_released)
        { _active_queue.push_back(Event(keybind.Command())); }

    is_queueing_events = false;

    return Status::NO_ERR;
}

SafeReturn<Event> EventQueue::GetNextEvent()
{
    if(!is_processing_events)
    { return SafeReturn(Event(), Status::EventQueueNOT_PROCESSING_EVENTS); }

    Event next_event(_safe_queue.front());

    if(do_demo_recording)
    { RecordEventToDemo(next_event); }
    // FIXME: This doesn't check the size before calling pop_front
    _safe_queue.erase(_safe_queue.cbegin());

    return SafeReturn(next_event);
}

unsigned int EventQueue::GetCurrentQueueSize()
{ return _safe_queue.size(); }

void EventQueue::ClearQueue()
{
    WAIT(is_queueing_events, 1.0) // _active_queue has one second before we flip the fuck out and kill it with hammers
    _active_queue.clear();

    WAIT(is_processing_events, 5.0) // _safe_queue gets five whole seconds because it's earned more love
    _safe_queue.clear();
}

void EventQueue::PanicClearQueue()
{
    PRINT_DEBUG("EventQueue::PanicClearQueue called. Printing the queue log...\n")
    DebugPrintQueueLog();

    // I'm just considerate like that an' shit
    bool reactivate_event_queue = is_queueing_events;
    bool reactivate_event_processing = is_processing_events;

    // Aight, geddafuckouddahea
    DisableEventQueue();
    EndProcessing();

    _active_queue.clear();
    _safe_queue.clear();

    // Aight, geddafuckbackovaheaifyouwasdoinshi
    if(reactivate_event_queue)      EnableEventQueue();
    if(reactivate_event_processing) try_BeginProcessing();
}

void EventQueue::StartRecordingDemo()
{ do_demo_recording = true; }

#define AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES INT_MAX

bool EventQueue::StopRecordingDemo()
{
    do_demo_recording = false;
    while(std::filesystem::is_regular_file(std::filesystem::path(DEMO_FILENAME)))
    {
        if(demo_recording_number == AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES)
        {
            PRINT_ERROR("EventQueue::StopRecordingDemo - you either have an unreasonable amount of demo files, or something else went wrong")
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

void EventQueue::RecordEventToDemo(const Event& event)
{ demo_recording_storage += "Creation Time: [" + std::to_string(event.GetCreationTime()) + "] Command: <" + event.GetCommand() + ">\n"; }
