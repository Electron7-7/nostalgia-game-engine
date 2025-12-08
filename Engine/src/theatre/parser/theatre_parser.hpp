#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "filesystem/fwd.hpp"

#include "theatre_data.hpp"
#include "core/safe_return.hpp"

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

extern size_t gColumn;
extern size_t gLine;
extern int    gBreakOnColumn;
extern int    gBreakOnLine;

#endif // THEATRE_PARSER_H
