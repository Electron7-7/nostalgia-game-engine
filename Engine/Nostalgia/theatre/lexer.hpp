#ifndef THEATRE_FILE_LEXER_H
#define THEATRE_FILE_LEXER_H

namespace TheatreFile
{
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

    Error Lexer(Farg<FileData> inFileData, TokenArray& outTokens);
}

#endif // THEATRE_FILE_LEXER_H
