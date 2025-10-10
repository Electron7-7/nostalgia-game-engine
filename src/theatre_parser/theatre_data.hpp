#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "fwd.hpp"

#include "ids.hpp"
#include "thing_data.hpp"
#include "safe_return.hpp"

struct TheatreData
{
    static const TheatreData Missing;

    std::vector<ThingData> things_data{};
    std::string name{"UntitledTheatre"};
    std::string file_path{""};
    int index = -1;

    void SetupUIDsAndPriorities();

    SafeStatus AddData(const ThingData& Data);

    std::string formatted() const;
    ID id() const;
    void clear();
    void debug_PrintData();

    constexpr bool operator==(const TheatreData& Other) const
    { return (id_ == Other.id_); }

private:
    ID id_ = ID::None;
};

#endif // THEATRE_DATA_H
