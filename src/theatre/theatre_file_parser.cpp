#include "theatre_file_parser.hpp"

#include <fstream>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

static std::string s_TheatreFileStringData = "";

static TheatreFileParser s_TheatreFileParser;
TheatreFileParser* g_pTheatreFileParser = &s_TheatreFileParser;

const std::set<std::string> c_TheatreFileExtensions =
{
    ".theatre",
    ".t",
    ".graphxtheatre",
    ".gt",
    ".nostalgiatheatre",
    ".nt"
};

SafeStatus TheatreFileParser::try_LoadTheatreFile(const std::string& theatre_file)
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

SafeStatus TheatreFileParser::try_ParseTheatreFile()
{
    if(s_TheatreFileStringData.empty())
    { return Status::TheatreParserNO_THEATRE_LOADED; }

    // Parse the theatre file
    // Create the Theatre object
    // Call `g_pTheatreManager->LoadTheatre(new_theatre_object)`

    return Status::NO_ERROR;
}
