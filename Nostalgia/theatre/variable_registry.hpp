#ifndef THEATRE_VARIABLE_REGISTRY_H
#define THEATRE_VARIABLE_REGISTRY_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

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

    using ResourceData = std::map<PID, Shared<FileData>>;
    using References   = std::map<PID, ID>;
    using Enums        = std::map<std::string, enum_pair>;

    VariableRegistry() noexcept;

    Farg<References>   GetRegisteredIDs() const;
    static Farg<Enums>        GetRegisteredEnums();
    static Farg<ResourceData> GetRegisteredResourceData();

    void Init();

    bool  try_GetID(Sarg inName, ID& outID) const;
    bool  try_GetIDName(ID inID, std::string& outName) const;
    ID    GetID(Sarg inName) const;
    Sarg  GetIDName(ID inID) const;
    Error RegisterID(Sarg inName, ID inID, bool doNoCopies = true);
    Error RemoveID(Sarg inName);
    Error RemoveID(ID);
    void  ClearIDs();
    bool  HasID(ID) const;
    bool  HasID(Sarg inName) const;

    static bool try_GetResourceData(Sarg inName, Shared<FileData>& outData);
    static bool try_GetResourceData(ID inID, Shared<FileData>& outData);
    static Shared<FileData> GetResourceData(Sarg inName);
    static Shared<FileData> GetResourceData(ID inID);
    static bool HasResourceData(Sarg inName);
    static bool HasResourceData(ID inID);
    static Error RegisterResourceData(ID inID, Sarg inName, Farg<Shared<FileData>> inData, bool doNoCopies = true);
    static Error RemoveResourceData(Sarg inName);
    static Error RemoveResourceData(ID inID);
    static void ClearResourceData();

    static void ClearEnums();

    template<IsEnum T>
        static bool try_GetEnum(Sarg inName, T& outValue)
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
        static Error RegisterEnum(Sarg inName, T inValue, bool doNoCopies = true)
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
        static Error RemoveEnum(Sarg inName, bool doRemoveCollisions = false)
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
    static ResourceData m_sResourceData;

    void RegisterEngineReferences();
    static void RegisterEngineEnums();
    static void RegisterEngineResourceData();
};

#endif // THEATRE_VARIABLE_REGISTRY_H
