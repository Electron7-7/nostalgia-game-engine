#include "demo_parser.hpp"
#include "printing.hpp"
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>

bool DemoParser::LoadDemoFromFile(const std::string& demo_file_path)
{
    if(!std::filesystem::is_regular_file(demo_file_path))
    {
        PRINT_ERROR("DemoParser::LoadDemoFromFile - demo doesn't exist at path: '{}'", demo_file_path)
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
                PRINT_WARNING("DemoParser::LoadDemoFromFile - a line failed to parse: '{}'", line)
        }

        demo_file.close();
        return true;
    }

    PRINT_ERROR("DemoParser::LoadDemoFromFile - unable to open file at path: '{}'", demo_file_path)
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
            PRINT_WARNING("A line failed to parse - \"{}\"", line)
    }

    return at_least_one_line_was_parsed;
}

std::vector<Event> DemoParser::GetEventQueue()
{ return _event_queue; }

bool DemoParser::ParseLine(const std::string& line)
{
    std::string creation_time_buffer;
    std::string command_buffer;

    for(int i = 0 ; i < line.length() ; i++)
    {
        if(line.at(i) == '[')
        {
            while(line.at(i) != ']')
            {
                creation_time_buffer.append(1, line.at(i));
                i++;
            }
        }

        if(line.at(i) == '<')
        {
            while(line.at(i) != '>')
            {
                command_buffer.append(1, line.at(i));
                i++;
            }
        }
    }

    double creation_time;
    try
    {
        creation_time = std::stod(creation_time_buffer);
    }
    catch(std::invalid_argument const& exception)
    {
        // TODO: Add printouts for this exception.
        creation_time = 0.0;
    }

    _event_queue.insert(_event_queue.end(), Event(ConsoleCommand(command_buffer.c_str()), creation_time));

    return true;
}
