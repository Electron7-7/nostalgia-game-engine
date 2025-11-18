#include "thing_variable.hpp"
#include "managers/theatre_manager.hpp"
#include "core/globals.hpp"
#include "common/colors.hpp"

ThingVar::ThingVar(const std::string& string, const std::string& _name)
    : name{_name}, value{string}, type{eString} {}

ThingVar::ThingVar(const ID& id, const std::string& _name)
    : name{_name}, value{""}, type{eReference}, reference_id{id}
    {
        if(!UniqueID::GetReservedName(id, value))
            { value = g_pTheatreManager->GetThing(id)->name(); }
    }

ThingVar::ThingVar(const penum_t& p_enum, const std::string& _name)
    : name{_name}, type{eEnum}, enum_name{p_enum.name()}, enum_value{p_enum.id()} {}

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
        return std::format("[{}]", value);
    case eReference:
        return std::format("<{}> // uid: {}", value, reference_id);
    case eEnum:
        return std::format("({})", enum_name);
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
