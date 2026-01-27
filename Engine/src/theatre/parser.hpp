#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "fwd/filesystem.hpp"
#include "theatre/variable_registry.hpp"
#include "theatre/number_parser.hpp"
#include "math/glm_concepts.hpp"
#include <math/glm_format.hpp> // used when converting GLM variables to strings

namespace TheatreFile
{
    /// Forward Declaration
    struct TheatreData;

    // https://en.wikipedia.org/wiki/Lexical_analysis#Lexical_token_and_lexical_tokenization
    enum class TokenName
    {
        Identifier,
        Keyword,
        Separator,
        Operator,
        Literal,
        SinglelineComment,
        MultilineComment,
        Whitespace,
        None
    };

    struct Token
    {
        TokenName category{TokenName::None};
        std::string token{"\0"};
    };

    using TokenArray = std::vector<Token>;

    Error Parser(Farg<TokenArray> inTokens, Shared<TheatreData>& outData);
    Error Lexer(Farg<FileData> inFileData, TokenArray& outTokens);
}

#endif // THEATRE_PARSER_H
