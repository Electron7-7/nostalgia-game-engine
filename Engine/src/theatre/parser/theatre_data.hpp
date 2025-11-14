#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "fwd.hpp"

#include "common/ids.hpp"
#include "thing_data.hpp"
#include "common/safe_return.hpp"

struct TheatreData
{
    static const TheatreData Missing;

    std::vector<ThingData> things_data{};
    std::string name{"UntitledTheatre"};
    std::string file_path{""};
    id_t index{ID::Invalid};

    void SetupUIDsAndPriorities();

    SafeStatus AddData(const ThingData& Data);

    std::string formatted() const;
    ID id() const;
    void clear();
    void debug_PrintData();

    constexpr bool operator==(const TheatreData& Other) const
    { return (id_ == Other.id_); }

private:
    ID id_ = ID::Invalid;
};

#endif // THEATRE_DATA_H
