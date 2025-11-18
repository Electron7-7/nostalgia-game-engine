#ifndef THING_VARIABLE_H
#define THING_VARIABLE_H

#include "core/penum_t.hpp"
#include "core/id.hpp"
#include "math/glm_concepts.hpp"
#include "math/glm_format.hpp" // IWYU pragma: keep // used by one of the constructors

#include <string>
#include <format>

template<typename T>
    concept IsNumber = !std::is_same_v<bool, std::decay_t<T>> && (std::is_arithmetic_v<T> || GLMContainer<T>);

template<typename T>
    concept IsEnum = std::is_enum_v<T>;

struct ThingVar
{
    constexpr ThingVar() = default;
    template<IsNumber T>
        ThingVar(const T& Number, const std::string& Name = "")
            : name{Name}, value{std::format("{}", Number)}, type{eNumber} {}
    template<IsEnum T>
        ThingVar(T Enum, const std::string& EnumName, const std::string& Name = "")
            : name{Name}, type{eEnum}, enum_name{EnumName}, enum_value{Enum} {}
    ThingVar(const std::string& String, const std::string& Name = "");
    ThingVar(const ID& ReferenceID, const std::string& Name = "");
    ThingVar(const penum_t& PrettyEnum, const std::string& Name = "");
    ThingVar(bool Boolean, const std::string& Name = "");

    std::string name{"Untitled ThingVar"};
    std::string value{""};
    penum_t     type{eNothing};
    ID          reference_id{ID::Invalid};
    std::string enum_name{""};
    int         enum_value{0};

    std::string formatted() const;
    std::string formatted_value() const;
    std::string log(bool colored = false) const;

    constexpr bool operator==(const std::string& VarName) const
    { return !name.compare(VarName); }

    static constexpr penum_t eNothing    { 0, "Nothing"    };
    static constexpr penum_t eReference  { 1, "Reference"  };
    static constexpr penum_t eEnum       { 2, "Enum"       };
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
