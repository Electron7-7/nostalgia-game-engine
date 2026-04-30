#include "../theatre_file.hpp"
#include "things/resource_database.hpp"
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
    cDelimiterEnterReference   {'<'},
    cDelimiterExitReference    {'>'},
    cDelimiterWeakString       {'"'},
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
                try { outData->index = std::stoi(inTokens.at(++i).token); }
                catch(std::invalid_argument const& e) {}
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
    bool _skip_definition{false}, _in_sandwich{false};

    for(size_t i{0}; i < inTokens.size(); ++i)
    {
        Token _token{inTokens.at(i)};
        if(s_CheckIfComment(in_comment, _token))
            { continue; }
        else if(_token.token[0] == cDelimiterStartSandwich)
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
    TheatreFile::ThingData thing_data{};
    ThingVariable thing_var{};

    Comment in_comment{NO_COMMENT};

    bool in_literal{false},
        in_string{false};

    for(; ioIndex < inTokens.size(); ++ioIndex)
    {
        FAUTO token{inTokens.at(ioIndex)};
        if(s_CheckIfComment(in_comment, token))
            { continue; }
        else if((in_literal
                and token.token[0] != cDelimiterExitNumber and token.token[0] != cDelimiterEnterExitBitMask)
            or (in_string
                and token.token[0] != cDelimiterStrongString
                and token.token[0] != cDelimiterWeakString))
            { thing_var.value += token.token; continue; }
        switch(token.category)
        {
        case TokenName::SinglelineComment:
            [[fallthrough]];
        case TokenName::MultilineComment:
            [[fallthrough]];
        case TokenName::None:
            break;
        case TokenName::Literal:
            if(!in_literal)
            {
                thing_var.type = ThingVarType::String;
                thing_var.value = token.token.substr(1, token.token.size() - 2);
            }
            else
                { thing_var.value += token.token; }
            break;
        case TokenName::Operator:
            if(token.token[0] == cDelimiterStartSandwich)
            {
                Token next_token{};
                bool exit_for{false};
#pragma message("TODO: make this less shit")
                ++ioIndex;
                for(; ioIndex < inTokens.size() and !exit_for; ++ioIndex)
                {
                    next_token = inTokens.at(ioIndex);
                    if(next_token.category != TokenName::Keyword and
                        next_token.category != TokenName::Identifier)
                            { continue; }
                    thing_var.name = next_token.token;
                    for(size_t i{ioIndex+1}; i < inTokens.size() and !exit_for; ++i)
                    {
                        next_token = inTokens.at(i);
                        if(next_token.category != TokenName::Identifier)
                            { continue; }
                        thing_var.value = next_token.token;
                        thing_var.thing_type = thing_var.name;
                        exit_for = true;
                    }
                    --ioIndex;
                }
                thing_data.children_variables.push_back(thing_var);
                thing_var.clear();
                s_ParseThing(ioIndex, inTokens, outData);
                continue;
            }
            break;
        case TokenName::Whitespace:
            if(!thing_var.value.empty()
                and !in_string
                and !in_literal
                and token.token[0] == '\n')
            {
                if(thing_var.type == ThingVarType::Child)
                    { thing_data.children_variables.push_back(thing_var); }
                else if(thing_var.type == ThingVarType::Parent)
                    { thing_data.parent_variable = thing_var; }
                else
                    { thing_data.variables.push_back(thing_var); }
                thing_var.clear();
            }
            break;
        case TokenName::Separator:
            switch(token.token[0])
            {
            case cDelimiterExitReference:
            {
                std::string _name{thing_var.value};
                if(thing_var.type != ThingVarType::ID and thing_var.name.compare("Child"))
                    { _name = thing_var.name; }
                if(auto found_it{s_ThingNameToTypeID.find(_name)}; found_it != s_ThingNameToTypeID.end())
                    { thing_var.thing_type = found_it->second; }
                else
                    { thing_var.thing_type = ResourceDatabase::TypeOf(_name); }
                break;
            }
            case cDelimiterEnterReference:
                if(thing_var.type == ThingVarType::None)
                    { thing_var.type = ThingVarType::ID; }
                break;
            case cDelimiterEnterEnum:
                thing_var.type = ThingVarType::Enum;
                break;
            case cDelimiterEnterExitBitMask:
                if(not in_literal and thing_var.value.empty())
                {
#pragma message("TODO: merge duplicate code")
                    thing_var.type = ThingVarType::BitMask;
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
                                thing_var.value = found_it->second.data();
                            }
                            break;
                        }
                    }
                    in_literal = true;
                    break;
                }
                in_literal = false;
                break;
            case cDelimiterEnterNumber:
                thing_var.type = ThingVarType::Number;
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
                            thing_var.value = found_it->second.data();
                        }
                        break;
                    }
                }
                in_literal = true;
                break;
            case cDelimiterExitNumber:
                in_literal = false;
                if(!thing_var.value.empty())
                {
                    auto _val{thing_var.value};
                    // https://stackoverflow.com/a/313990
                    std::transform(_val.begin(), _val.end(), _val.begin(),
                        [](unsigned char character){ return std::tolower(character); });
                    if(!_val.compare("false") || !_val.compare("true"))
                    {
                        thing_var.type = ThingVarType::Bool;
                        thing_var.value = _val;
                    }
                }
                break;
            case cDelimiterWeakString:
            case cDelimiterStrongString:
                in_string = !in_string;
                break;
            case cDelimiterExitDefinition:
                if(not thing_var.invalid())
                {
                    if(thing_var.type == ThingVarType::Child)
                        { thing_data.children_variables.push_back(thing_var); }
                    else if(thing_var.type == ThingVarType::Parent)
                        { thing_data.parent_variable = thing_var; }
                    else
                        { thing_data.variables.push_back(thing_var); }
                }

                // This has to be done here and not when initially assigning the type PID, because it uses the
                // unknown Thing's variables to approximate its base type. It's not super accurate, but since this
                // is an error, anyways, I only really focus on a few more "important" types.
                if(not ThingFactory::IsThing(thing_data.type))
                {
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
                }
                outData.push_back(thing_data);
                return thing_data;
            default:
                break;
            }
            break;
        case TokenName::Keyword:
            if(!token.token.compare("Child"))
                { thing_var.type = ThingVarType::Child; thing_var.name = token.token; }
            else if(!token.token.compare("Parent"))
                { thing_var.type = ThingVarType::Parent; thing_var.name = token.token; }
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
                s_ThingNameToTypeID[thing_data.name] = thing_data.type;
            }
            else if(thing_var.name.empty())
                { thing_var.name = token.token; }
            else if(thing_var.value.empty())
                { thing_var.value = token.token; }
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
