#include "theatre_interpreting.hpp"
#include "theatre.hpp"

#include <fstream>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

static std::string s_TheatreFileStringData = "";

const std::set<std::string> c_TheatreFileExtensions =
{
    ".theatre",
    ".t",
    ".graphxtheatre",
    ".gt",
    ".nostalgiatheatre",
    ".nt"
};

SafeStatus try_LoadTheatreFile(const std::string& theatre_file)
{
    if(!fs::exists(fs::path(theatre_file)))
    { return Status::TheatreParserFILE_DOES_NOT_EXIST; }

    if(!fs::path(theatre_file).has_extension() || !c_TheatreFileExtensions.contains(fs::path(theatre_file).extension().string()))
    { return Status::TheatreParserWRONG_FILE_EXTENSION; }

    std::ifstream theatre_file_stream(theatre_file);
    s_TheatreFileStringData = std::string(std::istreambuf_iterator<char>(theatre_file_stream), std::istreambuf_iterator<char>());
    theatre_file_stream.close();

    return Status::NO_ERROR;
}

Theatre InterpretTheatreFile()
{
    Theatre new_theatre;

    // FIXME: This is in progress; used to be a 'SafeReturn<Theatre>'
    if(s_TheatreFileStringData.empty())
    { return new_theatre; }



    return new_theatre;
}
