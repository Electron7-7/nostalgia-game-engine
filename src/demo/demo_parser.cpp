#include "demo_parser.hpp"
#include "debug.hpp"
#include "printing.hpp"
#include "filesystem/filesystem.hpp"
#include "string_to_num.hpp"

#include <sstream>
#include <fstream>

bool DemoParser::LoadDemoFromFile(const std::string& demo_file_path)
{
    if(!Filesystem::IsFile(demo_file_path))
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
                { at_least_one_line_was_parsed = true; }
            else
                { PRINT_WARNING("DemoParser::LoadDemoFromFile - a line failed to parse: '{}'", line) }
        }
        demo_file.close();
        return at_least_one_line_was_parsed;
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
            { at_least_one_line_was_parsed = true; }
        else
            { PRINT_WARNING("A line failed to parse - \"{}\"", line) }
    }
    return at_least_one_line_was_parsed;
}

std::vector<InputEvent> DemoParser::GetEventQueue()
{ return m_EventQueue; }

bool DemoParser::ParseLine(const std::string& line)
{
    std::string l_Tick;
    std::string l_CreationTime;
    bool l_ParsedFirstParens = false;
    std::string l_BindingID;
    std::string l_BindingStatus;
    std::string l_BindingJustPressed;
    std::vector<std::string> l_Actions;
    bool l_IsMouseMotion = false;
    std::string l_CurrentMouse;
    std::string l_LastMouse;

    for(size_t i = 0 ; i < line.length() ; ++i)
    {
        char character = line.at(i);
        if(character == '[')
        {
            character = line.at(++i);
            while(character != ']')
            {
                while(character != ':')
                {
                    if(character != ' ')
                        { l_Tick.append(1, character); }
                    character = line.at(++i);
                }
                character = line.at(++i);
                while(character != ']')
                {
                    if(character != ' ')
                        { l_CreationTime.append(1, character); }
                    character = line.at(++i);
                }
            }
            continue;
        }


        if(character == '(')
        {
            character = line.at(++i);
            if(!l_ParsedFirstParens)
            {
                l_ParsedFirstParens = true;
                while(character != ':')
                {
                    if(character != ' ')
                        { l_BindingID.append(1, character); }
                    character = line.at(++i);
                }
                character = line.at(++i);
                while(character != ':')
                {
                    if(character != ' ')
                        { l_BindingStatus.append(1, character); }
                    character = line.at(++i);
                }
                character = line.at(++i);
                while(character != ')')
                {
                    if(character != ' ')
                        { l_BindingJustPressed.append(1, character); }
                    character = line.at(++i);
                }
            }
            else if(l_ParsedFirstParens && !l_IsMouseMotion)
            {
                l_IsMouseMotion = true;
                while(character != ')')
                {
                    if(character != ' ')
                        { l_CurrentMouse.append(1, character); }
                    character = line.at(++i);
                }
            }
            else if(l_ParsedFirstParens && l_IsMouseMotion)
            {
                while(character != ')')
                {
                    if(character != ' ')
                        { l_LastMouse.append(1, character); }
                    character = line.at(++i);
                }
            }
            continue;
        }

        if(character == '<')
        {
            character = line.at(++i);
            while(character != '>')
            {
                std::string l_Action = "";
                while((character != ',' || character != '>') && i < line.size() - 1)
                {
                    if(character != ' ')
                        { l_Action.append(1, character); }
                    character = line.at(++i);
                }
                l_Actions.push_back(l_Action);
            }
            continue;
        }
    }

    PRINT_DEBUG("l_Tick: '{}', l_CreationTime: '{}'", l_Tick, l_CreationTime)
    PRINT_DEBUG("l_BindingID: '{}', l_BindingStatus: '{}', l_BindingJustPressed: '{}', l_CurrentMouse: '{}', l_LastMouse: '{}',", l_BindingID, l_BindingStatus, l_BindingJustPressed, l_CurrentMouse, l_LastMouse)

    InputEvent new_event;
    unsigned int l_Status;
    if(!StringToNum(new_event.m_Tick, l_Tick)) { return false; }
    if(!StringToNum(new_event.m_Time, l_CreationTime)) { return false; }
    if(!StringToNum(new_event.m_Binding.m_ID, l_BindingID)) { return false; }
    if(!StringToNum(l_Status, l_BindingStatus)) { return false; }
    if(!StringToNum(new_event.m_Binding.JustChanged, l_BindingJustPressed)) { return false; }
    if(!StringToNum(new_event.m_CurrentMousePosition, l_CurrentMouse)) { return false; }
    if(!StringToNum(new_event.m_LastMousePosition, l_LastMouse)) { return false; }
    new_event.m_Binding.Status = static_cast<InputStatus>(l_Status);
    m_EventQueue.push_back(new_event);
    return true;
}

