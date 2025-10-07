#ifndef THING_VARIABLE_H
#define THING_VARIABLE_H

#include "penum_t.hpp"
#include "colors.hpp"

#include <string>
#include <format>

struct ThingVar
{
    std::string name{"Untitled ThingVar"};
    std::string value{""};
    penum_t     type{eNothing};

    static constexpr penum_t eNothing    { 0, "Nothing"   };
    static constexpr penum_t eReferenceT { 1, "TheatreReference" };
    static constexpr penum_t eReferenceE { 2, "EngineReference"  };
    static constexpr penum_t eNumber     { 3, "Number"    };
    static constexpr penum_t eBool       { 4, "Bool"      };
    static constexpr penum_t eString     { 5, "String"    };

    std::string formatted_value() const
    {
        switch(type)
        {
        case eString:
            return std::format("'{}'", value);
        case eBool:
        case eNumber:
            return std::format("({})", value);
        case eReferenceE:
            return std::format("[{}]", value);
        case eReferenceT:
            return std::format("<{}>", value);
        case eNothing:
        default:
            return value;
        }
    }

    std::string log(bool colored = false) const
    {
        if(colored)
            { return std::format("{2}({5}){0} {1}{3}{0} == {4}", sty::Reset, sty::Bold + fg::Green, sty::Bold + fg::Yellow, name, value, type); }
        else
            { return std::format("({2}) {0} == {1}", name, value, type); }
    }

    constexpr bool operator==(const std::string& VarName) const
    { return !name.compare(VarName); }
};

template<>
struct std::formatter<ThingVar> : std::formatter<std::string>
{
    auto format(const ThingVar& thing_var, std::format_context& ctx) const
    { return std::formatter<std::string>::format(thing_var.log(), ctx); }
};

#endif // THING_VARIABLE_H
