#include "../theatre_file.hpp"
#include "thirdparty/frozen/set.h"
#include "thirdparty/frozen/string.h"

static constexpr char        cCommentDelimiter{'/'};
static constexpr const char* cCommentDelimiterSingleLine{"//"};
static constexpr const char* cCommentDelimiterMultiLine{"///"};

static constexpr frozen::set<char, 2>
    cStringDelimiters{'\'', '"'};
static constexpr frozen::set<char, 2>
    cValidLiterals{'.', '-'};
static constexpr frozen::set<char, 2>
    cOperators{'=', ':'};
static constexpr frozen::set<char, 4>
    cWhitespace{' ', '\t', '\n', '\r'};
static constexpr frozen::set<char, 11>
    cSeparators{',', '[', ']', '{', '}', '<', '>', '(', ')', '@', '#'};
static constexpr frozen::set<frozen::string, 4>
    cKeywords{"Child", "Parent", "declare", "inherits"};

enum Comment { SINGLE, MULTI, NO_COMMENT };

static void debug_PrettifyEnums();

using namespace TheatreFile;

Error TheatreFile::Lex(Farg<FileData> inData, TokenArray& outTokens)
{
#ifdef NOSTALGIA_DEBUGGING
    debug_PrettifyEnums();
#endif
    if(!inData.Status())
        { return inData.Status(); }

    Sarg data_string{inData.DataString()};
    size_t file_size{data_string.size()};

    TokenArray tokens{};
    std::string value_buffer{};

    Comment in_comment{NO_COMMENT};

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
                            if(in_comment == NO_COMMENT)
                                { in_comment = MULTI; }
                            else if(in_comment == MULTI)
                                { in_comment = NO_COMMENT; }
                            continue;
                        }
                    }
                    ++i;
                    tokens.emplace_back(TokenName::SinglelineComment,
                        try_singleline_comment);
                    if(in_comment == NO_COMMENT)
                        { in_comment = SINGLE; }
                    continue;
                }
            }
        }
        else if(cOperators.contains(character))
            { tokens.emplace_back(TokenName::Operator, std::string{character}); continue; }
        else if(cStringDelimiters.contains(character))
        {
            if(in_comment != NO_COMMENT)
            {
                tokens.emplace_back(TokenName::Identifier, std::string{character});
                continue;
            }
            else if(i+1 >= data_string.size()) { continue; }
#pragma message("TODO: clean this up")
            value_buffer += data_string[i++];
            while(!cStringDelimiters.contains(data_string[i])
                and data_string[i] != '\n')
                { value_buffer += data_string[i++]; }
            value_buffer += data_string[i];

            tokens.emplace_back(TokenName::Literal, value_buffer);
            value_buffer.clear();
            continue;
        }
        else if(cWhitespace.contains(character) or cSeparators.contains(character))
        {
            TokenName name_buffer{TokenName::Identifier};
            if(!value_buffer.empty())
            {
                if(cKeywords.contains(frozen::string{value_buffer}))
                    { name_buffer = TokenName::Keyword; }
                else
                {
                    name_buffer = TokenName::Literal;
                    for(char character : value_buffer)
                    {
                        if(!(character >= '0' and character <= '9')
                            and !cValidLiterals.contains(character))
                                { name_buffer = TokenName::Identifier; }
                    }
                }
                tokens.emplace_back(name_buffer, value_buffer);
                value_buffer.clear();
            }
            if(cWhitespace.contains(character))
            {
                name_buffer = TokenName::Whitespace;
                if(character == '\n' and in_comment == SINGLE)
                    { in_comment = NO_COMMENT; }
            }
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
