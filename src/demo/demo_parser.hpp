#ifndef DEMO_PARSER_H
#define DEMO_PARSER_H

#include "input/event.hpp"
#include <list>

class DemoParser
{
public:
    static bool LoadDemoFromMemory(const std::string& DemoFile);
    static bool LoadDemoFromFile(const std::string& DemoFilePath);
    static std::list<Event> GetEventQueue();

private:
    static std::list<Event> _event_queue;
    static bool is_demo_file_parsed;
    static bool is_demo_file_loaded;

    static bool ParseLine(const std::string& Line);
};

#endif // DEMO_PARSER_H
