#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "theatre/parser/theatre_data.hpp"
#include "core/safe_return.hpp"
#include "filesystem/file_data.hpp"

struct TheatreParser
{
public:
    static bool ParseTheatreFile(Sarg FilePath, TheatreData& Output);
    static bool ParseTheatreFileFromMemory(Farg<FileData> FileData, TheatreData& Output);
    static SafeReturn<std::string> WriteTheatre(Farg<TheatreData> TheatreData, Sarg OutputPath);

private:
    static FileData m_sTheatreFile;
    static bool ReadTheatre(TheatreData&);
};

extern size_t gColumn,
    gLine,
    gBreakOnColumn,
    gBreakOnLine;

#endif // THEATRE_PARSER_H
