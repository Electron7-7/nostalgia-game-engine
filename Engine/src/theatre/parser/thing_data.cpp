#include "thing_data.hpp"
#include "thing_variable.hpp"
#include "things/thing_factory.hpp"
#include "core/printing.hpp"
#include "common/colors.hpp"

ThingData::ThingData() = default;

ThingData::ThingData(Sarg inName, Sarg inTypeName):
    name{inName},
    type_{inTypeName}
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

bool ThingData::GetChildren(children_t& output) const
{
    if(children.empty())
        { return false; }
    for(FAUTO child : children)
        { output.emplace_back(child.id_or_enum, !child.name.compare(cUniqueChildVarName)); }
    return true;
}

void ThingData::SetChildren(Farg<children_t> input)
{
    children.clear();
    for(auto child : input)
        { children.emplace_back(child.id, (child.is_unique) ? cUniqueChildVarName : cChildVarName); }
}

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
    if(!inName.compare(cChildVarName))
    {
        children.emplace_back(ThingVar::Type::Reference, cChildVarName, inValue);
        return children.back();
    }
    RemoveVariable(inName);
    variables.emplace_back(inType, inName, inValue);
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
    for(FAUTO child : children)
    {
        if(indent)
            { log.push_back('\t'); }
        log.append(std::format("{}\n", child.log(colored)));
    }
    return log;
}

Farg<TTID> ThingData::type() const
{ return type_; }

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
