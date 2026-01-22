#ifndef THEATRE_VARIABLE_REGISTRY_H
#define THEATRE_VARIABLE_REGISTRY_H

#include <typeindex>

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

    VariableRegistry() noexcept;

    using References = std::map<std::string, uint>;
    using Enums      = std::map<std::string, enum_pair>;

    Farg<References> GetRegisteredIDs() const;
    Farg<Enums>      GetRegisteredEnums() const;

    void Init();

    bool  try_GetID(Farg<std::string> inName, uint& outID) const;
    bool  try_GetIDName(uint inID, std::string& outName) const;
    uint  GetID(Sarg inName) const;
    std::string GetIDName(uint inID) const;
    Error RegisterID(Farg<std::string> inName, uint inID, bool doNoCopies = true);
    Error RemoveID(Farg<std::string> inName);
    Error RemoveID(uint inID);
    void  ClearIDs();
    bool  HasID(uint inID) const;
    bool  HasID(Sarg inName) const;

    static void ClearEnums();

    template<IsEnum T>
        static bool try_GetEnum(Farg<std::string> inName, T& outValue)
        {
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
            long val{static_cast<long>(inValue)};
            std::type_index ind{typeid(T)};
            for(FARG(auto) [name, pair] : m_sEnums)
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
        static T GetEnum(Farg<std::string> inName)
        {
            T out{};
            try_GetEnum(inName, out);
            return out;
        }

    template<IsEnum T>
        static std::string GetEnumName(T inValue)
        {
            std::string out{};
            try_GetEnumName(inValue, out);
            return out;
        }

    template<IsEnum T>
        static Error RegisterEnum(Farg<std::string> inName, T inValue, bool doNoCopies = true)
        {
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
        static Error RemoveEnum(Farg<std::string> inName, bool doRemoveCollisions = false)
        {
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

private:
    References mReferences{};
    static Enums m_sEnums;

    static void RegisterEngineEnums();
    void RegisterEngineReferences();
};

#endif // THEATRE_VARIABLE_REGISTRY_H
