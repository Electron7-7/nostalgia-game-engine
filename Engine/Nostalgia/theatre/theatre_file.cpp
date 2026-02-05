#include "./theatre_file.hpp"
#include "./theatre_data.hpp"
#include "./parser.hpp"
#include "filesystem/filesystem.hpp"

using namespace TheatreFile;

Error TheatreFile::ParseTheatreFile(Sarg inPathToFile,
    Shared<TheatreData>& outData)
{
    std::string file_path{inPathToFile};
    if(!FileSystem::IsFile(file_path))
    {
        file_path = FileSystem::GetCurrentDirectory() + "/" + inPathToFile;
        if(!FileSystem::IsFile(file_path))
            { return ERR_NOT_FOUND; }
    }
    return ParseTheatreFile(FileData{file_path}, outData);
}

Error TheatreFile::ParseTheatreFile(Farg<FileData> inFileData,
    Shared<TheatreData>& outData)
{
    TokenArray tokens{};
    if(!print_error_enum(Lexer(inFileData, tokens)))
        { return ERR_THEATRE_LEXER; }
    else if(!print_error_enum(Parser(tokens, outData)))
        { return ERR_THEATRE_PARSER; }
    return OK;
}
