#include "../theatre_file.hpp"
#include "things/thing_factory.hpp"
#include "console/console.hpp"
#include "thirdparty/frozen/map.h"
#include "thirdparty/frozen/string.h"

// These preprocessor definitions are used on unknown/invalid types to approximate their closest inherited type
#define ACTOR_VAR_NAMES "Position", "Origin", "Scale", "Size", "OuuughImSoBigAndRound", "RotationDegrees", "Rotation", "RotationRadians"
#define ACTOR3D_VAR_NAMES "EulerDegrees", "Euler", "EulerRadians", "Quaternion"
#define RESOURCE_VAR_NAMES "File", "Data", "Path", "Font", "Model", "Image", "Image0", "Image1", "DiffuseTexture", "SpecularTexture", "FullBright"

static constexpr frozen::map<frozen::string, frozen::string, 2>
sNamedNumbers{
    {"ALL",  "111111111111111111111111111111"},
    {"NONE", "000000000000000000000000000000"},
};
static constexpr const char* cKeywordDeclare{"declare"};
static constexpr char
    cDelimiterTheatreName      {'@'},
    cDelimiterTheatreIndex     {'#'},
    cDelimiterNamedNumber      {'#'},
    cDelimiterStartSandwich    {':'},
    cDelimiterEnterDefinition  {'{'},
    cDelimiterExitDefinition   {'}'},
    cDelimiterEnterNumber      {'['},
    cDelimiterExitNumber       {']'},
    cDelimiterEnterExitBitMask {'|'},
    cDelimiterEnterEnum        {'('},
    cDelimiterExitEnum         {')'},
    cDelimiterEnterReference   {'<'},
    cDelimiterExitReference    {'>'},
    cDelimiterWeakString       {'"'},
    cDelimiterVectorComponent  {','},
    cDelimiterStrongString     {'\''};

using namespace TheatreFile;

enum Comment { SINGLE_LINE, MULTI_LINE, NO_COMMENT }; // yes, `NO_COMMENT` is a pun

static std::map<std::string, PID> s_ThingNameToTypeID{};

static void s_SetupNamesAndTypes(Farg<TokenArray>, TheatreFile::TheatreData&);
static void s_ParseDeclaration(size_t&, Farg<TokenArray>, TheatreFile::TheatreData&);
static TheatreFile::ThingData s_ParseThing(size_t&, Farg<TokenArray>, TheatreFile::TheatreData&);
static bool s_CheckIfComment(Comment&, Farg<Token>);

Error TheatreFile::Parse(Farg<TokenArray> inTokens, Shared<TheatreData> outData)
{
    ThingData     thing_dat{};
    ThingVariable thing_var{};

    Comment in_comment{NO_COMMENT};

    s_ThingNameToTypeID.clear();

    s_SetupNamesAndTypes(inTokens, *outData);

    if(Console::GetVariable("TheatreFile.Parser.print_declarations").int_value)
    {
        print_debug("Parser Forward Declarations:");
        for(FAUTO [name, type] : outData->type_declarations)
            { debug_print("\t[ {} == {} ]", name, type); }
    }

    if(Console::GetVariable("TheatreFile.Parser.print_name_type_map").int_value)
    {
        print_debug("Parser Name to TypeID Map:");
        for(FAUTO [name, type] : s_ThingNameToTypeID)
            { debug_print("\t[ {} == {} ]", name, type.name()); }
    }

    for(size_t i{0}; i < inTokens.size(); ++i)
    {
        Farg<Token> token{inTokens.at(i)};
        if(s_CheckIfComment(in_comment, token))
            { continue; }
        else if(token.category == TokenName::Separator
            and i+1 < inTokens.size())
        {
            if(token.token[0] == cDelimiterTheatreName)
                { outData->name = inTokens.at(++i).token; }
            else if(token.token[0] == cDelimiterTheatreIndex)
            {
                try { outData->index = std::stol(inTokens.at(++i).token); }
                catch(std::invalid_argument const& e) {}
                catch(std::out_of_range const& e) {}
            }
        }
        else if(token.category == TokenName::Keyword and not token.token.compare(cKeywordDeclare))
        {
            while(++i < inTokens.size())
            {
                FAUTO _token{inTokens.at(i)};
                if(_token.category == TokenName::Whitespace and _token.token[0] == '\n')
                    { break; }
            }
        }
        else if(token.category == TokenName::Identifier)
            { s_ParseThing(i, inTokens, *outData); }
    }
    return OK;
}

