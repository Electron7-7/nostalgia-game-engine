#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "safe_return.hpp"
#include "hash.hpp"
#include "glm/fwd.hpp"

#include <set>
#include <string>

enum class VariableType
{
    Nothing, // This is only used by the parser
    TheatreRef,
    EngineRef,
    Number,
    Bool,
    String,
    Default = String,
};

constexpr std::string StringifyEnum(const VariableType& VariableTypeEnum)
{
    switch(VariableTypeEnum)
    {
    case VariableType::TheatreRef:
        return "TheatreRef";
    case VariableType::EngineRef:
        return "EngineRef";
    case VariableType::Number:
        return "Number";
    case VariableType::Bool:
        return "Bool";
    case VariableType::String:
        return "String";
    default:
        return "Uhh... what?";
    }
}

struct variable_t
{
public:
    static const variable_t undefined;

    variable_t(const std::string& Name, const std::string& Value, const VariableType& Type);
    variable_t();

    const bool operator<(const variable_t& Compare) const
        { return m_Hash < Compare.m_Hash; }

private:
#ifdef DEBUGGING
    friend void debug_PrintParsedTheatreData();
#endif
    friend struct data_t;
    variable_t(const std::string& Name);

    std::string  m_Name  = "Untitled";
    std::string  m_Value = "N/A";
    VariableType m_Type  = VariableType::Default;
    size_t       m_Hash  = g_StringHash(m_Name);

    void clear();
};

struct data_t
{
public:
    data_t();

    static constexpr short VARIABLE_SET   = (short)true;
    static constexpr short VARIABLE_ADDED = (short)false;

    short SetVariable(const std::string& Name, const std::string& Value, const VariableType& Type);
    SafeReturn<const variable_t&> try_GetVariable(const std::string& Name);

    const std::string& GetName() const;
    void SetName(const std::string& Name);

    const std::string& GetType() const;
    void SetType(const std::string& TypeName);

    void clear();

    // Variable Interpreting Functions
    // template<typename T, bool is_numeric = std::is_arithmetic_v<T>>
    // T GetNumber(const std::string& VariableName);
    // template<> float GetNumber(const std::string& VariableName);
    // template<> int GetNumber(const std::string& VariableName);
    // template<> glm::vec3 GetNumber(const std::string& VariableName);
    // template<> glm::vec2 GetNumber(const std::string& VariableName);
    // bool GetBool(const std::string& VariableName);

    const bool operator<(const data_t& Compare) const
        { return m_Hash < Compare.m_Hash; }

private:
#ifdef DEBUGGING
    friend void debug_PrintParsedTheatreData();
#endif
    friend struct TheatreData;
    data_t(const std::string& Name);

    std::set<variable_t> m_Variables = {}; // The string key is the variable's name
    std::string m_Name = "Untitled";
    std::string m_Type = "Unset";
    size_t      m_Hash = g_StringHash(m_Name);
};

struct TheatreData
{
    std::string m_Name = "Untitled Theatre";
    long m_Index = -1; // FIXME: Make this not a magic number

    const std::set<data_t>& GetThings() const;
    const std::set<data_t>& GetResources() const;
    std::set<data_t> GetMergedData() const;

    SafeReturn<data_t> try_GetData(const std::string& Name) const; // Used by the parser
    SafeReturn<data_t> try_GetThingData(const std::string& ThingName) const;
    SafeReturn<data_t> try_GetResourceData(const std::string& ResourceName) const;

    SafeStatus AddData(const data_t& Data);

    void clear();

private:
    static data_t s_SafeDataReturn;

    std::set<data_t> m_Things;
    std::set<data_t> m_Resources;

    void AddThingData(const data_t& ThingData);
    void AddResourceData(const data_t& ResourceData);
};

#endif // THEATRE_DATA_H
