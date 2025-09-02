#include "theatre_parser.hpp"

#include <set>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string>

// NOTE: Yes, this is stupid. Too bad!
#ifdef DEBUGGING
    int g_BreakOnColumn = -1;
    int g_BreakOnLine   = -1;

    size_t g_Column = 1;
    size_t g_Line   = 1;

#   define COLUMN   g_Column
#   define LINE     g_Line
#else  // !DEBUGGING
    size_t s_Column = 1;
    size_t s_Line   = 1;

#   define COLUMN   s_Column
#   define LINE     s_Line
#endif // DEBUGGING

static const std::string s_cParserLocation = std::to_string(LINE) + ":" + std::to_string(COLUMN);
static const std::string s_cSyntaxErrorPrefix   = " NostalgiaTheatre Syntax Error at "   + s_cParserLocation;
static const std::string s_cSyntaxWarningPrefix = " NostalgiaTheatre Syntax Warning at " + s_cParserLocation;

#define SYNTAX_ERROR(FORMAT, ARGS...)   __LABELLED_PRINT(__ERROR   + s_cSyntaxErrorPrefix,   FORMAT, ARGS)
#define SYNTAX_WARNING(FORMAT, ARGS...) __LABELLED_PRINT(__WARNING + s_cSyntaxWarningPrefix, FORMAT, ARGS)

// https://stackoverflow.com/a/313990
static constexpr std::string ToLower(std::string string)
{
    std::transform(string.begin(), string.end(), string.begin(),
        [](unsigned char character){ return std::tolower(character); });
    return string;
}

namespace fs = std::filesystem; // NOTE: Fuck you, I'm not typing allat

static std::string s_TheatreFileDataString = "";
static TheatreData s_TheatreData;

static const std::set<std::string> c_NostalgiaExtensions =
{
    ".theatre",
    ".t",
    ".nostalgiatheatre",
    ".nt"
};

// TODO: Add support for GraphXTheatre files
static const std::set<std::string> c_GraphXExtensions =
{
    ".graphxtheatre",
    ".gt",
};

static const char enter_context        = '{';
static const char enter_numeric        = '(';
static const char enter_reference      = '<';
static const char enter_engine_ref     = '[';
static const char enter_exit_string    = '\'';
static const char exit_context         = '}';
static const char exit_numeric         = ')';
static const char exit_reference       = '>';
static const char exit_engine_ref      = ']';
static const char name_delimiter       = '@';
static const char index_delimiter      = '#';
static const char assignment_delimiter = '=';
static const char sandwich_delimiter   = ':';
static const char comment_delimiter    = '/';
static const char newline              = '\n';
static const char tab                  = '\t';
static const char space                = ' ';
static const std::string resources     = "Resources";
static const std::string things        = "Things";

enum class Location
{
    TopLevel = 0,
    Context  = 1,
    Object   = 2,
    Sandwich = 3,
};

enum class Defining
{
    Name     = -2,
    Index    = -1,
    Context  = 0,
    Object   = 1,
    Variable = 2,
};

enum class Parsing
{
    Nothing       =  0,
    ObjectType    =  1,
    ObjectName    =  2,
    VariableName  =  3,
    VariableValue =  4,
};

#ifdef DEBUGGING
void debug_PrintParsedTheatreData()
{
    PRINT_DEBUG("Parsed Theatre Data Printout:")

    SafeReturn<TheatreData> try_data = TheatreParser::try_GetTheatreData();
    if(!SafeStatus::PrintCheck(try_data.Status()))
    { return; }

    std::set<data_t> temp_data = try_data.Data().GetMergedData();

    for(const auto& data : temp_data)
    {
        std::print("({}) {}:\n", data.m_Type, data.m_Name);

        const std::set<variable_t>& variables = data.m_Variables;
        for(const auto& variable : variables)
        { std::print("\t({}) {} = {}\n", StringifyEnum(variable.m_Type), variable.m_Name, variable.m_Value); }

        std::print("\n");
    }
}
#endif // DEBUGGING

SafeReturn<TheatreData> TheatreParser::try_GetTheatreData()
{
    // FIXME: Make this better
    return s_TheatreData;
}

