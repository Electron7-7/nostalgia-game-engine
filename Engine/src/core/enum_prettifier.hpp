#ifdef FWD_DCL
#   define ENUM_SET(NAME) extern constinit const char* NAME;
    namespace EnumPrettifier
    {
        namespace EnumSets
        {
            ENUM_SET(Global)
            ENUM_SET(PhysicsBodyShape)
            ENUM_SET(PhysicsBodyMotion)
        }
    }
#   undef ENUM_SET
#elif !defined ENUM_PRETTIFIER_H
#define ENUM_PRETTIFIER_H

#include "farg.hpp"
#include "error.hpp"
#include "common/concepts.hpp"
#include "common/string_hash.hpp"

#include <string>
#include <unordered_map>

#define ENUM_SET(NAME) inline static constinit const char* NAME{#NAME};

// Used by the lazy
#define PRETTIFY_ENUM(ENUM, ENUM_SET) EnumPrettifier::Add(#ENUM, ENUM_SET::ENUM, #ENUM_SET)
#define GET_PRETTY_ENUM(ENUM, ENUM_SET) EnumPrettifier::Get(ENUM, #ENUM_SET)

struct EnumSet
{
    ENUM_SET(Global)
    ENUM_SET(PhysicsBodyShape)
    ENUM_SET(PhysicsBodyMotion)
};

class EnumPrettifier
{
private:
    inline static std::unordered_map<uint, std::unordered_map<int, std::string>> m_sEnumMaps{};

public:
    template<IsEnum T>
        static Farg<std::string> Get(T inEnum, uint inEnumSet)
        {
            static const std::string c_sDefaultString{"N/A"};
            if(auto found_them{m_sEnumMaps.find(inEnumSet)}; found_them != m_sEnumMaps.end())
            {
                if(auto found_it{found_them->second.find(static_cast<int>(inEnum))};
                    found_it != found_them->second.end())
                    { return found_it->second; }
            }
            return c_sDefaultString;
        }

    template<IsEnum T>
        static void Set(Farg<std::string> inName, T inEnum, uint inEnumSet)
        { m_sEnumMaps[inEnumSet][static_cast<int>(inEnum)] = inName; }

    template<IsEnum T>
        static Error Add(Farg<std::string> inName, T inEnum, uint inEnumSet)
        {
            if(m_sEnumMaps[inEnumSet].contains(static_cast<int>(inEnum)))
                { return ERR_ALREADY_EXISTS; }
            Set(inName, inEnum, inEnumSet);
            return OK;
        }

    template<IsEnum T>
        static Error EraseEnum(T inEnum, uint inEnumSet)
        {
            if(auto found_it{m_sEnumMaps.find(inEnumSet)}; found_it != m_sEnumMaps.end())
            {
                return (found_it->second.erase(static_cast<int>(inEnum)))
                    ? OK
                    : ERR_EMPTY;
            }
            return ERR_NOT_FOUND;
        }

    template<IsEnum T>
        static Farg<std::string> Get(T inEnum, Farg<std::string> inEnumSet)
        { return Get(inEnum, ConstexprHash(inEnumSet)); }

    template<IsEnum T>
        static void Set(Farg<std::string> inName, T inEnum, Farg<std::string> inEnumSet)
        { return Set(inName, inEnum, ConstexprHash(inEnumSet)); }

    template<IsEnum T>
        static Error Add(Farg<std::string> inName, T inEnum, Farg<std::string> inEnumSet)
        { return Add(inName, inEnum, ConstexprHash(inEnumSet)); }

    template<IsEnum T>
        static Error EraseEnum(T inEnum, Farg<std::string> inEnumSet)
        { return EraseEnum(inEnum, ConstexprHash(inEnumSet)); }


    static Error EraseEnum(Farg<std::string> inName, Farg<std::string> inSetName = EnumSet::Global);
    static Error EraseSet(Farg<std::string> inSetName);
    static Error EraseEnum(Farg<std::string> inName, uint inSetID);
    static Error EraseSet(uint inSetID);
    static void Clear();
};

#undef ENUM_SET
#endif // ENUM_PRETTIFIER_H
