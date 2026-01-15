#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "fwd/filesystem.hpp"
#include "fwd/theatre.hpp"
#include "core/id.hpp"
#include "core/farg.hpp"
#include "core/error.hpp"
#include "core/concepts.hpp"
#include "core/smart_pointers.hpp"
#include "math/concepts.hpp"
#include "math/glm_concepts.hpp"
#include <vector>

namespace TheatreFile
{
    template<typename T>
        concept ThingVariable_t = Number<T>
            or GLMContainer<T>
            or Bool<T>
            or IsEnum<T>
            or StringType<T>
            or ID_t<T>;

    enum class ThingVarType
    { String, Bool, Number, Enum, Child, Parent, ID, None };

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

    struct ThingVariable
    {
        std::string  name{};  // If Child/Parent, this is the type name
        std::string  value{}; // If Child/Parent, this is the Child/Parent's name
        ThingVarType type{ThingVarType::None};

        void clear()
        { *this = ThingVariable{}; }

        bool invalid() const
        { return type == ThingVarType::None or (name.empty() and value.empty()); }
    };

    using TokenArray = std::vector<Token>;
    using ThingVarArray = std::vector<ThingVariable>;

    struct ThingData
    {
        PID           type{};
        std::string   name{};
        ThingVarArray variables{};

        void clear() { *this = ThingData{}; }

        int get_children(ThinkerChildren& outChildren);
        int set_children(Farg<ThinkerChildren> inChildren);

        Error get_parent(ThinkerRelative& outParent);
        Error set_parent(Farg<ThinkerRelative> inParent);

        template<typename T, StringType... Names> requires ThingVariable_t<T>
            Error set_variable(Farg<T> inValue, Sarg inName);

        template<typename T, StringType... Names> requires ThingVariable_t<T>
            Error get_variable(T& outValue, Names... inNames);

        template<StringType... Names>
            int erase_variables(Names... inNames);
    };

    using TheatreData = std::vector<ThingData>;

    Error Parser(Farg<TokenArray> inTokens, TheatreData& outData);
    Error Lexer(Sarg inFilePath, TokenArray& outTokens);
    Error Lexer(Shared<FileData> inFileData, TokenArray& outTokens);
}

#endif // THEATRE_PARSER_H
