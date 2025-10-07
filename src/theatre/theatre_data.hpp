#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "fwd.hpp"

#include "ids.hpp"
#include "thing_data.hpp"
#include "safe_return.hpp"
#include "frozen/string.h"

#include <map>

struct TheatreData
{
    static const TheatreData Missing;

    frozen::string name = "TheatreData";
    int index = -1;

    const std::vector<ThingData>& GetData() const;
    void UpdateReferences(const std::map<std::string, std::string>& NameIDMap);
    void OrderByPriority();

    SafeStatus AddData(const ThingData& Data);

    ID id() const;
    void clear();
    void debug_PrintData();

    constexpr bool operator==(const TheatreData& Other) const
    { return (id_ == Other.id_); }

private:
    std::vector<ThingData> data_ = {};
    ID id_ = ID::None;
};

#endif // THEATRE_DATA_H
