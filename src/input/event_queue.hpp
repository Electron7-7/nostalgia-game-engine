#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "key.hpp"
#include "event.hpp"
#include "safe_return.hpp"

#include <vector>

class EventQueue
{
public:
    static void LoadQueue(const std::vector<Event>& InitFromQueue);
    static bool LoadDemoFromFile(const std::string& DemoFilePath);
    static bool LoadDemoFromMemory(const std::string& DemoFile);

    static bool EnableEventQueue();
    static bool DisableEventQueue();

    static SafeStatus try_BeginProcessing();
    static bool EndProcessing();

    static SafeStatus try_QueueEvents(KeyID Key, bool IsReleased = false);
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
    static bool is_copying_queue;

    static bool do_demo_recording;
    static int demo_recording_number;
    static std::string demo_recording_name;
    static std::string demo_recording_extension;
    static std::string demo_recording_storage;

    static void PushFront(Event event);
    static bool PopFront();

    static void RecordEventToDemo(const Event& Event);
};

#endif // EVENT_QUEUE_H
