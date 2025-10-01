#ifndef DEMO_PARSER_H
#define DEMO_PARSER_H

#include "input/event.hpp"

#include <vector>

class DemoParser
{
public:
    bool LoadDemoFromMemory(const std::string& DemoFile);
    bool LoadDemoFromFile(const std::string& DemoFilePath);
    std::vector<InputEvent> GetEventQueue();

private:
    std::vector<InputEvent> m_EventQueue = {};
    bool ParseLine(const std::string& Line);
};

#endif // DEMO_PARSER_H
