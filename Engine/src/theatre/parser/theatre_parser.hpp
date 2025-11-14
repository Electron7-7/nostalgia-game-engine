#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "filesystem/fwd.hpp"

#include "theatre_data.hpp"
#include "common/safe_return.hpp"

struct TheatreParser
{
public:
    static bool ParseTheatreFile(const std::string& FilePath, TheatreData& Output);
    static bool ParseTheatreFileFromMemory(const FileData& FileData, TheatreData& Output);
    static SafeReturn<std::string> WriteTheatre(const TheatreData& TheatreData, const std::string& OutputPath);

private:
    static FileData m_sTheatreFile;
    static bool ReadTheatre(TheatreData&);
};

#ifdef DEBUGGING
    extern size_t g_Column;
    extern size_t g_Line;
    extern int    g_BreakOnColumn;
    extern int    g_BreakOnLine;
#endif // DEBUGGING

#endif // THEATRE_PARSER_H
