#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "safe_return.hpp"
#include "theatre_data.hpp"

struct TheatreParser
{
public:
    static const TheatreData& GetTheatreData();
    static SafeStatus try_ParseTheatre();

    static SafeStatus try_LoadTheatreFromFile(const std::string& FilePath);
    static void LoadTheatreFromMemory(const std::string& FileData);

private:
    static unsigned int GetNewID();
};

#ifdef DEBUGGING
    extern size_t g_Column;
    extern size_t g_Line;
    extern int    g_BreakOnColumn;
    extern int    g_BreakOnLine;
#endif // DEBUGGING

#endif // THEATRE_PARSER_H
