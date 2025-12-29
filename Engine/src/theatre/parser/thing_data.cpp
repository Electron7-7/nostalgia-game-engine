#include "thing_data.hpp"
#include "thing_variable.hpp"
#include "things/thing_factory.hpp"
#include "core/uid.hpp"
#include "core/printing.hpp"
#include "common/colors.hpp"

const ThingData ThingData::PlayerDefaultData{
    "Default Player",
    ThingType::NostalgiaPlayer,
    UID::a_Player,
    {
        ThingVar{glm::vec3(0.0f), "Rotation"},
        ThingVar{glm::vec3(0.0f), "Origin"},
    }};

ThingData::ThingData() = default;

ThingData::ThingData(Sarg inName, Sarg inTypeName):
    name{inName},
    type_{ConstexprHash(inTypeName)}
{
    if(!g_pThingFactory->IsThing(type_))
        { print_warningv(VERBOSE1, "{} is an invalid type!", type_.log()); }
}

ThingData::ThingData(Sarg inName, Farg<TTID> inType, ID inID, Farg<std::vector<ThingVar>> inVariables):
    uid{inID},
    name{inName},
    variables(inVariables),
    type_{inType}
{
    if(type_ == ThingType::NostalgiaPlayer) // ThingData::PlayerDefaultData triggers the error message bc of when it's constructed
        { return; }
    else if(!g_pThingFactory->IsThing(type_))
        { print_error("'{}' is an invalid type!", type_.name()); }
}

ThingData::ThingData(Sarg inName, Farg<TTID> inType, Farg<std::vector<ThingVar>> inVariables):
    name{inName},
    variables(inVariables),
    type_{inType} {}

bool ThingData::RemoveVariable(Sarg inName)
{
    if(auto iter = std::find(variables.cbegin(), variables.cend(), inName);
        iter != variables.cend())
    { variables.erase(iter); return true; }
    return false;
}

ThingVar& ThingData::AddVariable(Farg<ThingVar> inVariable, Sarg inName)
{
    RemoveVariable(inName);
    variables.push_back(inVariable);
    variables.back().name = inName;
    return variables.back();
}

ThingVar& ThingData::AddVariable(Sarg inName, Sarg inValue, ThingVar::Type inType)
{
    RemoveVariable(inName);
    variables.emplace_back();
    variables.back().name  = inName;
    variables.back().value = inValue;
    variables.back().type  = inType;
    return variables.back();
}

std::string ThingData::log(bool colored, bool indent) const
{
    std::string log;
    if(colored)
    {
        log = std::format("{1}(Type: {3}, ID: {5}){0} {2}{4}{0}\n",
            Sty::Reset,
            Sty::Bold + Fg::Yellow,
            Sty::Bold + Fg::Green,
            type_.name(),
            name,
            (uid.invalid())
                ? "ID::Invalid"
                : std::to_string(uid[]));
    }
    else
    {
        log = std::format("({}) {} [{}]\n",
            type_.name(),
            name,
            uid[]);
    }
    for(FARG(auto) var : variables)
    {
        if(indent)
            { log.push_back('\t'); }
        log.append(std::format("{}\n", var.log(colored)));
    }
    return log;
}

Farg<TTID> ThingData::type() const
{ return type_; }

bool ThingData::set_type(Sarg inTypeName)
{ return set_type(ConstexprHash(inTypeName)); }

bool ThingData::set_type(Farg<TTID> inType)
{
    type_ = inType;
    if(!g_pThingFactory->IsThing(inType))
    {
        return !print_warning("The specified type '{}' is not a known type! This data structure will not be used if its type name is invalid (meaning, you won't see '{}' in the Theatre)",
            inType.name(),
            name);
    }
    return true;
}

void ThingData::clear()
{ *this = ThingData(); }

bool ThingData::GetBoolean(bool& outBool, Sarg inName) const
{
    if(auto assert_var{AssertVariable(inName, ThingVar::Type::Bool)}; assert_var != variables.cend())
    {
        if(!assert_var->value.compare(std::format("{}", true)))
            { return outBool = true; }
        else if(!assert_var->value.compare(std::format("{}", false)))
            { return !(outBool = false); }
    }
    return false;
}

bool ThingData::GetString(std::string& outString, Sarg inName) const
{
    if(auto assert_var{AssertVariable(inName, ThingVar::Type::String)}; assert_var != variables.cend())
        { outString = assert_var->value; return true; }
    return false;
}

ThingData::VarIter_t ThingData::AssertVariable(Sarg inName, ThingVar::Type inType) const
{
    auto iter{std::find(variables.cbegin(), variables.cend(), inName)};
    if(iter == variables.cend() || iter->type != inType || iter->value.empty())
        { return variables.cend(); }
    return iter;
}
