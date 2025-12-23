#include "uid.hpp"
#include "printing.hpp"

#include <random>
#include <set>

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
    if(m_sActiveIDs.size() == cMaxUID)
    {
        print_warning("Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUID);
        m_sActiveIDs.clear();
    }
    uint new_id{ID::Generate()};
    while(!m_sActiveIDs.insert(new_id).second)
        { new_id = ID::Generate(); }
    return new_id;
}

void UID::Clear()
{ m_sActiveIDs.clear(); }

bool UID::Push(uint id)
{ return m_sActiveIDs.insert(id).second; }

bool UID::Erase(uint id)
{ return m_sActiveIDs.erase(id); }

bool UID::Contains(uint id)
{ return m_sActiveIDs.contains(id); }

bool UID::IsReserved(uint id)
{ return id < front; }
