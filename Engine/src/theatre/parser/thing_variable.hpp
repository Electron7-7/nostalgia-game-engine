#ifndef THING_VARIABLE_H
#define THING_VARIABLE_H

#include "theatre/parser/number_parser.hpp"
#include "theatre/variable_registry.hpp"
#include "core/id.hpp"
#include "core/printing.hpp"

#include <string>

enum class ThingVarType : ushort
{
    Reference,
    Enum,
    Number,
    Bool,
    String,
    None,
};

struct ThingVar
{
    using Type = ThingVarType;

    Type type{Type::None};
    std::string name{};
    std::string value{};
    long id_or_enum{ID::Invalid};
    size_t enum_type_hash{0};

    ThingVar() noexcept;
    ThingVar(Type inParsedType, Sarg inParsedName, Sarg inParsedValue, uint inParsedID = ID::Invalid) noexcept;
    ThingVar(Sarg inString, Sarg inName = "") noexcept;
    ThingVar(ID inThingID, Sarg inName = "", Sarg inThingNameOverride = "") noexcept;
    ThingVar(bool inBoolean, Sarg inName = "") noexcept;

    bool empty() const;

    template<NumberOrGLM T>
        ThingVar(Farg<T> inNumber, Sarg inName = "") noexcept:
            ThingVar{Type::Number, inName, NumToString(inNumber)} {}

    template<IsEnum T>
        ThingVar(T inEnum, Sarg inName = "") noexcept:
            type{Type::Enum},
            name{inName}
        {
            if(!g_pVariableRegistry->try_GetEnumName(inEnum, value))
            {
                print_warningv(VERBOSE1 | VERBOSE3, "Invalid enum passed [IsEnum T = {}]", typeid(T).name());
                return;
            }
            id_or_enum = static_cast<long>(inEnum);
            enum_type_hash = typeid(T).hash_code();
        }

    std::string formatted() const;
    std::string formatted_value() const;
    std::string log(bool useColors = false, bool isChild = false) const;

    constexpr bool operator==(Sarg inVarName) const
    { return !name.compare(inVarName); }
};

#endif // THING_VARIABLE_H
