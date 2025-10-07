#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "safe_return.hpp"
#include "theatre_data.hpp"

struct TheatreParser
{
public:
    static SafeStatus try_ParseTheatre(TheatreData& Output);
    static SafeStatus try_LoadTheatreFromFile(TheatreData& Output, const std::string& FilePath);
    static SafeStatus try_LoadTheatreFromMemory(TheatreData& Output, const std::string& FileData);
};

#ifdef DEBUGGING
    extern size_t g_Column;
    extern size_t g_Line;
    extern int    g_BreakOnColumn;
    extern int    g_BreakOnLine;
#endif // DEBUGGING

#endif // THEATRE_PARSER_H
