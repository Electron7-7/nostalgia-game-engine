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

    static constexpr penum_t eNothing   { 0, "Nothing"   };
    static constexpr penum_t eReference { 1, "Reference" };
    static constexpr penum_t eNumber    { 2, "Number"    };
    static constexpr penum_t eBool      { 3, "Bool"      };
    static constexpr penum_t eString    { 4, "String"    };

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
