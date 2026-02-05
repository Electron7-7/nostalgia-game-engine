#ifndef THEATRE_FILE_PARSER_H
#define THEATRE_FILE_PARSER_H

#include <Nostalgia/theatre/lexer.hpp>

namespace TheatreFile
{
    /// Forward Declaration
    struct TheatreData;

    Error Parser(Farg<TokenArray> inTokens, Shared<TheatreData>& outData);
}

#endif // THEATRE_PARSER_H
