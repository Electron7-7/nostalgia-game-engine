#ifndef THEATRE_DATA_H
#define THEATRE_DATA_H

#include "fwd.hpp"

#include "common/ids.hpp"
#include "thing_data.hpp"
#include "common/safe_return.hpp"
#include "frozen/map.h"
#include "physics/enums.hpp"

static constexpr frozen::map<std::string, penum_t, 7>
sPrettyEnumLookup =
{
    { PhysicsBodyMotion::Dynamic.name(),   PhysicsBodyMotion::Dynamic   },
    { PhysicsBodyMotion::Kinematic.name(), PhysicsBodyMotion::Kinematic },
    { PhysicsBodyMotion::Static.name(),    PhysicsBodyMotion::Static    },
    { PhysicsBodyShape::Box.name(),        PhysicsBodyShape::Box        },
    { PhysicsBodyShape::Capsule.name(),    PhysicsBodyShape::Capsule    },
    { PhysicsBodyShape::Cylinder.name(),   PhysicsBodyShape::Cylinder   },
    { PhysicsBodyShape::Sphere.name(),     PhysicsBodyShape::Sphere     },
};

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
