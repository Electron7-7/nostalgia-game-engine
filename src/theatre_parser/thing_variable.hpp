#ifndef THING_VARIABLE_H
#define THING_VARIABLE_H

#include "penum_t.hpp"
#include "ids.hpp"
#include "glm_to_string.hpp"

#include <string>
#include <format>

template<typename T>
concept IsNumber = requires
{
    !std::is_same_v<bool, std::decay_t<T>> &&
    (std::is_arithmetic_v<T> || GLMContainer<T>) == true;
};

struct ThingVar
{
    constexpr ThingVar() = default;
    template<IsNumber T>
        ThingVar(const T& Number, const std::string& Name = "")
            : name{Name}, value{std::format("{}", Number)}, type{eNumber} {}
    ThingVar(const std::string& String, const std::string& Name = "");
    ThingVar(const ID& ReferenceID, const std::string& Name = "");
    ThingVar(const penum_t& PrettyEnum, const std::string& Name = "");
    ThingVar(bool Boolean, const std::string& Name = "");

    std::string name{"Untitled ThingVar"};
    std::string value{""};
    penum_t     type{eNothing};
    ID          reference_id{ID::Invalid};
    penum_t     pretty_enum{};

    std::string formatted() const;
    std::string formatted_value() const;
    std::string log(bool colored = false) const;

    constexpr bool operator==(const std::string& VarName) const
    { return !name.compare(VarName); }

    static constexpr penum_t eNothing    { 0, "Nothing"    };
    static constexpr penum_t eReference  { 1, "Reference"  };
    static constexpr penum_t ePrettyEnum { 2, "PrettyEnum" };
    static constexpr penum_t eNumber     { 3, "Number"     };
    static constexpr penum_t eBool       { 4, "Bool"       };
    static constexpr penum_t eString     { 5, "String"     };
};

template<>
struct std::formatter<ThingVar> : std::formatter<std::string>
{
    auto format(const ThingVar& thing_var, std::format_context& ctx) const
    { return std::formatter<std::string>::format(thing_var.log(), ctx); }
};

#endif // THING_VARIABLE_H
