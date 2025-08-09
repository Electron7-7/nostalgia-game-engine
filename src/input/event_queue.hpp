#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "common/safe_return.hpp"
#include "input/key.hpp"

#include <vector>
#include <set>

class EventQueue
{
public:
    ~EventQueue();

    static void LoadQueue(const std::vector<Event>& InitFromQueue);
    // TODO: idk if both EventQueue and DemoParser having the same 'LoadDemo' functions is a great idea...
    static bool LoadDemoFromFile(const std::string& DemoFilePath);
    static bool LoadDemoFromMemory(const std::string& DemoFile);

    static bool EnableEventQueue();
    static bool DisableEventQueue();

    static SafeStatus try_BeginProcessing();
    static bool EndProcessing();

    static bool try_AddBinding(const char* KeyName, const char* CommandName);
    static bool try_RemoveBinding(const char* KeyName, const char* CommandName);
    static bool try_ClearBindings(const char* KeyName);
    static void ClearAllBindings();

    static SafeStatus try_QueueEvents(const Key& Key, bool IsReleased);
    static SafeReturn<Event> GetNextEvent(); // This will also remove that Event from the queue!
    static unsigned int GetCurrentQueueSize();
    static void ClearQueue();
    static void PanicClearQueue();

    static void DebugPrintQueueLog();
    static void StartRecordingDemo();
    static bool StopRecordingDemo();

private:
    static std::vector<Event> _active_queue;
    static std::vector<Event> _safe_queue;
    static unsigned int _last_processed_event_index;

    static bool can_queue_events;
    static bool is_processing_events;
    static bool is_queueing_events;

    static bool do_demo_recording;
    static int demo_recording_number;
    static std::string demo_recording_name;
    static std::string demo_recording_extension;
    static std::string demo_recording_storage;

    static std::map<Key, std::set<KeyBind>> keybinds_map;

    static Key GetKey(const char* KeyName);
    static void RecordEventToDemo(const Event& Event);
};

extern EventQueue* global_EventSystem;

#endif // EVENT_QUEUE_H
