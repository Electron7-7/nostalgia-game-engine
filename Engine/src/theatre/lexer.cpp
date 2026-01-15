#include "parser.hpp"
#include "thing_factory.hpp"
#include "core/printing.hpp"
#include "core/enum_prettifier.hpp"
#include "filesystem/file_data.hpp"
#include "thirdparty/frozen/set.h"

// static constexpr char                         cTheatreNameDelimiter{'@'};
// static constexpr char                         cTheatreIndexDelimiter{'#'};
static constexpr char        cCommentDelimiter{'/'};
static constexpr std::string cCommentDelimiterSingleLine{"//"};
static constexpr std::string cCommentDelimiterMultiLine{"///"};

static constexpr frozen::set<char, 3>
    cLiteralDelimiters{'[', '\'', '"'};
static constexpr frozen::set<char, 2>
    cOperators{'=', ':'};
static constexpr frozen::set<char, 4>
    cWhitespace{' ', '\t', '\n', '\r'};
static constexpr frozen::set<char, 12>
    cSeparators{';', ',', '}', ']', '[', '\'', '"', '{', '<', '>', '(', ')'};
static constexpr frozen::set<std::string, 4>
    cKeywords{"Resources", "Thinkers", "Child", "Parent"};

static void debug_PrettifyEnums();

enum class Comment
{ SingleLine, MultiLine, None };

using namespace TheatreFile;

Error TheatreFile::Lexer(Sarg inPath, TokenArray& outTokens)
{
#ifdef DEBUGGING
    debug_PrettifyEnums();
#endif
    Error status{Lexer(MakeShared<FileData>(inPath), outTokens)};
    if(!print_error_enum(status))
        { print_error("Failed to load Theatre from file: '{}'", inPath); }
    return status;
}

Error TheatreFile::Lexer(Shared<FileData> inData, TokenArray& outTokens)
{
#ifdef DEBUGGING
    debug_PrettifyEnums();
#endif
    if(!inData->Status())
        { return inData->Status(); }

    Sarg data_string{inData->DataString()};
    size_t file_size{data_string.size()};

    TokenArray tokens{};
    std::string value_buffer{};

    for(size_t i{0}; i < file_size; ++i)
    {
        char character{data_string[i]};
        if(character == cCommentDelimiter)
        {
            if(i + 1 < file_size)
            {
                if(auto try_singleline_comment{data_string.substr(i, 2)};
                    !try_singleline_comment.compare(cCommentDelimiterSingleLine))
                {
                    if(i + 2 < file_size)
                    {
                        if(std::string try_multiline_comment{data_string.substr(i, 3)};
                            !try_multiline_comment.compare(cCommentDelimiterMultiLine))
                        {
                            i+=2;
                            tokens.emplace_back(TokenName::MultilineComment,
                                try_multiline_comment);
                            continue;
                        }
                    }
                    ++i;
                    tokens.emplace_back(TokenName::SinglelineComment,
                        try_singleline_comment);
                    continue;
                }
            }
        }
        else if(cOperators.contains(character))
            { tokens.emplace_back(TokenName::Operator, std::string{character}); continue; }
        else if(cSeparators.contains(character)
            and value_buffer.empty()
            and !cLiteralDelimiters.contains(character))
        {
            tokens.emplace_back(TokenName::Separator, std::string{character});
            continue;
        }
        else if(cLiteralDelimiters.contains(character) and !value_buffer.empty())
        {
            tokens.emplace_back(TokenName::Literal, value_buffer + character);
            value_buffer.clear();
            continue;
        }
        else if(cWhitespace.contains(character) or cSeparators.contains(character))
        {
            TokenName name_buffer{TokenName::Identifier};
            if(!value_buffer.empty())
            {
                if(cKeywords.contains(value_buffer) or ThingFactory::IsThing({value_buffer}))
                    { name_buffer = TokenName::Keyword; }
                tokens.emplace_back(name_buffer, value_buffer);
                value_buffer.clear();
            }
            if(cWhitespace.contains(character))
                { name_buffer = TokenName::Whitespace; }
            else
                { name_buffer = TokenName::Separator; }
            tokens.emplace_back(name_buffer, std::string{character});
            continue;
        }
        value_buffer.push_back(character);
    }
    outTokens = tokens;
    return OK;
}

void debug_PrettifyEnums()
{
    static bool already_prettified{false};
    if(already_prettified)
        { return; }
    EnumPrettifier::Register(ThingVarType::String,   "String");
    EnumPrettifier::Register(ThingVarType::Bool,     "Bool");
    EnumPrettifier::Register(ThingVarType::Number,   "Number");
    EnumPrettifier::Register(ThingVarType::Enum,     "Enum");
    EnumPrettifier::Register(ThingVarType::Child,    "Child");
    EnumPrettifier::Register(ThingVarType::Parent,   "Parent");
    EnumPrettifier::Register(ThingVarType::ID,       "ID");
    EnumPrettifier::Register(ThingVarType::None,     "None");
    EnumPrettifier::Register(TokenName::Identifier,  "Identifier");
    EnumPrettifier::Register(TokenName::Keyword,     "Keyword");
    EnumPrettifier::Register(TokenName::Separator,   "Separator");
    EnumPrettifier::Register(TokenName::Operator,    "Operator");
    EnumPrettifier::Register(TokenName::Literal,     "Literal");
    EnumPrettifier::Register(TokenName::Whitespace,  "Whitespace");
    EnumPrettifier::Register(TokenName::None,        "None");
    EnumPrettifier::Register(TokenName::SinglelineComment, "SinglelineComment");
    EnumPrettifier::Register(TokenName::MultilineComment,  "MultilineComment");
    already_prettified = true;
}
