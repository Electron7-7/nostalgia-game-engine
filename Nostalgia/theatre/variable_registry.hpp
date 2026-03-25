#ifndef THEATRE_VARIABLE_REGISTRY_H
#define THEATRE_VARIABLE_REGISTRY_H

class VariableRegistry
{
public:
    struct enum_pair
    {
        enum_pair() noexcept = default;

        template<IsEnum T>
            enum_pair(T inEnum) noexcept:
                value{static_cast<long>(inEnum)}, index{typeid(T)} {}

        long value{0};
        std::type_index index{typeid(std::nullptr_t)};
    };

    using Enums        = std::map<std::string, enum_pair>;

    VariableRegistry() noexcept;

    static Farg<Enums> GetRegisteredEnums();

    static void ClearEnums();

    template<IsEnum T>
        static bool try_GetEnum(Sarg inName, T& outValue)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            if(auto found_it{m_sEnums.find(inName)}; found_it != m_sEnums.end())
            {
                outValue = static_cast<T>(found_it->second.value);
                return true;
            }
            return false;
        }

    template<IsEnum T>
        static bool try_GetEnumName(T inValue, std::string& outName)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            long val{static_cast<long>(inValue)};
            std::type_index ind{typeid(T)};
            for(FAUTO [name, pair] : m_sEnums)
            {
                if(val == pair.value and ind == pair.index)
                {
                    outName = name;
                    return true;
                }
            }
            return false;
        }

    template<IsEnum T>
        static T GetEnum(Sarg inName)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            T out{};
            try_GetEnum(inName, out);
            return out;
        }

    template<IsEnum T>
        static std::string GetEnumName(T inValue)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            std::string out{};
            try_GetEnumName(inValue, out);
            return out;
        }

    template<IsEnum T>
        static Error RegisterEnum(Sarg inName, T inValue, bool doNoCopies = true)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            if(doNoCopies)
            {
                if(auto found_them{m_sEnums.find(inName)}; found_them != m_sEnums.end())
                    { return ERR_ALREADY_EXISTS; }
            }
            m_sEnums[inName] = inValue;
            return OK;
        }

    template<IsEnum T>
        static Error RemoveEnum(T inValue, bool doRemoveCollisions = false)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            Error status{ERR_NOT_FOUND};
            long val{static_cast<long>(inValue)};
            std::type_index ind{typeid(T)};
            for(auto it{m_sEnums.begin()}; it != m_sEnums.end();)
            {
                if(val == it->second.value and ind == it->second.index)
                {
                    it = m_sEnums.erase(it);
                    status = OK;
                    if(!doRemoveCollisions)
                        { break; }
                    continue;
                }
                ++it;
            }
            return status;
        }

    template<IsEnum T>
        static Error RemoveEnum(Sarg inName, bool doRemoveCollisions = false)
        {
            LockGuard<RMutex> enums_lock{m_sEnumsMutex};
            if(doRemoveCollisions)
            {
                return (m_sEnums.erase(inName))
                    ? OK
                    : ERR_NOT_FOUND;
            }
            else if(auto found_it{m_sEnums.find(inName)}; found_it != m_sEnums.end())
            {
                m_sEnums.erase(found_it);
                return OK;
            }
            return ERR_NOT_FOUND;
        }

    static void RegisterEngineEnums();

private:
    // Evil mutable keyword usage
    mutable RMutex mReferencesMutex{};

    static Enums m_sEnums;
    static RMutex m_sEnumsMutex;
};

#endif // THEATRE_VARIABLE_REGISTRY_H
