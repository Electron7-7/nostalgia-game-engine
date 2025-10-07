#include "demo.hpp"
#include "debug.hpp"
#include "filesystem/filesystem.hpp"
#include "input/event.hpp"

#include <fstream>

// The '.dem' file extension is purely cosmetic for now
static std::string sExtension = "dem";
static unsigned int sDuplicateNameIterator = 0;
static constexpr std::string sDefaultBaseName{"demo"};

Demo::Demo() = default;

Demo::Demo(const TheatreData& theatre_data, const std::vector<EventQueue>& queues, const std::string& base_name):
    mQueues(queues),
    mTheatreData(theatre_data),
    mName(base_name)
{}

Demo::Demo(const TheatreData& theatre_data):
    Demo(theatre_data, {}, sDefaultBaseName)
{}

Demo::Demo(const TheatreData& theatre_data, const std::string& name):
    Demo(theatre_data, {}, name)
{}

Demo::Demo(const TheatreData& theatre_data, const std::vector<EventQueue>& queues):
    Demo(theatre_data, queues, sDefaultBaseName)
{}

bool Demo::Play()
{
    if(mIsPlaying)
        { return true; }
    else if(mTheatreData == TheatreData::Missing)
        { return false; }
    return mIsPlaying = true;
}

bool Demo::WriteToFile()
{
    if(mQueues.empty())
        { return false; }
    std::string base_name(mName);
    while(true)
    {
        mName = base_name;
        if(sDuplicateNameIterator > 0)
            { mName += std::format("_{}", sDuplicateNameIterator); }
        mName += std::format(".{}", sExtension);
        if(!FileSystem::Exists(mName))
            { break; }
        ++sDuplicateNameIterator;
    }

#   pragma message("TODO: implement functions for reading/writing files in `FileSystem`")
    InputEvent temp_event;
    std::ofstream demo_file(mName);
    size_t i = 0;
    for(EventQueue& queue : mQueues)
    {
        PRINT_DEBUG("Demo::WriteToFile - File: {}, Queue #{}", mName, ++i)
        size_t j = 0;
        while(queue.GetNextEvent(temp_event))
        {
            PRINT_DEBUG("\tEvent #{} Demo String: {}", ++j, temp_event.DemoString())
            demo_file << std::format("{}\n", temp_event.DemoString());
        }
        demo_file << std::format("<{}>\n", i);
    }
    demo_file.close();
    return true;
}

bool Demo::GetNextQueue(EventQueue& output)
{
    if(mQueues.size() >= 1)
        { mQueues.pop_back(); }
    if(mQueues.size() < 1)
        { return false; }
    output = mQueues.back();
    return true;
}

const std::string& Demo::name() const
{ return mName; }

void Demo::push_back(const EventQueue& queue)
{ mQueues.push_back(queue); }

void Demo::push_back(const std::vector<InputEvent>& queue)
{
    mQueues.emplace_back();
    mQueues.back().ReplaceQueue(queue);
}

void Demo::clear()
{ mQueues.clear(); }
