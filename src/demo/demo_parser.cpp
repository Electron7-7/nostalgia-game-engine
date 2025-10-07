#include "demo_parser.hpp"
#include "printing.hpp"
#include "truncate_string.hpp"
#include "string_to_num.hpp"
#include "filesystem/filesystem.hpp"
#include "managers/input_manager.hpp"

#include <sstream>

DemoParser::DemoParser(const std::string& path, Demo& output)
{
    if(FileSystem::IsFile(path))
        { m_DemoSuccessfullyLoaded = LoadDemoFromFile(path, output); }
    else
        { m_DemoSuccessfullyLoaded = LoadDemoFromMemory(path, output); }
}

bool DemoParser::LoadDemoFromFile(const std::string& path, Demo& output)
{
    std::string file_data("");
    if(!FileSystem::try_ReadFileToString(path, file_data))
    {
        PRINT_WARNING("DemoParser::LoadDemoFromFile - Failed to load demo file '{}'\n", gTruncateString(path));
        return false;
    }
    return LoadDemoFromMemory(file_data, output);
}

bool DemoParser::LoadDemoFromMemory(const std::string& data, Demo& output)
{
    std::istringstream demo_data(data);
    std::vector<InputEvent> event_queue;
    bool l_AtLeastOneLineParsed = false;
    std::string line;
    while(std::getline(demo_data, line))
    {
        if(ParseLine(line, event_queue, output))
            { l_AtLeastOneLineParsed = true; }
        else
            { PRINT_WARNING("DemoParser::ParseLine - Failed to parse this line: '{}'", line) }
    }
    if(!l_AtLeastOneLineParsed)
        { PRINT_WARNING("DemoParser::LoadDemoFromMemory - Failed to parse demo file data") }
    return l_AtLeastOneLineParsed;
}

static void sAdvanceCharacter(char& character, size_t& iterator, const std::string& line)
{
    if(++iterator >= line.size())
        { iterator = 0; }
    character = line.at(iterator);
}

bool DemoParser::ParseLine(const std::string& line, std::vector<InputEvent>& queue, Demo& demo)
{
    if(line.starts_with('<'))
    {
        demo.push_back(queue);
        queue.clear();
        return true;
    }

    std::string id_buffer;
    std::string status_buffer;
    std::string just_changed_buffer;
    std::string curr_mouse_buffer;
    std::string last_mouse_buffer;
    bool is_mouse_motion = false;

    for(size_t i = 0 ; i < line.length() ; ++i)
    {
        char character = line.at(i);
        if(character == '[')
        {
            sAdvanceCharacter(character, i, line);
            while(character != ']')
            {
                while(character != ':')
                {
                    id_buffer.append(1, character);
                    sAdvanceCharacter(character, i, line);
                }
                sAdvanceCharacter(character, i, line);
                while(character != ':')
                {
                    status_buffer.append(1, character);
                    sAdvanceCharacter(character, i, line);
                }
                sAdvanceCharacter(character, i, line);
                while(character != ']')
                {
                    just_changed_buffer.append(1, character);
                    sAdvanceCharacter(character, i, line);
                }
            }
            continue;
        }
        if(character == '(')
        {
            sAdvanceCharacter(character, i, line);
            while(character != ')')
            {
                if(!is_mouse_motion)
                    { curr_mouse_buffer.append(1, character); }
                else
                    { last_mouse_buffer.append(1, character); }
                sAdvanceCharacter(character, i, line);
            }
            is_mouse_motion = (curr_mouse_buffer.size() > 1);
        }
    }

    if(!is_mouse_motion)
    {
        curr_mouse_buffer.clear();
        last_mouse_buffer.clear();
    }

    id_t id;
    int status;
    int just_changed;
    glm::vec2 curr_mouse{0.0f, 0.0f};
    glm::vec2 last_mouse{0.0f, 0.0f};

    if(!StringToNum(id, id_buffer) ||
        !StringToNum(status, status_buffer) ||
        !StringToNum(just_changed, just_changed_buffer))
    { return false; }
    else if(is_mouse_motion &&
        (!StringToNum(curr_mouse, curr_mouse_buffer) ||
            !StringToNum(last_mouse, last_mouse_buffer)))
    { return false; }

    InputEvent new_event;
    new_event.mBinding = InputManager::m_sGetBinding(id);
    new_event.mBinding.status = static_cast<InputStatus>(status);
    new_event.mBinding.just_changed = static_cast<bool>(just_changed);
    new_event.mActions = InputManager::GetActions(id);
    new_event.mCurMousePos = curr_mouse;
    new_event.mLastMousePos = last_mouse;
    queue.push_back(new_event);
    return true;
}
