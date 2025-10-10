#include "demo_controller.hpp"
#include "input/event_queue.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/input_manager.hpp"
#include "filesystem/filesystem.hpp"
#include "theatre_parser/theatre_parser.hpp"
#include "printing.hpp"
#include "string_to_num.hpp"
#include "truncate_string.hpp"

#include <sstream>

static std::string sExtension{".gen1_demo"};
static std::vector<EventQueue> sDemo{};

static DemoController sDemoController;
DemoController* g_pDemoController = &sDemoController;

bool DemoController::Record()
{
    if(is_recording_)
        { return false; }
    else if(is_playing_)
    {
        print_error("DemoController::Record - Cannot record a gen1 demo while a gen1 demo is being played");
        return false;
    }
    else if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
    {
        print_error("DemoController::Record - You can't start recording a gen1 demo inside a Theatre");
        return false;
    }
    sDemo.clear();
    return is_recording_ = true;
}

bool DemoController::StopRecording()
{
    recorded_unsaved_demo_ = is_recording_;
    is_recording_ = false;
    return recorded_unsaved_demo_;
}

bool DemoController::RecordingDemo() const
{ return is_recording_; }

bool DemoController::Save(const std::string& path)
{
    if(!recorded_unsaved_demo_ || sDemo.empty())
        { return false; }
    std::string theatre_path{g_pTheatreManager->GetInitialState().file_path};
    if(theatre_path.empty())
    {
        auto status = TheatreParser::WriteTheatre(g_pTheatreManager->GetCurrentState(), FileSystem::GetDir(path));
        if(status.Status() != Status::NO_ERR)
            { print_warning("DemoController::Save - unable to save Theatre file for gen1 demo"); return false; }
        theatre_path = status.Data();
    }
    std::string base_name = FileSystem::GetAbsolute(path);
    std::string name;
    int iterator = 0;
    while(true)
    {
        name = base_name;
        if(iterator != 0)
            { name += std::format("_{}", iterator); }
        name += sExtension;
        if(!FileSystem::Exists(name))
            { break; }
        ++iterator;
    }

    InputEvent temp_event;
    std::string output{std::format("!Nostalgia Gen1 Demo\n{{{}}}", theatre_path)};
    size_t i = -1;
    for(EventQueue& queue : sDemo)
    {
        output += std::format("\n[{}]", ++i);
        while(queue.GetNextEvent(temp_event))
            { output += std::format("\n\t{}", temp_event.DemoString()); }
    }
    return FileSystem::try_WriteFileFromString(name, output);
}

bool DemoController::Play(const std::string& path)
{
    if(is_playing_)
        { return false; }
    else if(is_recording_)
    {
        print_error("DemoController::Play - cannot play a gen1 demo while a gen1 demo is being recorded");
        return false;
    }
    sDemo.clear();
    if(!ParseDemo(path, sDemo))
        { return false; }
    return is_playing_ = true;
}

bool DemoController::StopPlaying()
{
    if(!is_playing_)
        { return false; }
    return !(is_playing_ = false);
}

bool DemoController::PlayingDemo() const
{ return is_playing_; }

void DemoController::ProcessQueue(EventQueue& queue)
{
    if(is_playing_)
    {
        if(sDemo.empty())
            { StopPlaying(); return; }
        queue = sDemo.back();
        sDemo.pop_back();
    }
    else if(is_recording_)
        { sDemo.push_back(queue); }
}

bool DemoController::ParseDemo(const std::string& path, std::vector<EventQueue>& output)
{
    std::string file_data("");
    if(!FileSystem::try_ReadFileToString(path, file_data))
    {
        if(!FileSystem::try_ReadFileToString(FileSystem::ReplaceExtension(sExtension, path), file_data))
        {
            print_error("DemoController::LoadDemoFromFile - failed to load gen1 demo '{}'\n", gTruncateString(path));;
            return false;
        }
    }
    std::vector<InputEvent> event_queue;
    bool l_AtLeastOneLineParsed = false;
    std::stringstream demo_data{file_data};
    std::string line;
    while(std::getline(demo_data, line))
    {
        SafeStatus status = ParseLine(line, event_queue, output);
        if(status == Status::DemoControllerLINE_PARSED)
            { l_AtLeastOneLineParsed = true; }
        else if(status == Status::DemoControllerLINE_FAILED)
            { print_warning("DemoController::ParseLine - gen1 demo failed to parse this line: '{}'", line); }
    }
    if(!l_AtLeastOneLineParsed)
        { print_warning("DemoController::LoadDemoFromMemory - no inputs recorded or otherwise unable to parse gen1 demo file"); }
    return l_AtLeastOneLineParsed;
}

static void sAdvanceCharacter(char& character, size_t& iterator, const std::string& line)
{
    if(++iterator >= line.size())
        { iterator = 0; }
    character = line.at(iterator);
}

SafeStatus DemoController::ParseLine(const std::string& line, std::vector<InputEvent>& queue, std::vector<EventQueue>& demo)
{
    if(line.starts_with('['))
    {
        demo.emplace_back(queue);
        queue.clear();
        return Status::NO_ERR;
    }
    else if(line.starts_with('{'))
    {
        TheatreManager::LoadTheatreFromFile(line.substr(1, line.size() - 2));
        return Status::NO_ERR;
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
        if(character == '<')
        {
            sAdvanceCharacter(character, i, line);
            while(character != '>')
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
                while(character != '>')
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

    id_t id;
    int status;
    int just_changed;
    glm::vec2 curr_mouse{0.0f, 0.0f};
    glm::vec2 last_mouse{0.0f, 0.0f};

    if(!StringToNum(id, id_buffer) ||
        !StringToNum(status, status_buffer) ||
        !StringToNum(just_changed, just_changed_buffer))
    { return Status::DemoControllerLINE_FAILED; }
    else if(is_mouse_motion &&
        (!StringToNum(curr_mouse, curr_mouse_buffer) ||
            !StringToNum(last_mouse, last_mouse_buffer)))
    { return Status::DemoControllerLINE_FAILED; }

    InputEvent new_event;
    new_event.mBinding = InputManager::GetBinding(id);
    new_event.mBinding.status = static_cast<InputStatus>(status);
    new_event.mBinding.just_changed = static_cast<bool>(just_changed);
    new_event.mActions = InputManager::GetActions(id);
    new_event.mCurMousePos = curr_mouse;
    new_event.mLastMousePos = last_mouse;
    queue.push_back(new_event);
    return Status::DemoControllerLINE_PARSED;
}
