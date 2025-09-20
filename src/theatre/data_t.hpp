#ifndef DATA_T_H
#define DATA_T_H

#include "variable_t.hpp"
#include "string_to_num.hpp"
#include "things/id.hpp"
#include "things/things.hpp"
#include "filesystem/fwd.hpp"

#include <vector>
#include <algorithm>
#include <string>

struct data_t
{
public:
    data_t();
    data_t(const std::string& Name, const std::string& Type, id_t ID = ID::None);

    void AddVariable(const std::string& Name, const std::string& Value, const VariableType& Type);

    id_t GetID() const;
    void SetID(id_t ID);

    const std::string& GetName() const;
    void SetName(const std::string& Name);

    size_t GetType() const;
    const std::string& GetTypeName() const;
    void SetType(const std::string& Type);

    int GetPriority() const;
    void SetPriority(int Priority);

    void clear();

    bool GetTheatreRef(unsigned int& AssignTo, const std::string& VariableName) const;
    bool GetFileData(FileData& AssignTo, const std::string& VariableName) const;
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

private:
    friend struct TheatreData;
    std::vector<variable_t> m_Variables = {};
    std::string m_Name = "Untitled";
    std::string m_TypeName = "Invalid";
    size_t      m_Type = Type::Invalid;
    id_t        m_AssignedID = ID::None;
    int         m_Priority = 0;
};

#endif // DATA_T_H
