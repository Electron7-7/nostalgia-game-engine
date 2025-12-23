#define DO_NOT_COMPILE
#ifndef DO_NOT_COMPILE

#include "demo_controller.hpp"
#include "bindings.hpp"
#include "core/printing.hpp"
#include "theatre/parser/number_parser.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/input_manager.hpp"
#include "filesystem/filesystem.hpp"
#include "theatre/parser/theatre_parser.hpp"

#include <sstream>

static std::string sExtension{".gen1_demo"};
static std::string sDemoTheatreName{""};
static std::vector<InputEvent> sDemo{};
static size_t sIndexOfTheatreLoad{0};
static size_t sPlaybackIndex{0};

static DemoController sDemoController;
DemoController* g_pDemoController = &sDemoController;

void DemoController::NotifyOfTheatreChange()
{
    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL && is_recording_)
        { sIndexOfTheatreLoad = sDemo.size() - 1; }
}

bool DemoController::Record()
{
    if(is_recording_)
        { return false; }
    else if(is_playing_)
        { return print_error("DemoController::Record - Cannot record a gen1 demo while a gen1 demo is being played"); }
    else if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { return print_error("DemoController::Record - You can't start recording a gen1 demo inside a Theatre"); }
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
            { return print_warning("DemoController::Save - unable to save Theatre file for gen1 demo"); }
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

    std::string output{std::format("!Nostalgia Gen 1 Demo")};
    size_t i = -1;
    for(InputEvent& event : sDemo)
    {
        if(i == sIndexOfTheatreLoad)
            { output += std::format("\n{{{}}}", theatre_path); }
        output += std::format("\n[{}] {}", ++i, event.DemoString());
    }
    return FileSystem::try_WriteFileFromString(name, output);
}

bool DemoController::Play(const std::string& path)
{
    if(is_playing_)
        { return false; }
    else if(is_recording_)
        { return print_error("DemoController::Play - cannot play a gen1 demo while a gen1 demo is being recorded"); }
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
    IManager::ShutdownTheatre();
    return !(is_playing_ = false);
}

bool DemoController::PlayingDemo() const
{ return is_playing_; }

void DemoController::ProcessEvent(InputEvent& event)
{
    if(is_playing_)
    {
        if(sPlaybackIndex >= sDemo.size())
            { StopPlaying(); return; }
        event = sDemo.at(sPlaybackIndex);
        if(sIndexOfTheatreLoad == sPlaybackIndex)
            { g_pTheatreManager->LoadTheatreFromFile(sDemoTheatreName); }
        ++sPlaybackIndex;
    }
    else if(is_recording_)
        { sDemo.push_back(event); }
}

bool DemoController::ParseDemo(const std::string& path, std::vector<InputEvent>& output)
{
    std::string file_data("");
    if(!FileSystem::try_ReadFileToString(path, file_data) &&
        !FileSystem::try_ReadFileToString(FileSystem::ReplaceExtension(sExtension, path), file_data))
    {
        return print_error("DemoController::LoadDemoFromFile - failed to load gen1 demo '{}{}'\n",
            path.substr(0, 68),
            (path.size() <= 68) ? "" : "...");
    }
    InputEvent event;
    bool l_AtLeastOneLineParsed = false;
    std::stringstream demo_data{file_data};
    std::string line;
    while(std::getline(demo_data, line))
    {
        SafeStatus status = ParseLine(line, event, output);
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

SafeStatus DemoController::ParseLine(const std::string& line, InputEvent& event, std::vector<InputEvent>& demo)
{
    if(line.starts_with('['))
    {
        demo.emplace_back(event);
        event.clear();
        std::string buffer{""};
        glm::vec2 curr_mouse{0.0f};
        glm::vec2 last_mouse{0.0f};
        bool current_mouse{true};
        bool parsing_data{false};
        for(const char& char_ : line)
        {
            if(char_ == ')')
            {
                if(current_mouse)
                    { StringToNum(curr_mouse, buffer); buffer.clear(); }
                else
                    { StringToNum(last_mouse, buffer); buffer.clear(); }
                current_mouse = parsing_data = false;
                continue;
            }
            else if(char_ == '(')
                { parsing_data = true; continue; }
            if(parsing_data)
                { buffer.append(1, char_); }
        }
        event.UpdateMouseMotion(curr_mouse, last_mouse);
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

    std::string id_buffer{""};
    std::string status_buffer{""};
    std::string just_changed_buffer{""};
    std::vector<std::string> actions{};

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
                while(character != ':')
                {
                    just_changed_buffer.append(1, character);
                    sAdvanceCharacter(character, i, line);
                }
                sAdvanceCharacter(character, i, line);
                std::string action_buffer{""};
                while(character != '>')
                {
                    if(character == ',')
                        { actions.push_back(action_buffer); action_buffer.clear(); }
                    else
                        { action_buffer.append(1, character); }
                    sAdvanceCharacter(character, i, line);
                }
                actions.push_back(action_buffer);
            }
            continue;
        }
    }

    ID id{id_buffer};
    int status{-1};
    int just_changed{-1};

    if(!id.invalid() &&
        g_pInputManager->BindingExists(id) &&
        StringToNum(status, status_buffer) &&
        StringToNum(just_changed, just_changed_buffer))
    {
        event.add(InputBinding{
            ID{id_buffer},
            actions,
            false,
            static_cast<InputStatus>(status),
            static_cast<bool>(just_changed)});
        return Status::DemoControllerLINE_PARSED;
    }
    return Status::DemoControllerLINE_FAILED;
}

#endif // DO_NOT_COMPILE
