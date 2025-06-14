#ifndef DEMO_PARSER_H
#define DEMO_PARSER_H

#include "event.hpp"
#include <vector>

class DemoParser
{
public:
    bool LoadDemoFromMemory(const std::string& DemoFile);
    bool LoadDemoFromFile(const std::string& DemoFilePath);
    std::vector<Event> GetEventQueue();

private:
    std::vector<Event> _event_queue = {};
    bool is_demo_file_parsed = false;
    bool is_demo_file_loaded = false;

    bool ParseLine(const std::string& Line);
};

#endif // DEMO_PARSER_H
