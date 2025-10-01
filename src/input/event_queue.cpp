#include "event_queue.hpp"
#include "debug.hpp"
#include "colors.hpp"
#include "filesystem/filesystem.hpp"
#include "demo/demo_parser.hpp"

#include <format>
#include <climits>
#include <fstream>
#include <glm/vec2.hpp>

#define DEMO_FILENAME std::format("{}-{}.{}", demo_recording_name, demo_recording_number, demo_recording_extension)

std::vector<InputEvent> EventQueue::m_sActiveQueue = {};
std::vector<InputEvent> EventQueue::m_sSafeQueue   = {};
unsigned int EventQueue::_last_processed_event_index = 0;

bool EventQueue::can_queue_events = false;
bool EventQueue::is_processing_events = false;
bool EventQueue::is_queueing_events = false;
bool EventQueue::is_copying_queue = false;
bool EventQueue::do_demo_recording = false;

std::vector<std::string> EventQueue::demo_recording_storage = {};
std::string EventQueue::demo_recording_name = "demo";
int EventQueue::demo_recording_number = 1;
std::string EventQueue::demo_recording_extension = "dem";

void EventQueue::DebugPrintQueueLog()
{
    std::print("{}-- Start of Queue Log --{}\n", Style::Bold + Foreground::Blue, Style::Reset);
    for(auto& event : m_sActiveQueue)
        { std::print("{}\n", event.Log()); }
    std::print("{}--- End of Queue Log ---{}\n", Style::Bold + Foreground::Blue, Style::Reset);
}

void EventQueue::LoadQueue(const std::vector<InputEvent>& event_queue)
{
    ClearQueue();
    m_sActiveQueue = event_queue;
}

bool EventQueue::LoadDemoFromFile(const std::string& demo_file_path)
{
    DemoParser demo_parser;
    if(!demo_parser.LoadDemoFromFile(demo_file_path))
        { return false; }
    ClearQueue();
    m_sActiveQueue = demo_parser.GetEventQueue();
    DEBUG(DebugPrintQueueLog();)
    return true;
}

bool EventQueue::LoadDemoFromMemory(const std::string& demo_file)
{
    DemoParser demo_parser;
    if(!demo_parser.LoadDemoFromMemory(demo_file))
        { return false; }
    ClearQueue();
    m_sActiveQueue = demo_parser.GetEventQueue();
    DEBUG(DebugPrintQueueLog();)
    return true;
}

void EventQueue::SetDemoFileName(const std::string& name)
{ Filesystem::MakePathAbsolute(name, demo_recording_name); }

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
    if(m_sActiveQueue.empty() || _last_processed_event_index >= m_sActiveQueue.size())
        { return Status::EventQueueEMPTY; }

    m_sSafeQueue.clear(); // Just to be, well, safe

    std::vector<InputEvent>::iterator queue_begin = m_sActiveQueue.begin();
    std::advance(queue_begin, _last_processed_event_index);

    m_sSafeQueue.insert(m_sSafeQueue.cend(), queue_begin, m_sActiveQueue.end());
    _last_processed_event_index += m_sSafeQueue.size(); // This variable is 1 index ahead, because we don't want to process the same event twice

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

SafeStatus EventQueue::QueueInputEvent(const InputEvent& event)
{
    if(!can_queue_events)
        { return Status::EventQueueNOT_ENABLED; }
    else if(!event.Valid())
        { return Status::EventQueueINVALID_EVENT; }
    is_queueing_events = true;
    m_sActiveQueue.push_back(event);
    if(do_demo_recording)
        { RecordEventToDemo(event); }
    is_queueing_events = false;
    return Status::NO_ERR;
}

SafeReturn<InputEvent> EventQueue::GetNextEvent()
{
    if(!is_processing_events)
        { return SafeReturn(InputEvent(), Status::EventQueueNOT_PROCESSING_EVENTS); }
    else if(m_sSafeQueue.empty())
        { return SafeReturn(InputEvent(), Status::EventQueueEMPTY); }

    InputEvent next_event(m_sSafeQueue.front());
    m_sSafeQueue.erase(m_sSafeQueue.cbegin());

    return SafeReturn(next_event);
}

unsigned int EventQueue::GetCurrentQueueSize()
{ return m_sSafeQueue.size(); }

void EventQueue::ClearQueue()
{
    m_sActiveQueue.clear();
    m_sSafeQueue.clear();
}

void EventQueue::PanicClearQueue()
{
    PRINT_DEBUG("EventQueue::PanicClearQueue called. Printing the queue log...\n")
    DebugPrintQueueLog();

    bool reactivate_event_queue = is_queueing_events;
    bool reactivate_event_processing = is_processing_events;

    DisableEventQueue();
    EndProcessing();

    m_sActiveQueue.clear();
    m_sSafeQueue.clear();

    if(reactivate_event_queue)
        { EnableEventQueue(); }
    if(reactivate_event_processing)
        { try_BeginProcessing(); }
}

void EventQueue::StartRecordingDemo()
{ do_demo_recording = true; }

#define AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES INT_MAX

bool EventQueue::StopRecordingDemo()
{
    do_demo_recording = false;
    while(Filesystem::IsFile(DEMO_FILENAME))
    {
        if(demo_recording_number == AN_UNREASONABLE_AMOUNT_OF_DEMO_FILES)
        {
            PRINT_ERROR("EventQueue::StopRecordingDemo - you either have an unreasonable amount of demo files, or something else went wrong")
            PRINT_DEBUG("The demo file that attempted to save:\n{}\n", demo_recording_storage)
            return false;
        }
        ++demo_recording_number;
    }

    if(!demo_recording_storage.empty() && !demo_recording_storage.at(0).empty())
    {
        std::ofstream demo_file(DEMO_FILENAME);
        for(const auto& event : demo_recording_storage)
            { demo_file << event; }
        demo_file.close();
    }
    demo_recording_storage.clear();
    return true;
}

void EventQueue::RecordEventToDemo(const InputEvent& event)
{
    std::string actions = "";
    std::string extras = "";
    size_t i = 0;
    size_t size = event.Actions().size();
    for(const auto& action : event.Actions())
    {
        actions += action;
        if(i++ < (size - 1))
            { actions += ","; }
    }
    if(!actions.empty())
        { extras += std::format(" <{}>", actions); }
    if(event.IsMouseMotion())
        { extras += std::format(" ({:0.3f},{:0.3f}) ({:0.3f},{:0.3f})", event.CurrentMousePosition().x, event.CurrentMousePosition().y, event.LastMousePosition().x, event.LastMousePosition().y); }
    demo_recording_storage.push_back(std::format("[{} : {:0.3f}] ({} : {} : {}){}\n", event.TickNumber(), event.CreationTime(), static_cast<id_t>(event.Binding()), static_cast<int>(event.Binding().Status), static_cast<int>(event.Binding().JustChanged), extras));
}
