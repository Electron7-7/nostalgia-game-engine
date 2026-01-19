#ifndef ENUM_PRETTIFIER_H
#define ENUM_PRETTIFIER_H

#include "core/farg.hpp"
#include "core/error.hpp"
#include "math/concepts.hpp"
#include <unordered_map>
#include <typeindex>

class EnumPrettifier
{
public:
    using enums_map = std::unordered_map<long, std::string>;
    using enums_registry = std::unordered_map<std::type_index, enums_map>;

    // Will not overwrite a previously registered enum
    template<IsEnum T>
        static Error Register(T inEnum, Sarg inName) noexcept
        {
            return (m_sEnums[typeid(T)].emplace(static_cast<long>(inEnum), inName).second)
                ? OK
                : ERR_ALREADY_EXISTS;
        }

    // Will overwrite a previously registered enum
    template<IsEnum T>
        static void Assign(T inEnum, Sarg inName) noexcept
            { m_sEnums[typeid(T)][static_cast<long>(inEnum)] = inName; }

    template<IsEnum T>
        static Sarg Prettify(T inEnum) noexcept
        {
            if(auto found_one{m_sEnums.find(typeid(T))};
                found_one != m_sEnums.end())
            {
                if(auto found_two{found_one->second.find(static_cast<long>(inEnum))};
                    found_two != found_one->second.end())
                    { return found_two->second; }
            }
            return m_sEmpty;
        }

private:
    inline static enums_registry m_sEnums{};
    inline static std::string m_sEmpty{"Unregistered Enum"};
};

#endif // ENUM_PRETTIFIER_H
