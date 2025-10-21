#include "theatre_parser.hpp"
#include "filesystem/filesystem.hpp"
#include "filesystem/file_data.hpp"
#include "common/string_to_num.hpp"
#include "common/string_compare.hpp"

#include <string>

// TODO: make a separate project for Theatre file syntax and include error/warning generation?

// Yes, this is stupid. Too bad!"
#ifdef DEBUGGING
    int g_BreakOnColumn = -1;
    int g_BreakOnLine   = -1;

    size_t g_Column = 1;
    size_t g_Line   = 1;

#   define COLUMN   g_Column
#   define LINE     g_Line
#else  // !DEBUGGING
    static size_t s_Column = 1;
    static size_t s_Line   = 1;

#   define COLUMN   s_Column
#   define LINE     s_Line
#endif // DEBUGGING

/*static const std::set<std::string> c_NostalgiaExtensions =
{
    ".theatre",
    ".t",
    ".nostalgiatheatre",
    ".nt"
};*/

FileData TheatreParser::m_sTheatreFile;

static const char enter_context        = '{';
static const char enter_numeric        = '(';
static const char enter_reference      = '<';
#pragma message("TODO: Implement `p_enum` as its own variable type")
static const char enter_pretty_enum    = '[';
static const char enter_exit_string    = '\'';
static const char exit_context         = '}';
static const char exit_numeric         = ')';
static const char exit_reference       = '>';
static const char exit_pretty_enum     = ']';
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

bool TheatreParser::ParseTheatreFile(const std::string& path, TheatreData& output)
{
    if(SafeStatus::PrintCheck(m_sTheatreFile.LoadFile(path)) && ReadTheatre(output))
        { output.file_path = path; return true; }
    return false;
}

bool TheatreParser::ParseTheatreFileFromMemory(const FileData& data, TheatreData& output)
{
    m_sTheatreFile = data;
    return ReadTheatre(output);
}

SafeReturn<std::string> TheatreParser::WriteTheatre(const TheatreData& data, const std::string& file_path)
{
    std::string final_path = (FileSystem::HasStem(file_path)) ? file_path : FileSystem::Directory(file_path) + FileSystem::ReplaceExtension(".nt", data.name);
    return SafeReturn(final_path, FileSystem::try_WriteFileFromString(final_path, data.formatted()));
}

bool TheatreParser::ReadTheatre(TheatreData& output)
{
#   pragma message("TODO: If/when I decide to make Theatres an object and allow for multiple Theatres to be loaded, I need a way of storing/clearing *only* that Theatre's IDs (probably as a range?)")

    ThingData temp_data;
    ThingData temp_data_swap;
    output.clear();

    COLUMN = 1;
    LINE   = 1;

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

    const unsigned char* data = m_sTheatreFile.Data();
    int data_size = m_sTheatreFile.Size();
    for(int iterator = 0 ; iterator < data_size ; ++iterator,++COLUMN)
    {
        if(set_new_line)
        {
            ++LINE;
            COLUMN = 1;
        }
        set_new_line = false;
        character = data[iterator];

#ifdef DEBUGGING
        if(LINE == g_BreakOnLine && COLUMN == g_BreakOnColumn)
            { print_debug("Breakpoint opportunity at line {}, column {}", LINE, COLUMN); }
#endif // DEBUGGING

#pragma message("TODO: Make this shit less bad")
        // TODO: Expanding on the message above, figure out how to remove this secondary switch statement (yes, it comes first but it's still secondary)
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
            if(iterator > 0 && (data[iterator - 1] == comment_delimiter))
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
        case enter_pretty_enum:
            variable_type = ThingVar::ePrettyEnum;
            continue;
        case enter_reference:
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
            if(variable_type == ThingVar::eNumber && gIsBool(buffer))
            {
                variable_type = ThingVar::eBool;
                gSetLowercase(buffer);
            }
            [[fallthrough]];
        case exit_pretty_enum:
        case exit_reference:
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
                    { invalid_context = true; }
                buffer.clear();
                continue;
            }
            if(location != Location::Sandwich)
                { location = Location::Object; }
            continue;
        case newline:
            set_new_line = true;
            inside_comment = false;
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
            [[fallthrough]];
        case exit_context:
            buffer.clear();
            switch(location)
            {
            case Location::TopLevel:
                print_warning("TheatreFile - Extraneous '{}' (will be ignored)", "}");
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

                output.AddData(temp_data);

                std::string sandwich_variable_value = temp_data.name;
                temp_data = temp_data_swap;
                temp_data.AddVariable(sandwich_variable_name, sandwich_variable_value, ThingVar::eReference);

                temp_data_swap.clear();
                sandwich_variable_name.clear();
                continue;
            }

            output.AddData(temp_data);
            temp_data.clear();
            variable_name.clear();
            variable_value.clear();
            continue;
        }
        buffer += character;
    }
    output.name = theatre_name;
    StringToNum<id_t>(output.index, theatre_index);
    return true;
}
