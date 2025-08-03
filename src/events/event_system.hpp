#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include <vector>

class EventSystem
{
public:
    ~EventSystem();

    static void LoadQueue(const std::vector<Event>& InitFromQueue);
    // TODO: idk if both EventSystem and DemoParser having the same 'LoadDemo' functions is a great idea...
    static bool LoadDemoFromFile(const std::string& DemoFilePath);
    static bool LoadDemoFromMemory(const std::string& DemoFile);

    static const std::vector<Event>& GetQueue();
    static void AddEvent(const Event& NewEvent);
    static Event GetNextEvent(); // This will also remove that Event from the queue!
    static unsigned int GetQueueSize();
    static void ClearQueue();
    static void DebugPrintQueueLog();
    static void StartRecordingDemo();
    static bool StopRecordingDemo();

private:
    static std::vector<Event> _queue;
    static bool do_demo_recording;
    static std::string demo_recording_storage;
    static int demo_recording_number;
    static std::string demo_recording_name;
    static std::string demo_recording_extension;

    static void RecordEventToDemo(const Event& Event);
};

extern EventSystem* global_EventQueue;

#endif // EVENT_QUEUE_H
