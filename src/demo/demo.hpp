#ifndef DEMO_H
#define DEMO_H

#include "input/event_queue.hpp"
#include "theatre_parser/theatre_data.hpp"

#include <vector>

struct Demo
{
public:
    Demo();
    Demo(const TheatreData& TheatreData);
    Demo(const TheatreData& TheatreData, const std::string& BaseName);
    Demo(const TheatreData& TheatreData, const std::vector<EventQueue>& Queues);
    Demo(const TheatreData& TheatreData, const std::vector<EventQueue>& Queues, const std::string& BaseName);

    bool Play();
    bool WriteToFile();
    bool GetNextQueue(EventQueue&);

    const std::string& name() const;
    void push_back(const EventQueue& Queue);
    void push_back(const std::vector<InputEvent>& Queue);
    void clear();

private:
    std::vector<EventQueue> mQueues = {};
    TheatreData mTheatreData;
    std::string mName{""};
    bool mIsPlaying = false;
};

#endif // DEMO_H
