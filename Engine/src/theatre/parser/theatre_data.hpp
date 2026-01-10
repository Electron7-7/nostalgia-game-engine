#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "fwd/core.hpp"
#include "theatre/parser/thing_data.hpp"

struct TheatreData
{
    static const TheatreData Missing;

    std::vector<ThingData> things_data{};
    std::string name{"UntitledTheatre"};
    std::string file_path{""};
    uint index{ID::Invalid};

    void SetupUIDsAndPriorities();

    Error AddData(Farg<ThingData> Data);

    std::string formatted() const;
    ID id() const;
    void clear();
    void debug_PrintData() const;

    constexpr bool operator==(Farg<TheatreData> Other) const
    { return (id_ == Other.id_); }

private:
    ID id_{};
};

#endif // THEATRE_DATA_H
