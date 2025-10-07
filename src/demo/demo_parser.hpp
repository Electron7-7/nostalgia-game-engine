#ifndef DEMO_PARSER_H
#define DEMO_PARSER_H

#include "demo.hpp"

class DemoParser
{
public:
    DemoParser(const std::string& DemoFile, Demo& Output);

    static bool LoadDemoFromMemory(const std::string& DemoFile, Demo& OutputQueue);
    static bool LoadDemoFromFile(const std::string& DemoFilePath, Demo& OutputQueue);

    constexpr operator const bool&() const
    { return m_DemoSuccessfullyLoaded; }

private:
    bool m_DemoSuccessfullyLoaded;
    static bool ParseLine(const std::string&, std::vector<InputEvent>&, Demo&);
};

#endif // DEMO_PARSER_H