void s_SetupNamesAndTypes(Farg<TokenArray> inTokens, TheatreFile::TheatreData& outData)
{
    std::string _type{};
    Comment in_comment{NO_COMMENT};
    bool _skip_definition{false}, _in_sandwich{false}, _in_enum{false};

    for(size_t i{0}; i < inTokens.size(); ++i)
    {
        Token _token{inTokens.at(i)};
        if(s_CheckIfComment(in_comment, _token))
            { continue; }
        else if(_token.token[0] == cDelimiterEnterEnum)
            { _in_enum = true; }
        else if(_token.token[0] == cDelimiterExitEnum)
            { _in_enum = false; }
        else if(_token.token[0] == cDelimiterStartSandwich and not _in_enum)
            { _in_sandwich = true; }
        else if(_token.token[0] == cDelimiterEnterDefinition)
            { _skip_definition = true; }
        else if(_token.token[0] == cDelimiterExitDefinition)
            { _skip_definition = false; }
        else if(_skip_definition and not _in_sandwich)
            { continue; }
        else if(_token.category == TokenName::Keyword and not _token.token.compare(cKeywordDeclare))
            { s_ParseDeclaration(i, inTokens, outData); }
        else if(_token.category == TokenName::Identifier)
        {
            if(ThingFactory::IsThing(_token.token))
                { _type = _token.token; }
            else if(auto found_it{outData.type_declarations.find(_token.token)};
                    found_it != outData.type_declarations.end())
                { _type = _token.token; }
            else
                { continue; }

            while(++i < inTokens.size())
            {
                _token = inTokens.at(i);
                if(_token.category == TokenName::Identifier)
                {
                    s_ThingNameToTypeID[_token.token] = _type;
                    _type.clear();
                    break;
                }
            }
            _in_sandwich = false;
            _skip_definition = true;
            continue;
        }
    }
}

void s_ParseDeclaration(size_t& ioIndex, Farg<TokenArray> inTokens, TheatreFile::TheatreData& outData)
{
    std::string new_type{}, base_type{};
    while(ioIndex < inTokens.size() and (new_type.empty() or base_type.empty()))
    {
        FAUTO token{inTokens.at(++ioIndex)};
        if(token.category == TokenName::Identifier)
        {
            if(new_type.empty())
                { new_type = token.token; }
            else if(base_type.empty())
                { base_type = token.token; }
        }
    }
    outData.type_declarations[new_type] = base_type;
    print_error_enum(ThingFactory::AddThingDeclaration(new_type, base_type, true));
}

bool s_CheckIfComment(Comment& ioComment, Farg<Token> inToken)
{
    switch(inToken.category)
    {
        case TokenName::SinglelineComment:
            if(ioComment == NO_COMMENT)
                { ioComment = SINGLE_LINE; }
            break;
        case TokenName::MultilineComment:
            if(ioComment == NO_COMMENT)
                { ioComment = MULTI_LINE; }
            else if(ioComment == MULTI_LINE)
                { ioComment = NO_COMMENT; }
            break;
        case TokenName::Whitespace:
            if(inToken.token[0] == '\n' and ioComment == SINGLE_LINE)
                { ioComment = NO_COMMENT; }
            break;
        default:
            break;
    }
    return ioComment != NO_COMMENT;
}

