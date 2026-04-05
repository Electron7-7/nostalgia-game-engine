#ifndef THEATRE_FILE_H
#define THEATRE_FILE_H

#include <Nostalgia/things/thing_data.hpp>

namespace TheatreFile
{
    extern bool gDebugPrintLexerLogs,
        gDebugPrintParserLogs,
        gDebugDontPrintWhitespaceInLexerLogs,
        gDebugDontPrintCommentsInLexerLogs;

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

    struct TheatreData
    {
        std::string name{"Untitled Theatre"};
        uint index{ID::Invalid};
        std::vector<ThingData> data{};

        template<typename... _Args>
            auto emplace_back(_Args&&... __args) noexcept
            { return data.emplace_back(std::forward<_Args>(__args)...); }

        void push_back(Farg<ThingData> inData) noexcept { data.push_back(inData); }
        void clear() noexcept { data.clear(); }
        std::string get_log(bool doIndentation = false, bool doANSIColors = false) const;
        std::string get_parsable_string() const;

        auto begin()        noexcept { return data.begin();  }
        auto end()          noexcept { return data.end();    }
        auto cbegin() const noexcept { return data.cbegin(); }
        auto cend()   const noexcept { return data.cend();   }
    };

    using TokenArray = std::vector<Token>;

    Error Lex(Farg<FileData> inFileData, TokenArray& outTokens);
    Error Parse(Farg<TokenArray> inTokens, Shared<TheatreData>& outData);
    Error Load(Sarg inPathToFile, Shared<TheatreData>& outData);
    Error Load(Farg<FileData> inFileData, Shared<TheatreData>& outData);
}

#endif // THEATRE_FILE_H
