#ifndef THEATRE_FILE_PARSER_H
#define THEATRE_FILE_PARSER_H

#include "safe_return.hpp"

#include <string>

class TheatreFileParser
{
public:
    SafeStatus try_LoadTheatreFile(const std::string& TheatreFile);
    SafeStatus try_ParseTheatreFile();
};

extern TheatreFileParser* g_pTheatreFileParser;

#endif // THEATRE_FILE_PARSER_H
