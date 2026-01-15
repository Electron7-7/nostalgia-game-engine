#include "parser.hpp"
#include "thing_factory.hpp"
#include "common/string_hash.hpp"

static constexpr char cDelimiterStartSandwich  {':'};
static constexpr char cDelimiterEndResource    {';'};
static constexpr char cDelimiterEnterContext   {'{'};
static constexpr char cDelimiterExitContext    {'}'};
static constexpr char cDelimiterEnterNumber    {'['};
static constexpr char cDelimiterExitNumber     {']'};
static constexpr char cDelimiterEnterEnum      {'('};
static constexpr char cDelimiterEnterRefOrEnum {'<'};
static constexpr char cDelimiterWeakString     {'"'};
static constexpr char cDelimiterStrongString   {'\''};
static constexpr const char* cContextResources {"Resources"};
static constexpr const char* cContextThinkers  {"Thinkers"};

using namespace TheatreFile;

enum Context { TOP_LEVEL = 0, RESOURCES = 1, THINKERS = 2, IN_RESOURCE = 3, IN_THINKER = 4 };
enum Comment { SINGLE_LINE, MULTI_LINE, NO_COMMENT }; // yes, `NO_COMMENT` is a pun

static ThingVarType s_GetVarTypeFromKeyword(Sarg);
static TheatreFile::ThingData s_ParseThing(size_t&, Farg<TokenArray>, TheatreFile::TheatreData& outData, Context inContext);
static bool s_CheckIfComment(Comment&, Farg<Token>);

Error TheatreFile::Parser(Farg<TokenArray> inTokens, TheatreData& outData)
{

    ThingData     thing_dat{};
    ThingVariable thing_var{};

    Context context{TOP_LEVEL};
    Comment in_comment{NO_COMMENT};

    for(size_t i{0}; i < inTokens.size(); ++i)
    {
        Farg<Token> token{inTokens.at(i)};
        if(s_CheckIfComment(in_comment, token))
            { continue; }
        else if(token.category == TokenName::Keyword)
        {
            if(context == TOP_LEVEL)
            {
                if(!token.token.compare(cContextResources))     { context = RESOURCES; }
                else if(!token.token.compare(cContextThinkers)) { context = THINKERS;  }
            }
            else if(ThingFactory::IsThing(token.token))
                { s_ParseThing(i, inTokens, outData, context); }
        }
    }
    return OK;
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

ThingVarType s_GetVarTypeFromKeyword(Sarg inKeyword)
{
    switch(ConstexprHash(inKeyword))
    {
    case ConstexprHash("Child"):
        return ThingVarType::Child;
    case ConstexprHash("Parent"):
        return ThingVarType::Parent;
    default:
        return ThingVarType::None;
    }
}

TheatreFile::ThingData s_ParseThing(size_t& inIndex,
    Farg<TokenArray> inTokens,
    TheatreFile::TheatreData& outData,
    Context inContext)
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
        case TokenName::Literal:
            [[fallthrough]];
        case TokenName::None:
            [[fallthrough]];
        default:
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
                    if(inTokens.at(inIndex).category != TokenName::Keyword)
                        { continue; }
                    next_token = inTokens.at(inIndex);
                    thing_var.name = next_token.token;
                    for(size_t i{inIndex+1}; i < inTokens.size() and !exit_for; ++i)
                    {
                        if(inTokens.at(i).category != TokenName::Identifier)
                            { continue; }
                        thing_var.value = inTokens.at(i).token;
                        exit_for = true;
                    }
                    --inIndex;
                }
                thing_data.variables.push_back(thing_var);
                thing_var.clear();
                Context new_context{(ThingFactory::IsResource(next_token.token))
                    ? RESOURCES
                    : THINKERS};
                s_ParseThing(inIndex, inTokens, outData, new_context);
                continue;
            }
            break;
        case TokenName::Whitespace:
            if((inContext == IN_RESOURCE or inContext == IN_THINKER)
                and !thing_var.value.empty()
                and !in_string
                and !in_literal
                and token.token[0] == '\n')
            {
                thing_data.variables.push_back(thing_var);
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
            case cDelimiterEndResource:
                if(inContext == IN_RESOURCE)
                {
                    thing_data.variables.push_back(thing_var);
                    return thing_data;
                }
                break;
            case cDelimiterEnterContext:
                if(inContext == RESOURCES) { inContext = IN_RESOURCE; }
                else if(inContext == THINKERS) { inContext = IN_THINKER; }
                break;
            case cDelimiterExitContext:
                outData.push_back(thing_data);
                return thing_data;
            default:
                break;
            }
            break;
        case TokenName::Keyword:
            if(!token.token.compare("Child") or !token.token.compare("Parent"))
                { thing_var.type = s_GetVarTypeFromKeyword(token.token); }
            else if(ThingFactory::IsThing(token.token) and thing_data.type.invalid())
                { thing_data.type = token.token; }
            else if(thing_var.name.empty())
                { thing_var.name = token.token; }
            break;
        case TokenName::Identifier:
            if(thing_data.name.empty())
                { thing_data.name = token.token; }
            else if(thing_var.name.empty())
                { thing_var.name = token.token; }
            else if(thing_var.value.empty())
                { thing_var.value = token.token; }
            break;
        }
    }
    return thing_data;
}
