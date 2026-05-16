#include "../theatre_file.hpp"
#include "thirdparty/frozen/set.h"
#include "thirdparty/frozen/string.h"

using namespace TheatreFile;

static constexpr char        cCommentDelimiter{'/'};
static constexpr const char* cCommentDelimiterSingleLine{"//"};
static constexpr const char* cCommentDelimiterMultiLine{"///"};

static constexpr frozen::set<char, 2>
    cValidLiterals{'.', '-'};
static constexpr frozen::set<char, 2>
    cOperators{'=', ':'};
static constexpr frozen::set<char, 4>
    cWhitespace{' ', '\t', '\n', '\r'};
static constexpr frozen::set<char, 14>
    cSeparators{',', '[', ']', '|', '{', '}', '<', '>', '(', ')', '@', '#', '\'', '"'};
static constexpr frozen::set<frozen::string, 4>
    cKeywords{"Child", "Parent", "declare", "inherits"};

enum Comment { SINGLE, MULTI, NO_COMMENT };

static void debug_RegisterEnums();
static void s_HandleWordBuffer(std::string& ioBuffer, TokenArray& ioTokens, Comment inComment, char inChar)
{
    if(ioBuffer.empty())
        { return; }
    TokenName name_buffer{TokenName::Literal};
    if(cKeywords.contains(frozen::string{ioBuffer}))
        { name_buffer = TokenName::Keyword; }
    else
    {
        for(char character : ioBuffer)
        {
            if(not (character >= '0' and character <= '9')
                and not cValidLiterals.contains(character))
                { name_buffer = TokenName::Identifier; }
        }
    }
    ioTokens.emplace_back(name_buffer, ioBuffer);
    ioBuffer.clear();
}

using namespace TheatreFile;

Error TheatreFile::Lex(Farg<FileData> inData, TokenArray& outTokens)
{
#ifdef NOSTALGIA_DEBUGGING
    debug_RegisterEnums();
#endif
    if(not inData.status())
        { return inData.status(); }

    std::string data_string{inData.raw_data_str()};
    size_t file_size{data_string.size()};

    TokenArray tokens{};
    std::string value_buffer{};

    Comment in_comment{NO_COMMENT};

    for(size_t i{0}; i < file_size; ++i)
    {
        bool _do_break{false};
        char character{data_string[i]};
        if(character == cCommentDelimiter)
        {
            if(i + 1 < file_size)
            {
                if(auto _singleline{data_string.substr(i, 2)}; _singleline == cCommentDelimiterSingleLine)
                {
                    if(i + 2 < file_size)
                    {
                        if(auto _multiline{data_string.substr(i, 3)}; _multiline == cCommentDelimiterMultiLine)
                        {
                            i+=2;
                            tokens.emplace_back(TokenName::MultilineComment, _multiline, _do_break);
                            if(in_comment == NO_COMMENT)
                                { in_comment = MULTI; }
                            else if(in_comment == MULTI)
                                { in_comment = NO_COMMENT; }
                            continue;
                        }
                    }
                    ++i;
                    tokens.emplace_back(TokenName::SinglelineComment, _singleline, _do_break);
                    if(in_comment == NO_COMMENT)
                        { in_comment = SINGLE; }
                    continue;
                }
            }
        }
        else if(cOperators.contains(character))
        {
            s_HandleWordBuffer(value_buffer, tokens, in_comment, character);
            tokens.emplace_back(TokenName::Operator, std::string{character}, _do_break);
            continue;
        }
        else if(cWhitespace.contains(character) or cSeparators.contains(character))
        {
            s_HandleWordBuffer(value_buffer, tokens, in_comment, character);
            TokenName name_buffer{TokenName::Separator};
            if(cWhitespace.contains(character))
            {
                name_buffer = TokenName::Whitespace;
                if(character == '\n' and in_comment == SINGLE)
                    { in_comment = NO_COMMENT; }
            }
            tokens.emplace_back(name_buffer, std::string{character}, _do_break);
            continue;
        }
        value_buffer.push_back(character);
    }
    outTokens = tokens;
    return OK;
}

void debug_RegisterEnums()
{
    static bool already_prettified{false};
    if(already_prettified)
        { return; }
    EnumRegistry::Register(TokenName::Identifier,        "Lexer::Token::Identifier");
    EnumRegistry::Register(TokenName::Keyword,           "Lexer::Token::Keyword");
    EnumRegistry::Register(TokenName::Separator,         "Lexer::Token::Separator");
    EnumRegistry::Register(TokenName::Operator,          "Lexer::Token::Operator");
    EnumRegistry::Register(TokenName::Literal,           "Lexer::Token::Literal");
    EnumRegistry::Register(TokenName::Whitespace,        "Lexer::Token::Whitespace");
    EnumRegistry::Register(TokenName::None,              "Lexer::Token::None");
    EnumRegistry::Register(TokenName::SinglelineComment, "Lexer::Token::SinglelineComment");
    EnumRegistry::Register(TokenName::MultilineComment,  "Lexer::Token::MultilineComment");
    already_prettified = true;
}
