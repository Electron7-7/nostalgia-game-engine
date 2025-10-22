#include "thing_variable.hpp"
#include "managers/theatre_manager.hpp"
#include "common/colors.hpp"
#include "common/globals.hpp"

ThingVar::ThingVar(const std::string& string, const std::string& _name)
    : name{_name}, value{string}, type{eString} {}

ThingVar::ThingVar(const ID& id, const std::string& _name)
    : name{_name}, value{""}, type{eReference}, reference_id{id}
    {
        if(!UniqueIDs::GetReservedName(id, value))
            { value = g_pTheatreManager->GetThing(id)->name(); }
    }

ThingVar::ThingVar(const penum_t& p_enum, const std::string& _name)
    : name{_name}, type{ePrettyEnum}, pretty_enum{p_enum} {}

ThingVar::ThingVar(bool boolean, const std::string& _name)
    : name{_name}, value{(boolean) ? gTrue : gFalse}, type{eBool} {}

std::string ThingVar::formatted() const
{
    if(value.empty())
        { return ""; }
    return std::format("{} = {}", name, formatted_value());
}

std::string ThingVar::formatted_value() const
{
    switch(type)
    {
    case eString:
        return std::format("'{}'", value);
    case eBool:
    case eNumber:
        return std::format("({})", value);
    case eReference:
        return std::format("<{}>", value);
    case ePrettyEnum:
        return std::format("[{}]", value);
    case eNothing:
    default:
        return value;
    }
}

std::string ThingVar::log(bool colored) const
{
    if(colored)
        { return std::format("{2}({5}){0} {1}{3}{0} == {4}", Sty::Reset, Sty::Bold + Fg::Green, Sty::Bold + Fg::Yellow, name, formatted_value(), type); }
    else
        { return std::format("({2}) {0} == {1}", name, formatted_value(), type); }
}
