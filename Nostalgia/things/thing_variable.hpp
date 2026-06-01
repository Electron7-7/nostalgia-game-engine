#ifndef THING_VARIABLE_H
#define THING_VARIABLE_H

#include <Nostalgia/core/variant.hpp>

namespace TheatreFile
{
    using VariableUsageFlags = int;

    enum VariableHint : short
    {
        VARIABLE_HINT_NONE,
        VARIABLE_HINT_ENUM,
        VARIABLE_HINT_FLAGS,
        VARIABLE_HINT_FILE,
        VARIABLE_HINT_RESOURCE_TYPE,
        VARIABLE_HINT_PLACEHOLDER_TEXT,
        VARIABLE_HINT_THING_REFERENCE,
        VARIABLE_HINT_LINK_VALUES,
        VARIABLE_HINT_MAX,
    };

    enum VariableUsageFlags_ : VariableUsageFlags
    {
        VARIABLE_USAGE_INVALID  = 0,
        VARIABLE_USAGE_EDITOR   = 1 << 1,
        VARIABLE_USAGE_GROUP    = 1 << 2,
        VARIABLE_USAGE_CATEGORY = 1 << 3,
        VARIABLE_USAGE_SUBGROUP = 1 << 4,
    };

    struct ThingVariable
    {
        Variant value{0};
        std::string name{""};
        VariableHint hint{VARIABLE_HINT_NONE};
        std::string hint_string{""};
        VariableUsageFlags usage_flags{VARIABLE_USAGE_EDITOR};

        bool invalid() const noexcept;
    };

    using ThingVarArray = std::vector<ThingVariable>;
}

#endif // THING_VARIABLE_H
