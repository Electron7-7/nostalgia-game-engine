#include "ids.hpp"
#include "printing.hpp"

#include <random>
#include <set>

static std::random_device sRandomSeed;
static std::mt19937 sIdEngine(sRandomSeed());
static std::uniform_int_distribution<id_t> sIdDistribution{UniqueIDs::front, UniqueIDs::back};
static std::set<ID> sExistingIDs{};
static constexpr unsigned int cMaxUniqueIDs{UniqueIDs::back - UniqueIDs::Reserved::back}; // Probably one less than the real max but it shouldn't matter

ID UniqueIDs::Generate()
{
    if(sExistingIDs.size() == cMaxUniqueIDs)
    {
        print_warning("UniqueIDs::Generate - Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUniqueIDs);
        sExistingIDs.clear();
    }
    ID new_id = UniqueIDs::front;
    sIdEngine.seed(sRandomSeed());
    do { new_id = sIdDistribution(sIdEngine); } while ( sExistingIDs.contains(new_id) );
    sExistingIDs.insert(new_id);
    return new_id;
}

void UniqueIDs::Clear()
{ sExistingIDs.clear(); }

bool UniqueIDs::Push(ID id)
{
    if(Contains(id))
        { return false; }
    sExistingIDs.insert(id);
    return true;
}

bool UniqueIDs::Erase(ID id)
{ return (sExistingIDs.erase(id) != 0); }

bool UniqueIDs::Contains(ID id)
{ return sExistingIDs.contains(id); }

bool UniqueIDs::IsReserved(ID id)
{ return (id < front); }

bool UniqueIDs::GetReservedIDName(ID id, std::string& output)
{
    for(const auto& [name, r_id] : Reserved::ResourceNameToUIDMap)
        { if(r_id == id) { output = name; return true; } }
    return false;
}