SafeStatus TheatreParser::try_LoadTheatreFromFile(const std::string& theatre_file)
{
    if(!fs::exists(fs::path(theatre_file)))
    { return Status::TheatreParserFILE_DOES_NOT_EXIST; }

    if(!fs::path(theatre_file).has_extension() || !c_NostalgiaExtensions.contains(fs::path(theatre_file).extension().string()))
    { return Status::TheatreParserWRONG_FILE_EXTENSION; }

    std::ifstream theatre_file_stream(theatre_file);
    s_TheatreFileDataString = std::string(std::istreambuf_iterator<char>(theatre_file_stream), std::istreambuf_iterator<char>());
    theatre_file_stream.close();

    return Status::NO_ERR;
}

void TheatreParser::LoadTheatreFromMemory(const std::string& theatre_data)
{ s_TheatreFileDataString = theatre_data; }

SafeStatus TheatreParser::try_ParseTheatre()
{
    s_TheatreData.clear();

    COLUMN = 1;
    LINE   = 1;

    size_t data_size = s_TheatreFileDataString.size();

    char character = '\0';
    std::string buffer = "";

    Location location = Location::TopLevel;
    Defining defining = Defining::Name;
    Parsing  parsing  = Parsing::Nothing;

    bool inside_comment   = false;
    bool inside_string    = false;
    bool invalid_context  = false;
    bool set_new_line     = false;

    std::string theatre_name  = "";
    std::string theatre_index = "";

    std::string  variable_name  = "";
    std::string  variable_value = "";
    VariableType variable_type  = VariableType::Nothing;

    std::string sandwich_variable_name = "";

    data_t temp_data;
    data_t temp_data_swap;

    for(size_t iterator = 0 ; iterator < data_size ; ++iterator,++COLUMN)
    {
        character = s_TheatreFileDataString.at(iterator);

        if(set_new_line)
        {
            ++LINE;
            COLUMN = 1;
            set_new_line = false;
        }

#ifdef DEBUGGING
        // Setting a breakpoint after the 'if' statement here lets me break on a specific line and column
        if(LINE == g_BreakOnLine && COLUMN == g_BreakOnColumn)
            { PRINT_DEBUG("Break opportunity at line {}, column {}", LINE, COLUMN) }
#endif

        set_new_line = (character == '\n');

        // Temporarily bypass comment/string/context skipping on certain conditions
        // TODO: Make this shit less bad
        switch(character)
        {
        case exit_context:
        {
            if(!invalid_context)
                { break; }

            size_t temp_it = iterator;
            bool exit_while = false;
            bool check_word = false;
            std::string temp_buffer = "";
            while(!exit_while && temp_it < data_size)
            {
                char temp_character = s_TheatreFileDataString.at(++temp_it);
                switch(temp_character)
                {
                case ' ':
                case '\n':
                case '\t':
                    exit_while = check_word;
                    break;
                default:
                    check_word = true;
                    temp_buffer += temp_character;
                    break;
                }
            }
            if(!temp_buffer.compare(things) || !temp_buffer.compare(resources))
            {
                invalid_context = false;
                break;
            }
            continue;
        }
        case newline:
            inside_comment = false;
            break;
        case enter_exit_string:
            if(inside_comment)
                { break; }
            inside_string = !inside_string;
            if(inside_string)
                { variable_type = VariableType::String; }
            break;
        }

        if(inside_comment || invalid_context)
            { continue; }

        if(inside_string && character != enter_exit_string)
        {
            buffer += character;
            continue;
        }

        switch(character)
        {
        case comment_delimiter:
            if(iterator+1 < data_size && s_TheatreFileDataString.at(++iterator))
                { inside_comment = true; }
            continue;
        case name_delimiter:
            if(location == Location::TopLevel)
            {
                defining = Defining::Name;
                buffer.clear();
                continue;
            }
            break;
        case index_delimiter:
            if(location == Location::TopLevel)
            {
                defining = Defining::Index;
                theatre_name = buffer;
                buffer.clear();
                continue;
            }
            break;
        case newline:
            inside_comment  = false;
            if(location == Location::TopLevel)
            {
                if(defining == Defining::Name)
                {
                    theatre_name = buffer;
                    defining = Defining::Index;
                    buffer.clear();
                }

                else if(defining == Defining::Index)
                {
                    theatre_index = buffer;
                    defining = Defining::Context;
                    buffer.clear();
                }
            }
            continue;
        case tab:
        case space:
            if(buffer.empty())
                { continue; }
            switch(parsing)
            {
            case Parsing::ObjectType:
                temp_data.SetType(buffer);
                parsing = Parsing::ObjectName;
                buffer.clear();
                break;
            case Parsing::VariableName:
                variable_name = buffer;
                parsing = Parsing::VariableValue;
                buffer.clear();
                break;
            default:
                break;
            }
            continue;
        case assignment_delimiter:
            parsing = Parsing::VariableValue;
            continue;
        case sandwich_delimiter:
        {
            location = Location::Sandwich;
            parsing = Parsing::ObjectType;
            temp_data_swap = temp_data;
            sandwich_variable_name = variable_name;
            variable_name.clear();
            temp_data.clear();
            continue;
        }
        case enter_reference:
            variable_type = VariableType::TheatreRef;
            continue;
        case enter_engine_ref:
            variable_type = VariableType::EngineRef;
            continue;
        case enter_numeric:
            variable_type = VariableType::Number;
            continue;
        case enter_exit_string:
            if(inside_string)
                { continue; }
        case exit_numeric:
            if
            (
                variable_type == VariableType::Number &&
                buffer.size() >= 4 && // Has to be as large as "true"
                (
                    !ToLower(buffer).compare("false") ||
                    !ToLower(buffer).compare("true")
                )
            )
                { variable_type = VariableType::Bool; }
            [[fallthrough]];
        case exit_reference:
        case exit_engine_ref:
            variable_value = buffer;
            buffer.clear();
            temp_data.SetVariable(variable_name, variable_value, variable_type);
            parsing = Parsing::VariableName;
            continue;
        case enter_context:
            if(location == Location::TopLevel)
            {
                defining = Defining::Object;
                parsing  = Parsing::ObjectType;

                if(!buffer.compare(resources) || !buffer.compare(things))
                {
                    location = Location::Context;
                    invalid_context = false;
                }
                else
                {
                    SYNTAX_ERROR("Invalid context name: '{}'. Theatre parsing will be aborted", buffer)
                    invalid_context = true;
                }
                buffer.clear();
                continue;
            }
            if(location != Location::Sandwich)
                { location = Location::Object; }
            if(parsing == Parsing::ObjectName)
            {
                temp_data.SetName(buffer);
                buffer.clear();
                defining = Defining::Variable;
                parsing  = Parsing::VariableName;
            }
            continue;
        case exit_context:
            buffer.clear();
            switch(location)
            {
            case Location::TopLevel:
                SYNTAX_WARNING("Extraneous '{}' (will be ignored)", "}")
                continue;
            case Location::Context:
                defining = Defining::Context;
                location = Location::TopLevel;
                parsing  = Parsing::Nothing;
                continue;
            case Location::Object:
                location = Location::Context;
                defining = Defining::Object;
                parsing  = Parsing::ObjectType;
                break;
            case Location::Sandwich:
                defining = Defining::Variable;
                location = Location::Object;
                parsing  = Parsing::VariableName;

                s_TheatreData.AddData(temp_data);

                std::string sandwich_variable_value = temp_data.GetName();
                temp_data = temp_data_swap;
                temp_data.SetVariable(sandwich_variable_name, sandwich_variable_value, VariableType::TheatreRef);

                temp_data_swap.clear();
                sandwich_variable_name.clear();
                continue;
            }

            s_TheatreData.AddData(temp_data);
            temp_data.clear();
            variable_name.clear();
            variable_value.clear();

            continue;
        }

        buffer += character;
    }

#ifdef DEBUGGING
    debug_PrintParsedTheatreData();
#endif // DEBUGGING

    return Status::NO_ERR;
}
