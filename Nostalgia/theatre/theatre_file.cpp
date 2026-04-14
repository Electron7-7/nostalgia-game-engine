#include "./theatre_file.hpp"

using namespace TheatreFile;

bool TheatreFile::gDebugPrintLexerLogs{false},
    TheatreFile::gDebugPrintParserLogs{false},
    TheatreFile::gDebugDontPrintWhitespaceInLexerLogs{false},
    TheatreFile::gDebugDontPrintCommentsInLexerLogs{false};

static void sDebugPrintLexerLogs(Farg<TokenArray>);

std::string TheatreFile::TheatreData::get_log() const
{
    std::string _out{std::format("TheatreData [{}, {}]\n", name, index)};
    for(FAUTO _data : data)
        { _out += std::format("{}\n", _data.get_log()); }
    return _out;
}

std::string TheatreFile::TheatreData::get_parsable_string() const
{
    std::string _out{std::format("@{}#{}\n\n", name, index)};
    for(FAUTO [type, super] : type_declarations)
        { _out += std::format("declare {} inherits {}\n", type, super); }
    for(FAUTO _data : data)
        { _out += std::format("{}\n", _data.get_parsable_string()); }
    return _out;
}

Error TheatreFile::Load(std::string& ioPathToFile, Shared<TheatreData> outData)
{
    FileData theatre_file{};
    if(not theatre_file.LoadFile(ioPathToFile))
        { return ERR_FILE_LOAD; }
    ioPathToFile = theatre_file.filepath();
    return Load(theatre_file, outData);
}

Error TheatreFile::Load(Farg<FileData> inFileData, Shared<TheatreData> outData)
{
    TokenArray tokens{};
    Error lexer_code{Lex(inFileData, tokens)};
    if(gDebugPrintLexerLogs)
        { sDebugPrintLexerLogs(tokens); }
    if(!print_error_enum(lexer_code))
        { return ERR_THEATRE_LEXER; }

    Error parser_code{Parse(tokens, outData)};
    if(gDebugPrintParserLogs)
        { print_debug("Parser Output\n{}\n", outData->get_log()); }
    if(!print_error_enum(parser_code))
        { return ERR_THEATRE_PARSER; }
    return OK;
}

void sDebugPrintLexerLogs(Farg<TokenArray> inTokens)
{
    enum class Comment : int { SINGLE, MULTI, NO_COMMENT };
    Comment comment{Comment::NO_COMMENT};

    print_debug("Lexer Output");
    for(FAUTO token : inTokens)
    {
        if(token.category == TheatreFile::TokenName::Whitespace)
        {
            if(token.token[0] == '\n' and comment == Comment::SINGLE)
                { comment = Comment::NO_COMMENT; }
            if(gDebugDontPrintWhitespaceInLexerLogs)
                { continue; }
        }
        if(token.category == TheatreFile::TokenName::SinglelineComment)
        {
            if(comment == Comment::NO_COMMENT)
                { comment = Comment::SINGLE; }
            if(gDebugDontPrintCommentsInLexerLogs)
                { continue; }
        }
        if(token.category == TheatreFile::TokenName::MultilineComment)
        {
            if(comment == Comment::NO_COMMENT)
                { comment = Comment::MULTI; }
            else if(comment == Comment::MULTI)
                { comment = Comment::NO_COMMENT; }
            if(gDebugDontPrintCommentsInLexerLogs)
                { continue; }
        }
        if(gDebugDontPrintCommentsInLexerLogs and comment != Comment::NO_COMMENT)
            { continue; }
        debug_print("\tToken [{}, '{}']",
            EnumRegistry::GetEnumName(token.category),
            (token.token[0] == '\n')
                ? "\\n"
                : token.token);
    }
}
