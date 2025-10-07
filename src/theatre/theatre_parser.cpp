#include "theatre_parser.hpp"
#include "debug.hpp"
#include "to_lower.hpp"
#include "filesystem/filesystem.hpp"

#include <string>
#include <set>

// Yes, this is stupid. Too bad!"
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

static bool s_PlayerInstantiated = false;
static std::map<std::string, std::string> s_NameIDMap = {};
static std::string sTheatreFileDataString = "";

static const std::set<std::string> c_NostalgiaExtensions =
{
    ".theatre",
    ".t",
    ".nostalgiatheatre",
    ".nt"
};

// TODO: Add support for GraphXTheatre files?
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
static const char singleline_delimiter = ';';
static const char comment_delimiter    = '/';
static const char newline              = '\n';
static const char tab                  = '\t';
static const char space                = ' ';
static const std::string resources     = "Resources";
static const std::string things        = "Things";

SafeStatus TheatreParser::try_LoadTheatreFromFile(TheatreData& output, const std::string& theatre_file)
{
    if(!FileSystem::IsFile(theatre_file))
        { return Status::TheatreParserFILE_DOES_NOT_EXIST; }
    else if(!c_NostalgiaExtensions.contains(FileSystem::GetExtension(theatre_file)))
        { return Status::TheatreParserWRONG_FILE_EXTENSION; }
    else if(!FileSystem::try_ReadFileToString(theatre_file, sTheatreFileDataString))
        { return Status::ERROR_FILE_READ_ERROR; }
    return try_ParseTheatre(output);
}

SafeStatus TheatreParser::try_LoadTheatreFromMemory(TheatreData& output, const std::string& theatre_data)
{
    sTheatreFileDataString = theatre_data;
    return try_ParseTheatre(output);
}

static void s_AddParsedDataHelper(TheatreData& theatre_data, ThingData& thing_data)
{
    if(thing_data.type() == ThingType::NostalgiaPlayer)
        { s_PlayerInstantiated = true; thing_data.uid = UniqueIDs::Reserved::Player; }
    else
        { thing_data.uid = UniqueIDs::Generate(); }
    theatre_data.AddData(thing_data);
    s_NameIDMap[thing_data.name] = std::to_string(thing_data.uid);
}

enum class Context
{
    None,
    Things,
    Resources
};

enum class Location
{
    TopLevel,
    Context,
    Object,
    Sandwich
};

enum class Defining
{
    Name,
    Index,
    Context,
    Object,
    Variable
};

enum class Parsing
{
    Nothing,
    ObjectType,
    ObjectName,
    VariableName,
    VariableValue
};

