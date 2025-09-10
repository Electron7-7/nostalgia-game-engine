#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "things/id.hpp"
#include "hash.hpp"
#include "safe_return.hpp"
#include "string_to_num.hpp"
#include "types/typenames.hpp"
#include "filesystem/binary_file_data.hpp"

#include <map>
#include <vector>

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

// This is for printouts
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

    const std::string& Name() const;
    const std::string& Value() const;
    const VariableType& Type() const;

    const bool operator==(const std::string& Compare) const
    { return !m_Name.compare(Compare); }

private:
#ifdef DEBUGGING
    friend struct TheatreData;
#endif // DEBUGGING
    friend struct data_t;
    variable_t(const std::string& Name);

    std::string  m_Name  = "Untitled";
    std::string  m_Value = "";
    VariableType m_Type  = VariableType::Default;

    void clear(); // TODO: Maybe make this public?
};

struct data_t
{
public:
    data_t();

    void AddVariable(const std::string& Name, const std::string& Value, const VariableType& Type);

    id_t GetID() const;
    void SetID(id_t ID);

    const std::string& GetName() const;
    void SetName(const std::string& Name);

    size_t GetType() const;
    const std::string& GetTypeName() const;
    void SetType(const std::string& Type);

    void clear();

    bool GetTheatreRef(unsigned int& AssignTo, const std::string& VariableName) const;
    bool GetEngineRef(BinaryFileData& AssignTo, const std::string& VariableName) const;
    bool GetEngineRef(StringFileData& AssignTo, const std::string& VariableName) const;
    bool GetBool(bool& AssignTo, const std::string& VariableName) const;
    bool GetString(std::string& AssignTo, const std::string& VariableName) const;

    template<typename T>
    bool GetNumber(T& AssignTo, const std::string& VariableName) const
    {
        const auto& variable = std::find(m_Variables.begin(), m_Variables.end(), VariableName);
        if(variable == m_Variables.end() || variable->m_Value.empty())
            { return false; }
        StringToNum<T>(AssignTo, variable->m_Value);
        return true;
    }

    const bool operator<(const data_t& Compare) const { return m_Hash < Compare.m_Hash; }

private:
#ifdef DEBUGGING
    friend struct TheatreData;
#endif // DEBUGGING
    data_t(const std::string& Name, const std::string& Type);

    std::vector<variable_t> m_Variables = {};
    std::string m_Name = "Untitled";
    std::string m_TypeName = "Invalid";
    size_t      m_Type = Type::Invalid;
    size_t      m_Hash = ConstexprHash(m_Name + m_TypeName);
    id_t        m_AssignedID = NoID;
};

struct TheatreData
{
    static const TheatreData Missing;

    std::string m_Name = "Untitled Theatre";
    long m_Index = -1; // FIXME: Make this not a magic number

    const std::vector<data_t>& GetData() const;
    void UpdateTheatreReferences(const std::map<std::string, std::string>& NameIDMap);

    SafeStatus AddData(const data_t& Data);

    void clear();

#ifdef DEBUGGING
    void debug_PrintData();
#endif // DEBUGGING

private:
    std::vector<data_t> m_Data;
};

#endif // THEATRE_DATA_H
