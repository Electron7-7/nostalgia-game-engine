#include "event_queue.hpp"
#include "demo_parser.hpp"
#include "printing.hpp"

#include <climits>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>

#define DEMO_FILENAME (demo_recording_name + std::to_string(demo_recording_number) + demo_recording_extension)

// TODO: Is this a good idea? Seems find to me...
#define ASSERT_KEYNAME(KEY_NAME)  \
Key KEYID = GetKey(KEY_NAME); \
if(KEYID == Key::INVALID)         \
    return false;

EventQueue singleton_EventSystem;
EventQueue* global_EventSystem = &singleton_EventSystem;

std::vector<Event> EventQueue::_active_queue = {};
std::vector<Event> EventQueue::_safe_queue = {};
unsigned int EventQueue::_last_processed_event_index = 0;

bool EventQueue::can_queue_events = false;
bool EventQueue::is_processing_events = false;
bool EventQueue::is_queueing_events = false;

bool EventQueue::do_demo_recording = false;
std::string EventQueue::demo_recording_storage = "";
std::string EventQueue::demo_recording_name = "demo";
int EventQueue::demo_recording_number = 1;
std::string EventQueue::demo_recording_extension = ".demo";

std::map<Key, std::set<KeyBind>> EventQueue::keybinds_map = {};

void EventQueue::DebugPrintQueueLog()
{
    size_t active_queue_size = _active_queue.size();
    std::string queue_printout = "";

    for(size_t i = 0 ; i < active_queue_size ; i++)
        queue_printout += _active_queue.at(i).EventLog() + "\n";

    std::print("{0}-- Start of Queue Log --{1}\n{2}\n{0}--- End of Queue Log ---{1}", COLOR_BOLD(Color::Blue), Color::Reset, queue_printout);
}

EventQueue::~EventQueue()
{
    if(do_demo_recording)
        StopRecordingDemo();
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
        return false;

    ClearQueue();

    _active_queue = demo_parser.GetEventQueue();
    return true;
}

bool EventQueue::LoadDemoFromMemory(const std::string &demo_file)
{
    DemoParser demo_parser;

    if(!demo_parser.LoadDemoFromMemory(demo_file))
        return false;

    ClearQueue();

    _active_queue = demo_parser.GetEventQueue();
    return true;
}

bool EventQueue::EnableEventQueue()
{
    if(can_queue_events)
        return false;

    can_queue_events = true;
    return true;
}

bool EventQueue::DisableEventQueue()
{
    if(!can_queue_events)
        return false;

    can_queue_events = false;
    return true;
}

SafeStatus EventQueue::try_BeginProcessing()
{
    if(is_processing_events)
        return Status::ERROR_ALREADY_ACTIVE;

    if(_active_queue.empty() || (_last_processed_event_index + 1) >= _active_queue.size())
        return Status::EventQueueQUEUE_EMPTY; // FIXME: Return status codes instead

    WAIT_FOR(is_queueing_events, 0.5); // *stares at '_active_queue' reeeeeally hard*

    _safe_queue.clear(); // Just to be, well, safe
    _safe_queue.insert(_safe_queue.end(), (_active_queue.begin() + _last_processed_event_index), _active_queue.end());
    _last_processed_event_index += (_safe_queue.size() - 1);

    is_processing_events = true;
    return Status::NO_ERROR;
}

bool EventQueue::EndProcessing()
{
    if(!is_processing_events)
        return false;

    is_processing_events = false;
    return true;
}

bool EventQueue::try_AddBinding(const char* key_name, const char* command_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID))
        keybinds_map[KEYID] = {};

    keybinds_map.at(KEYID).insert(keybinds_map.at(KEYID).end(), KeyBind(command_name));
    return true;
}

bool EventQueue::try_RemoveBinding(const char* key_name, const char* command_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID) || !keybinds_map.at(KEYID).contains(command_name))
        return true; // TODO: Should this return false/an int status instead?

    std::set<KeyBind>& key_binds = keybinds_map.at(KEYID);

    key_binds.erase(command_name);

    if(key_binds.size() == 0) keybinds_map.erase(KEYID);

    return true;
}

bool EventQueue::try_ClearBindings(const char* key_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID))
        return true; // TODO: Should this return false/an int status instead?

    keybinds_map.erase(KEYID);
    return false;
}

void EventQueue::ClearAllBindings()
{
    keybinds_map.clear();
    // TODO: Remove this binding later; it's for the prototype/debug program
    keybinds_map[Key::ESC] = { CommandLine::cmd_HardQuitProgram };
}

Key EventQueue::GetKey(const char* key_name)
{
    if(!key_strings_map.contains(key_name))
        return Key::INVALID;

    return key_strings_map.at(key_name);
}

SafeStatus EventQueue::try_QueueEvents(const Key& key_id, bool is_released)
{
    if(!can_queue_events)
        return Status::EventQueueNOT_ENABLED;

    if(!keybinds_map.contains(key_id))
        return Status::EventQueueINVALID_KEY_ID;

    is_queueing_events = true;

    for(const KeyBind& keybind : keybinds_map.at(key_id))
        if(keybind.when_key_released == is_released)
            _active_queue.insert(_active_queue.end(), keybind.command_name);

    is_queueing_events = false;

    return Status::NO_ERROR;
}

SafeReturn<Event> EventQueue::GetNextEvent()
{
    if(!is_processing_events)
        return SafeReturn(Event(), Status::EventQueueNOT_PROCESSING_EVENTS);

    Event next_event(_safe_queue.back());

    if(do_demo_recording)
        RecordEventToDemo(next_event);
    _safe_queue.pop_back();

    return SafeReturn(next_event);
}

unsigned int EventQueue::GetCurrentQueueSize()
{ return _safe_queue.size(); }

void EventQueue::ClearQueue()
{
    WAIT_FOR(is_queueing_events, 1.0) // _active_queue has one second before we flip the fuck out and kill it with hammers
    _active_queue.clear();

    WAIT_FOR(is_processing_events, 5.0) // _safe_queue gets five whole seconds because it's earned more love
    _safe_queue.clear();
}

void EventQueue::PanicClearQueue()
{
    PRINTDEBUG("EventQueue::PanicClearQueue called. Printing the queue log...\n")
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
            PRINTERROR("EventQueue::StopRecordingDemo - you either have an unreasonable amount of demo files, or something else went wrong")
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

