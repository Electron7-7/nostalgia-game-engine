#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "data_t.hpp"
#include "safe_return.hpp"

#include <map>

struct TheatreData
{
    static const TheatreData Missing;

    std::string m_Name = "Untitled Theatre";
    long m_Index = -1;

    const std::vector<data_t>& GetData() const;
    void UpdateReferences(const std::map<std::string, std::string>& NameIDMap);
    void OrderByPriority();

    SafeStatus AddData(const data_t& Data);

    void clear();

#ifdef DEBUGGING
    void debug_PrintData();
#endif // DEBUGGING

private:
    std::vector<data_t> m_Data;
};

#endif // THEATRE_DATA_H
