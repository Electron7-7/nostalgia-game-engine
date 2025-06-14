#include "demo_parser.hpp"
#include "common/debugging.hpp"
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>

bool DemoParser::LoadDemoFromFile(const std::string& demo_file_path)
{
    if(!std::filesystem::is_regular_file(demo_file_path))
    {
        PRINTERR("DemoParser::LoadDemoFromFile - demo doesn't exist at path: " + demo_file_path);
        return false;
    }

    std::ifstream demo_file(demo_file_path);
    std::string line;
    bool at_least_one_line_was_parsed = false;
    if(demo_file.is_open())
    {
        while(std::getline(demo_file,line))
        {
            if(ParseLine(line))
                at_least_one_line_was_parsed = true;
            else
                PRINTWARN("A line failed to parse!");
        }

        demo_file.close();
        return true;
    }

    PRINTERR("DemoParser::LoadDemoFromFile - unable to open file at path: " + demo_file_path);
    return false;
}

bool DemoParser::LoadDemoFromMemory(const std::string& demo_file_data)
{
    std::istringstream demo_data(demo_file_data);
    std::string line;
    bool at_least_one_line_was_parsed = false;
    while(std::getline(demo_data, line))
    {
        if(ParseLine(line))
            at_least_one_line_was_parsed = true;
        else
            PRINTWARN("A line failed to parse!");
    }

    return at_least_one_line_was_parsed;
}

std::vector<Event> DemoParser::GetEventQueue()
{ return _event_queue; }

bool DemoParser::ParseLine(const std::string& line)
{
    std::string buffer;

    for(int i = 0 ; i < line.length() ; i++)
    {
        if(line.at(i) == '[')
    }


    return false; // TEMP: return false until this function is finished
}
