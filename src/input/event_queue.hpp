#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "safe_return.hpp"

#include <vector>
#include <glm/fwd.hpp>

class EventQueue
{
public:
    static void LoadQueue(const std::vector<InputEvent>& InitFromQueue);
    static bool LoadDemoFromFile(const std::string& DemoFilePath);
    static bool LoadDemoFromMemory(const std::string& DemoFile);
    static void SetDemoFileName(const std::string& DemoFileName);

    static bool EnableEventQueue();
    static bool DisableEventQueue();

    static SafeStatus try_BeginProcessing();
    static bool EndProcessing();

    static SafeStatus QueueInputEvent(const InputEvent&);
    static SafeReturn<InputEvent> GetNextEvent();
    static unsigned int GetCurrentQueueSize();
    static void ClearQueue();
    static void PanicClearQueue();

    static void DebugPrintQueueLog();
    static void StartRecordingDemo();
    static bool StopRecordingDemo();

private:
    static std::vector<InputEvent> m_sActiveQueue;
    static std::vector<InputEvent> m_sSafeQueue;
    static unsigned int _last_processed_event_index;

    static bool can_queue_events;
    static bool is_processing_events;
    static bool is_queueing_events;
    static bool is_copying_queue;

    static bool do_demo_recording;
    static int demo_recording_number;
    static std::string demo_recording_name;
    static std::string demo_recording_extension;
    static std::vector<std::string> demo_recording_storage;

    static void PushFront(const InputEvent& event);
    static bool PopFront();

    static void RecordEventToDemo(const InputEvent& Event);
};

#endif // EVENT_QUEUE_H
