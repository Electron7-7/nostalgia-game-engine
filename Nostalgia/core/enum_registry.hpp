#ifndef ENUM_REGISTRY_H
#define ENUM_REGISTRY_H

#define LOCK_MUTEX LockGuard<RMutex> lock{m_sMutex}
#define FIND_BY_NAME(NAME) \
    std::find_if(m_sEnums.begin(), m_sEnums.end(), \
        [NAME](Farg<Enum_t> enum_it) { return not enum_it.name.compare(NAME); } )
#define FIND_BY_ENUM(ENUM) \
    std::find_if(m_sEnums.begin(), m_sEnums.end(), \
        [ENUM](Farg<Enum_t> enum_it) \
            { return enum_it.type == typeid(T).hash_code() and enum_it.value == static_cast<long>(ENUM); } )
#define FOUND_BY_NAME(NAME) const auto found_it{FIND_BY_NAME(NAME)}; found_it != m_sEnums.end()
#define FOUND_BY_ENUM(ENUM) const auto found_it{FIND_BY_ENUM(ENUM)}; found_it != m_sEnums.end()

struct Enum_t
{
    size_t type{0}; long value{0}; std::string name{""};

    constexpr bool operator==(Farg<Enum_t> inOther) const noexcept
    { return type == inOther.type and value == inOther.value and not name.compare(inOther.name); }
};

namespace std
{
    template<> struct hash<Enum_t>
    {
        size_t operator()(Farg<Enum_t> inEnum) const
        { return inEnum.type; }
    };
}

class EnumRegistry
{
public:
    using Enums_t = std::unordered_set<Enum_t>;

    // Register an enum with a string name. Will not overwrite a previously registered enum
    template<IsEnum T>
        static Error Register(T inEnum, Sarg inName) noexcept
        {
            LOCK_MUTEX;
            return (m_sEnums.emplace(typeid(T).hash_code(), static_cast<long>(inEnum), inName).second)
                ? OK
                : ERR_ALREADY_EXISTS;
        }

    // Overwrite a previously registered enum's string name. If the enum doesn't exist, it is registered instead
    template<IsEnum T>
        static void Assign(T inEnum, Sarg inName) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_NAME(inName))
                { m_sEnums.erase(found_it); }
            Register(inEnum, inName);
        }

    // Erases the enum from the registry
    template<IsEnum T>
        static void Erase(T inEnum) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_ENUM(inEnum))
                { m_sEnums.erase(found_it); }
        }

    // Erases the enum registered with the given name from the registry
    template<IsEnum T>
        static void Erase(Sarg inName) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_NAME(inName))
                { m_sEnums.erase(found_it); }
        }

    // Returns `true` if the given enum was registered and `false` if not
    template<IsEnum T>
        static bool Contains(T inEnum) noexcept
        {
            LOCK_MUTEX;
            return FIND_BY_ENUM(inEnum) != m_sEnums.end();
        }

    static bool Contains(Sarg inName) noexcept
    {
        LOCK_MUTEX;
        return FIND_BY_NAME(inName) != m_sEnums.end();
    }

    // Searches for the enum via the given name and, if found, assigns its value to `ioEnum`
    template<IsEnum T>
        static Error GetEnum(Sarg inName, T& ioEnum) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_NAME(inName))
            {
                ioEnum = static_cast<T>(found_it->value);
                return OK;
            }
            return ERR_NOT_FOUND;
        }

    // Searches for the given enum and returns the string registered to it; if the enum cannot be found,
    // an empty string is returned instead
    template<IsEnum T>
        static Sarg GetEnumName(T inEnum) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_ENUM(inEnum))
                { return found_it->name; }
            return GlobalConstants::str_empty;
        }

    // Searches for the given enum's name and, if found, assigns it to `outName`
    template<IsEnum T>
        static Error GetEnumName(T inEnum, std::string& outName) noexcept
        {
            LOCK_MUTEX;
            if(FOUND_BY_ENUM(inEnum))
            {
                outName = found_it->name;
                return OK;
            }
            return ERR_NOT_FOUND;
        }

private:
    inline static Enums_t m_sEnums{};
    inline static RMutex m_sMutex{};
};

#undef LOCK_MUTEX
#undef FIND_BY_NAME
#undef FIND_BY_ENUM
#undef FOUND_BY_NAME
#undef FOUND_BY_ENUM
#endif // ENUM_REGISTRY_H