SafeStatus TheatreParser::try_ParseTheatre(TheatreData& output)
{
#   pragma message("TODO: If/when I decide to make Theatres an object and allow for multiple Theatres to be loaded, I need to give each Theatre a unique set of IDs (which they will manage)")
    UniqueIDs::Clear();

    s_NameIDMap.clear();
    TheatreData theatre_data;

    COLUMN = 1;
    LINE   = 1;

    size_t data_size = sTheatreFileDataString.size();

    char character = '\0';
    std::string buffer = "";

    Location location = Location::TopLevel;
    Defining defining = Defining::Name;
    Parsing  parsing  = Parsing::Nothing;
    Context  context  = Context::None;

    bool inside_comment   = false;
    bool inside_string    = false;
    bool invalid_context  = false;
    bool set_new_line     = false;

    std::string theatre_name  = "";
    std::string theatre_index = "";

    std::string  variable_name  = "";
    std::string  variable_value = "";
    penum_t      variable_type  = ThingVar::eNothing;

    std::string sandwich_variable_name = "";

    ThingData temp_data;
    ThingData temp_data_swap;

    for(size_t iterator = 0 ; iterator < data_size ; ++iterator,++COLUMN)
    {
        character = sTheatreFileDataString.at(iterator);

        if(set_new_line)
        {
            ++LINE;
            COLUMN = 1;
            set_new_line = false;
        }

#   ifdef DEBUGGING
        // Setting a breakpoint after the 'if' statement here lets me break on a specific line and column
        if(LINE == g_BreakOnLine && COLUMN == g_BreakOnColumn)
            { PRINT_DEBUG("Break opportunity at line {}, column {}", LINE, COLUMN) }
#   endif // DEBUGGING

        set_new_line = (character == '\n');

        // Temporarily bypass comment/string/context skipping on certain conditions
        #pragma message("TODO: Make this shit less bad")
        // TODO: Expanding on the message above, figure out how to remove this secondary switch statement (yes, it comes
        // first but it's still secondary)
        switch(character)
        {
        case newline:
            inside_comment = false;
            break;
        case enter_exit_string:
            if(inside_comment)
                { break; }
            inside_string = !inside_string;
            if(inside_string)
                { variable_type = ThingVar::eString; }
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
            if(iterator+1 < data_size && sTheatreFileDataString.at(++iterator))
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
        case tab:
        case space:
            if(buffer.empty())
                { continue; }
            switch(parsing)
            {
            case Parsing::ObjectType:
                temp_data.set_type(buffer);
                parsing = Parsing::ObjectName;
                buffer.clear();
                break;
            case Parsing::VariableName:
                variable_name = buffer;
                parsing = Parsing::VariableValue;
                buffer.clear();
                break;
            case Parsing::ObjectName:
                temp_data.name = buffer;
                buffer.clear();
                defining = Defining::Variable;
                parsing  = Parsing::VariableName;
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
        case enter_engine_ref:
            variable_type = ThingVar::eReference;
            continue;
        case enter_numeric:
            variable_type = ThingVar::eNumber;
            continue;
        case enter_exit_string:
            if(inside_string)
                { continue; }
            if(context == Context::Resources)
                { variable_name = "File"; }
        case exit_numeric:
            if
            (
                variable_type == ThingVar::eNumber &&
                buffer.size() >= 4 && // Has to be as large as "true"
                (
                    !ToLower(buffer).compare("false") ||
                    !ToLower(buffer).compare("true")
                )
            )
            {
                variable_type = ThingVar::eBool;
                buffer = ToLower(buffer);
            }
            [[fallthrough]];
        case exit_reference:
        case exit_engine_ref:
            variable_value = buffer;
            buffer.clear();
            temp_data.AddVariable(variable_name, variable_value, variable_type);
            if(context != Context::Resources)
                { parsing = Parsing::VariableName; }
            else
                { parsing = Parsing::ObjectType; }
            continue;
        case enter_context:
            if(location == Location::TopLevel)
            {
                defining = Defining::Object;
                parsing  = Parsing::ObjectType;

                if(!buffer.compare(resources))
                {
                    context = Context::Resources;
                    location = Location::Context;
                    invalid_context = false;
                }
                else if(!buffer.compare(things))
                {
                    context = Context::Things;
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
            continue;
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
        case singleline_delimiter:
            location = Location::Object;
            std::print("{}, {}\n", variable_name, variable_value);
            [[fallthrough]];
        case exit_context:
            buffer.clear();
            switch(location)
            {
            case Location::TopLevel:
                SYNTAX_WARNING("Extraneous '{}' (will be ignored)", "}")
                continue;
            case Location::Context:
                context  = Context::None;
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

                s_AddParsedDataHelper(theatre_data, temp_data);

                std::string sandwich_variable_value = temp_data.name;
                temp_data = temp_data_swap;
                temp_data.AddVariable(sandwich_variable_name, sandwich_variable_value, ThingVar::eReference);

                temp_data_swap.clear();
                sandwich_variable_name.clear();
                continue;
            }

            s_AddParsedDataHelper(theatre_data, temp_data);

            temp_data.clear();
            variable_name.clear();
            variable_value.clear();
            continue;
        }

        buffer += character;
    }

    if(!s_PlayerInstantiated)
        { theatre_data.AddData(ThingData::PlayerDefaults); }

    theatre_data.UpdateReferences(s_NameIDMap);
    theatre_data.OrderByPriority();

#ifdef DEBUGGING
    theatre_data.debug_PrintData();
#endif // DEBUGGING
    output = theatre_data;
    theatre_data.clear();
    return Status::NO_ERR;
}
