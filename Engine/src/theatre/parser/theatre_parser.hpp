#ifdef FWD_DCL
    struct TheatreParser;
    extern unsigned long gColumn;
    extern unsigned long gLine;
    extern int    gBreakOnColumn;
    extern int    gBreakOnLine;
#elif !defined THEATRE_PARSER_H
#define THEATRE_PARSER_H

#define FWD_DCL
#   include "filesystem/file_data.hpp"
#undef  FWD_DCL

#include "theatre_data.hpp"
#include "core/safe_return.hpp"

struct TheatreParser
{
public:
    static bool ParseTheatreFile(Farg<std::string> FilePath, TheatreData& Output);
    static bool ParseTheatreFileFromMemory(Farg<FileData> FileData, TheatreData& Output);
    static SafeReturn<std::string> WriteTheatre(Farg<TheatreData> TheatreData, Farg<std::string> OutputPath);

private:
    static FileData m_sTheatreFile;
    static bool ReadTheatre(TheatreData&);
};

extern size_t gColumn;
extern size_t gLine;
extern int    gBreakOnColumn;
extern int    gBreakOnLine;

#endif // THEATRE_PARSER_H