TheatreFile::ThingData s_ParseThing(size_t& ioIndex,
    Farg<TokenArray> inTokens,
    TheatreFile::TheatreData& outData)
{
    ThingData thing_data{};
    ThingVariable thing_var{};
    std::string _string_variable{};

    Comment in_comment{NO_COMMENT};

    int vector_size{1};
    bool in_literal{false},
        in_string{false},
        in_enum{false},
        is_parent{false},
        is_child{false};

    for(; ioIndex < inTokens.size(); ++ioIndex)
    {
        FAUTO token{inTokens.at(ioIndex)};
#ifdef NOSTALGIA_DEBUGGING
        if(token.break_on_me)
            { BREAK_HERE }
#endif // NOSTALGIA_DEBUGGING
        if(s_CheckIfComment(in_comment, token))
            { continue; }
        else if((in_literal
                and token.token[0] != cDelimiterExitNumber
                and token.token[0] != cDelimiterEnterExitBitMask
                and token.token[0] != cDelimiterVectorComponent)
            or (in_string
                and token.token[0] != cDelimiterStrongString
                and token.token[0] != cDelimiterWeakString)
            or (in_enum and token.token[0] != cDelimiterExitEnum))
            { _string_variable += token.token; continue; }
        switch(token.category)
        {
        case TokenName::SinglelineComment:
            [[fallthrough]];
        case TokenName::MultilineComment:
            [[fallthrough]];
        case TokenName::None:
            break;
        case TokenName::Operator:
            if(token.token[0] == cDelimiterStartSandwich)
            {
                Token next_token{};
                bool exit_for{false};
#pragma message("TODO: make this less shit")
                size_t _index{++ioIndex};
                for(; _index < inTokens.size() and !exit_for; ++_index)
                {
                    next_token = inTokens.at(_index);
                    if(next_token.category != TokenName::Keyword and
                        next_token.category != TokenName::Identifier)
                            { continue; }
                    ++_index;
                    for(; _index < inTokens.size() and !exit_for; ++_index)
                    {
                        next_token = inTokens.at(_index);
                        if(next_token.category != TokenName::Identifier)
                            { continue; }
                        thing_var.value = next_token.token;
                        exit_for = true;
                    }
                }
                thing_data.children_variables.push_back(thing_var);
                thing_var = {};
                _string_variable.clear();
                is_parent = false;
                is_child = false;
                s_ParseThing(ioIndex, inTokens, outData);
                continue;
            }
            break;
        case TokenName::Whitespace:
            if(!_string_variable.empty()
                and !in_string
                and !in_literal
                and token.token[0] == '\n')
            {
                if(is_child)
                    { thing_data.children_variables.push_back(thing_var); }
                else if(is_parent)
                    { thing_data.parent_variable = thing_var; }
                else
                    { thing_data.variables.push_back(thing_var); }
                thing_var = {};
                _string_variable.clear();
                is_parent = false;
                is_child = false;
            }
            break;
        case TokenName::Separator:
            switch(token.token[0])
            {
            case cDelimiterExitReference:
            {
                thing_var.value = _string_variable;
                break;
            }
            case cDelimiterEnterReference:
                break;
            case cDelimiterEnterEnum:
                in_enum = true;
                break;
            case cDelimiterExitEnum:
                in_enum = false;
                thing_var.value = _string_variable;
                thing_var.hint  = TheatreFile::VARIABLE_HINT_ENUM;
                break;
            case cDelimiterEnterExitBitMask:
                {
                    if(not in_literal and _string_variable.empty())
                    {
#pragma message("TODO: merge duplicate code")
                        if(ioIndex + 2 < inTokens.size())
                        {
                            FAUTO _first_token{inTokens.at(ioIndex + 1)};
                            FAUTO _second_token{inTokens.at(ioIndex + 2)};
                            if(_first_token.category == TokenName::Separator
                                and _first_token.token[0] == cDelimiterNamedNumber)
                            {
                                ++ioIndex;
                                if(auto found_it{sNamedNumbers.find(frozen::string{_second_token.token})};
                                    found_it != sNamedNumbers.end())
                                {
                                    ++ioIndex;
                                    _string_variable = found_it->second.data();
                                }
                                break;
                            }
                        }
                        in_literal = true;
                        break;
                    }
                    in_literal = false;
                    int _value{};
                    StringToNum(_value, _string_variable);
                    thing_var.value = _value;
                    break;
                }
            case cDelimiterVectorComponent:
                ++vector_size;
                _string_variable += token.token;
                break;
            case cDelimiterEnterNumber:
                if(ioIndex + 2 < inTokens.size())
                {
                    FAUTO _first_token{inTokens.at(ioIndex + 1)};
                    FAUTO _second_token{inTokens.at(ioIndex + 2)};
                    if(_first_token.category == TokenName::Separator
                        and _first_token.token[0] == cDelimiterNamedNumber)
                    {
                        ++ioIndex;
                        if(auto found_it{sNamedNumbers.find(frozen::string{_second_token.token})};
                            found_it != sNamedNumbers.end())
                        {
                            ++ioIndex;
                            _string_variable = found_it->second.data();
                        }
                        break;
                    }
                }
                in_literal = true;
                break;
            case cDelimiterExitNumber:
                in_literal = false;
                if(!_string_variable.empty())
                {
                    auto _val{_string_variable};
                    // https://stackoverflow.com/a/313990
                    std::transform(_val.begin(), _val.end(), _val.begin(),
                        [](unsigned char character){ return std::tolower(character); });
                    if(!_val.compare("false") || !_val.compare("true"))
                    {
                        thing_var.value = bool{_val == "true"};
                    }
                    else
                    {
                        switch(vector_size)
                        {
                        case 1:
                            if(IsInt(_string_variable))
                                { thing_var.value = StringToNum<int>(_string_variable); }
                            else if(IsFloat(_string_variable))
                                { thing_var.value = StringToNum<float>(_string_variable); }
                            break;
                        case 2:
                            thing_var.value = StringToNum<glm::vec2>(_string_variable);
                            break;
                        case 3:
                            thing_var.value = StringToNum<glm::vec3>(_string_variable);
                            break;
                        case 4:
                            if(GetLowercase(thing_var.name) == "quaternion")
                                { thing_var.value = StringToNum<glm::quat>(_string_variable); }
                            else
                                { thing_var.value = StringToNum<glm::vec4>(_string_variable); }
                            break;
                        }
                        vector_size = 1;
                    }
                }
                break;
            case cDelimiterWeakString:
            case cDelimiterStrongString:
                in_string = !in_string;
                if(not in_string)
                    { thing_var.value  = _string_variable; }
                break;
            case cDelimiterExitDefinition:
                if(not thing_var.invalid())
                {
                    if(is_child)
                        { thing_data.children_variables.push_back(thing_var); }
                    else if(is_parent)
                        { thing_data.parent_variable = thing_var; }
                    else
                        { thing_data.variables.push_back(thing_var); }
                }

                // This has to be done here and not when initially assigning the type PID, because it uses the
                // unknown Thing's variables to approximate its base type. It's not super accurate, but since this
                // is an error, anyways, I only really focus on a few more "important" types.
                if(not ThingFactory::IsThing(thing_data.type))
                {
                    print_warning("Invalid Thing discovered w/ type '{}'", thing_data.type.name());
                    thing_data.type = (thing_data.children_variables.empty() and
                        thing_data.parent_variable.invalid())
                            ? ThingType::Invalid
                            : ThingType::Thinker;

                    if(glm::vec2 actor2d_test{};
                        thing_data.get_variable(actor2d_test, ACTOR_VAR_NAMES) == OK)
                            { thing_data.type = ThingType::Actor2D; }
                    else if(glm::vec4 actor3d_test{};
                        thing_data.get_variable(actor3d_test, ACTOR3D_VAR_NAMES, ACTOR_VAR_NAMES) == OK)
                            { thing_data.type = ThingType::Actor3D; }
                    else if(glm::vec2 viewport_test{};
                        thing_data.get_variable(viewport_test, "ViewportSize", "ContentSize") == OK)
                            { thing_data.type = ThingType::Viewport; }
                    else if(std::string resource_test{};
                        thing_data.get_variable(resource_test, RESOURCE_VAR_NAMES) == OK)
                            { thing_data.type = ThingType::Resource; }
                    print_warning("Setting invalid type to '{}'", thing_data.type.name());
                }
                outData.push_back(thing_data);
                return thing_data;
            default:
                break;
            }
            break;
        case TokenName::Keyword:
            if(!token.token.compare("Child"))
            {
                is_child = true;
                thing_var.name = token.token;
            }
            else if(!token.token.compare("Parent"))
            {
                is_parent = true;
                thing_var.name = token.token;
            }
            break;
        case TokenName::Identifier:
            if(thing_data.type.invalid())
            {
                if(ThingFactory::IsThing(token.token))
                    { thing_data.type = token.token; }
                else if(auto found_it{outData.type_declarations.find(token.token)};
                    found_it != outData.type_declarations.end())
                {
                    thing_data.type = found_it->second;
                    print_warning("ThingType '{}' wasn't loaded, but a forward declaration was found. A '{}' will be made in its place",
                        found_it->first, found_it->second);
                }
                else
                    { thing_data.type = token.token; }
            }
            else if(thing_data.name.empty())
            {
                thing_data.name = token.token;
                while(ioIndex < inTokens.size())
                {
                    FAUTO _token{inTokens.at(++ioIndex)};
                    if(_token.category == TokenName::Whitespace
                        or _token.token[0] == cDelimiterEnterDefinition)
                        { break; }
                    thing_data.name += _token.token;
                }
                s_ThingNameToTypeID[thing_data.name] = thing_data.type;
            }
            else if(thing_var.name.empty())
                { thing_var.name = token.token; }
            else if(_string_variable.empty())
                { _string_variable = token.token; }
            break;
        default:
            break;
        }
    }
    return thing_data;
}

#undef ACTOR_VAR_NAMES
#undef ACTOR3D_VAR_NAMES
#undef RESOURCE_VAR_NAMES
