#include "demo_controller.hpp"
#include "binding.hpp"
#include "event_queue.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/input_manager.hpp"
#include "filesystem/filesystem.hpp"
#include "theatre_parser/theatre_parser.hpp"
#include "printing.hpp"
#include "string_to_num.hpp"
#include "truncate_string.hpp"

#include <sstream>

static std::string sExtension{".gen1_demo"};
static std::string sDemoTheatreName{""};
static std::vector<EventQueue> sDemo{};
static size_t sIndexOfTheatreLoad{0};
static size_t sPlaybackIndex{0};

static DemoController sDemoController;
DemoController* g_pDemoController = &sDemoController;

void DemoController::NotifyOfTheatreChange()
{
    if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL && is_recording_)
        { sIndexOfTheatreLoad = sDemo.size() - 1; }
}

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
    std::string output{std::format("!Nostalgia Gen 1 Demo")};
    size_t i = -1;
    for(EventQueue& queue : sDemo)
    {
        if(i == sIndexOfTheatreLoad)
            { output += std::format("\n{{{}}}", theatre_path); }
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
    sPlaybackIndex = 0;
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
        if(sPlaybackIndex >= sDemo.size())
            { StopPlaying(); return; }
        queue = sDemo.at(sPlaybackIndex);
        if(sIndexOfTheatreLoad == sPlaybackIndex)
            { g_pTheatreManager->LoadTheatreFromFile(sDemoTheatreName); }
        ++sPlaybackIndex;
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
        sDemoTheatreName = line.substr(1, line.size() - 2);
        sIndexOfTheatreLoad = demo.size() - 1;
        return Status::NO_ERR;
    }
    else if(line.starts_with('!'))
        { return Status::NO_ERR; }

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

    id_t id{ID::Invalid};
    int status{-1};
    int just_changed{-1};
    glm::vec2 curr_mouse{0.0f};
    glm::vec2 last_mouse{0.0f};

    if(StringToNum(id, id_buffer) &&
        g_pInputManager->BindingExists(id) &&
        StringToNum(status, status_buffer) &&
        StringToNum(just_changed, just_changed_buffer))
    {
        if(is_mouse_motion &&
            StringToNum(curr_mouse, curr_mouse_buffer) &&
            StringToNum(last_mouse, last_mouse_buffer))
            { queue.emplace_back(id, curr_mouse, last_mouse); }
        else
            { queue.emplace_back(id); }

        g_pInputManager->GetBinding(id).status_ = static_cast<InputStatus>(status);
        g_pInputManager->GetBinding(id).just_changed_ = static_cast<bool>(just_changed);
        return Status::DemoControllerLINE_PARSED;
    }
    return Status::DemoControllerLINE_FAILED;
}
