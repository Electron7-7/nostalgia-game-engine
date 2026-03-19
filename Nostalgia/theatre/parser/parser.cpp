#include "../theatre_file.hpp"
#include "things/thing_factory.hpp"

// These preprocessor definitions are used on unknown/invalid types to approximate their closest inherited type
#define ACTOR_VAR_NAMES "Position", "Origin", "Scale", "Size", "OuuughImSoBigAndRound", "RotationDegrees", "Rotation", "RotationRadians"
#define ACTOR3D_VAR_NAMES "EulerDegrees", "Euler", "EulerRadians", "Quaternion"
#define RESOURCE_VAR_NAMES "File", "Data", "Path", "Font", "Model", "Image", "Image0", "Image1", "DiffuseTexture", "SpecularTexture", "FullBright"

static constexpr char cDelimiterTheatreName    {'@'};
static constexpr char cDelimiterTheatreIndex   {'#'};
static constexpr char cDelimiterStartSandwich  {':'};
static constexpr char cDelimiterExitContext    {'}'};
static constexpr char cDelimiterEnterNumber    {'['};
static constexpr char cDelimiterExitNumber     {']'};
static constexpr char cDelimiterEnterEnum      {'('};
static constexpr char cDelimiterEnterRefOrEnum {'<'};
static constexpr char cDelimiterWeakString     {'"'};
static constexpr char cDelimiterStrongString   {'\''};
static constexpr const char* cKeywordDeclare   {"declare"};

using namespace TheatreFile;

enum Comment { SINGLE_LINE, MULTI_LINE, NO_COMMENT }; // yes, `NO_COMMENT` is a pun

static std::map<std::string, std::string> s_ThingTypeForwardDeclarations{};

static void s_ParseDeclaration(size_t&, Farg<TokenArray>);
static TheatreFile::ThingData s_ParseThing(size_t&, Farg<TokenArray>, Shared<TheatreFile::TheatreData>);
static bool s_CheckIfComment(Comment&, Farg<Token>);

Error TheatreFile::Parse(Farg<TokenArray> inTokens, Shared<TheatreData>& outData)
{
    ThingData     thing_dat{};
    ThingVariable thing_var{};

    Comment in_comment{NO_COMMENT};

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
            { s_ParseDeclaration(i, inTokens); }
        else if(token.category == TokenName::Identifier)
            { s_ParseThing(i, inTokens, outData); }
    }
    return OK;
}

void s_ParseDeclaration(size_t& inIndex, Farg<TokenArray> inTokens)
{
    std::string new_type{},
        base_type{};
    while(inIndex < inTokens.size() and (new_type.empty() or base_type.empty()))
    {
        FAUTO token{inTokens.at(++inIndex)};
        if(token.category == TokenName::Identifier)
        {
            if(new_type.empty())
                { new_type = token.token; }
            else if(base_type.empty())
                { base_type = token.token; }
        }
    }
    s_ThingTypeForwardDeclarations[new_type] = base_type;
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

TheatreFile::ThingData s_ParseThing(size_t& inIndex,
    Farg<TokenArray> inTokens,
    Shared<TheatreFile::TheatreData> outData)
{
    TheatreFile::ThingData thing_data{};
    ThingVariable thing_var{};

    Comment in_comment{NO_COMMENT};

    bool in_literal{false},
        in_string{false};

    for(; inIndex < inTokens.size(); ++inIndex)
    {
        FAUTO token{inTokens.at(inIndex)};
        if(s_CheckIfComment(in_comment, token))
            { continue; }
        else if((in_literal and token.token[0] != cDelimiterExitNumber)
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
                ++inIndex;
                for(; inIndex < inTokens.size() and !exit_for; ++inIndex)
                {
                    next_token = inTokens.at(inIndex);
                    if(next_token.category != TokenName::Keyword and
                        next_token.category != TokenName::Identifier)
                            { continue; }
                    thing_var.name = next_token.token;
                    for(size_t i{inIndex+1}; i < inTokens.size() and !exit_for; ++i)
                    {
                        next_token = inTokens.at(i);
                        if(next_token.category != TokenName::Identifier)
                            { continue; }
                        thing_var.value = next_token.token;
                        exit_for = true;
                    }
                    --inIndex;
                }
                thing_data.children_variables.push_back(thing_var);
                thing_var.clear();
                s_ParseThing(inIndex, inTokens, outData);
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
            case cDelimiterEnterRefOrEnum:
                if(thing_var.type == ThingVarType::None)
                    { thing_var.type = ThingVarType::ID; }
                break;
            case cDelimiterEnterEnum:
                thing_var.type = ThingVarType::Enum;
                break;
            case cDelimiterEnterNumber:
                in_literal = true;
                thing_var.type = ThingVarType::Number;
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
            case cDelimiterExitContext:
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
                outData->push_back(thing_data);
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
                else if(auto found_it{s_ThingTypeForwardDeclarations.find(token.token)};
                    found_it != s_ThingTypeForwardDeclarations.end())
                {
                    thing_data.type = found_it->second;
                    print_warning("ThingType '{}' wasn't loaded, but a forward declaration was found. A '{}' will be made in its place",
                        found_it->first, found_it->second);
                }
                else
                    { thing_data.type = token.token; }
            }
            else if(thing_data.name.empty())
                { thing_data.name = token.token; }
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
