#include "thing_variable.hpp"
#include "common/colors.hpp"

ThingVar::ThingVar() noexcept = default;

ThingVar::ThingVar(Type inParsedType, Sarg inParsedName, Sarg inParsedValue, uint inParsedID) noexcept:
    type{inParsedType},
    name{inParsedName},
    value{inParsedValue},
    id_or_enum{inParsedID} {}

ThingVar::ThingVar(Sarg inString, Sarg inName) noexcept:
    ThingVar{Type::String, inName, inString} {}

ThingVar::ThingVar(bool inBoolean, Sarg inName) noexcept:
    ThingVar{Type::Bool, inName, NumToString(inBoolean) } {}

ThingVar::ThingVar(ID inThingID, Sarg inName, Sarg inThingName) noexcept:
    type{Type::Reference},
    name{inName},
    value{inThingName},
    id_or_enum{inThingID[]}
{
    if(inThingName.empty())
        { g_pVariableRegistry->try_GetIDName(id_or_enum, value); }
}

std::string ThingVar::formatted() const
{
    if(value.empty())
        { return value; }
    return std::format("{} = {}", name, formatted_value());
}

std::string ThingVar::formatted_value() const
{
    switch(type)
    {
    case Type::String:
        return std::format("'{}'", value);
    case Type::Bool:
    case Type::Number:
        return std::format("[{}]", value);
    case Type::Reference:
        return std::format("<{}> // uid: {}", value, id_or_enum);
    case Type::Enum:
        return std::format("({}) // value: {}", value, id_or_enum);
    case Type::None:
    default:
        return value;
    }
}

static constexpr std::string sTypeName(ThingVar::Type inType)
{
    switch(inType)
    {
    case ThingVar::Type::String:
        return "String";
    case ThingVar::Type::Bool:
    case ThingVar::Type::Number:
        return "Number";
    case ThingVar::Type::Reference:
        return "Reference";
    case ThingVar::Type::Enum:
        return "Enum";
    case ThingVar::Type::None:
        return "None";
    default:
        return "Invalid ThingVar::Type";
    }
}

std::string ThingVar::log(bool colored, bool isChild) const
{
    const char* child_prefix{(isChild) ? "Child " : ""};
    if(colored)
    {
        return std::format("{2}({6}{4}){0} {1}{3}{0} == {5}",
            Sty::Reset,
            Sty::Bold + Fg::Green,
            Sty::Bold + Fg::Yellow,
            name,
            sTypeName(type),
            formatted_value(),
            child_prefix);
    }
    else
    {
        return std::format("({}{}) {} == {}",
            child_prefix,
            sTypeName(type),
            name,
            formatted_value());
    }
}
