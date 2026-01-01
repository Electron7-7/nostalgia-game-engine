#include "id.hpp"
#include "uid.hpp"
#include "printing.hpp"
#include <random>
#include <set>

static std::set<uint> sActiveIDs{};
static std::random_device sRandomSeed;
static std::mt19937 sIdEngine(sRandomSeed());
static std::uniform_int_distribution<uint> sIdDistribution{UID::front, UID::back};

constexpr uint cMaxUID{UID::back - UID::reserved_back}; // Probably one less than the real max but it shouldn't matter

uint ID::Generate()
{
    sIdEngine.seed(sRandomSeed());
    return sIdDistribution(sIdEngine);
}

uint UID::Generate()
{
    if(sActiveIDs.size() == cMaxUID)
    {
        print_warning("Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUID);
        sActiveIDs.clear();
    }
    uint new_id{ID::Generate()};
    while(!sActiveIDs.insert(new_id).second)
        { new_id = ID::Generate(); }
    return new_id;
}

void UID::Clear()
{ sActiveIDs.clear(); }

bool UID::Push(uint id)
{ return sActiveIDs.insert(id).second; }

bool UID::Erase(uint id)
{ return sActiveIDs.erase(id); }

UID::ReservedType UID::GetReservedType(uint id)
{
    if(id > reserved_back)
        { return ReservedType::NotReserved; }
    else if(id == a_Player)
        { return ReservedType::Player; }
    else if(id < models_front)
        { return ReservedType::Image; }
    else if(id < fonts_front)
        { return ReservedType::Model; }
    else if(id < front)
        { return ReservedType::Font; }
    return ReservedType::NotReserved;
}

bool UID::IsReserved(uint id)
{ return id < front; }

bool UID::Contains(uint id)
{ return sActiveIDs.contains(id); }
