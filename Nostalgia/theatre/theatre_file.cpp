#include "./theatre_file.hpp"

using namespace TheatreFile;

bool TheatreFile::gDebugPrintLexerLogs{false},
    TheatreFile::gDebugPrintParserLogs{false},
    TheatreFile::gDebugDontPrintWhitespaceInLexerLogs{false},
    TheatreFile::gDebugDontPrintCommentsInLexerLogs{false};

static void sDebugPrintLexerLogs(Farg<TokenArray>);
static void sDebugPrintParserLogs(Farg<std::vector<ThingData>>);

Error TheatreFile::Load(std::string& ioPathToFile, Shared<TheatreData>& outData)
{
    FileData theatre_file{};
    if(not theatre_file.LoadFile(ioPathToFile))
        { return ERR_FILE_LOAD; }
    ioPathToFile = theatre_file.filepath();
    return Load(theatre_file, outData);
}

Error TheatreFile::Load(Farg<FileData> inFileData,
    Shared<TheatreData>& outData)
{
    TokenArray tokens{};
    Error lexer_code{Lex(inFileData, tokens)};
    if(gDebugPrintLexerLogs)
        { sDebugPrintLexerLogs(tokens); }
    if(!print_error_enum(lexer_code))
        { return ERR_THEATRE_LEXER; }

    Error parser_code{Parse(tokens, outData)};
    if(gDebugPrintParserLogs)
        { sDebugPrintParserLogs(outData->data); }
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

void sDebugPrintParserLogs(Farg<std::vector<ThingData>> inData)
{
    print_debug("Parser Output");
    for(FAUTO thing_data : inData)
    {
        debug_print("<ThingData>");
        debug_print("\ttype:   {}", thing_data.type.name());
        debug_print("\tname:   {}", thing_data.name);
        if(!thing_data.parent_variable.invalid())
            { debug_print("\tparent: {}", thing_data.parent_variable.get_log()); }
        for(FAUTO child : thing_data.children_variables)
            { debug_print("\tchild: {}", child.get_log()); }
        debug_print(""); // Extra newline
    }
}
