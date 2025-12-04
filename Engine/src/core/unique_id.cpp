#include "core/id.hpp"
#include "core/printing.hpp"
#include "core/type_helpers.hpp"

#include <random>
#include <set>

static std::random_device sRandomSeed;
static std::mt19937 sIdEngine(sRandomSeed());
static std::uniform_int_distribution<id_t> sIdDistribution{(id_t)UniqueID::front, (id_t)UniqueID::back};
static std::set<id_t> sExistingIDs{};
static constexpr unsigned int cMaxUniqueID{(id_t)UniqueID::back - (id_t)UniqueID::Reserved::back}; // Probably one less than the real max but it shouldn't matter

id_t base_id::Generate()
{
    sIdEngine.seed(sRandomSeed());
    return sIdDistribution(sIdEngine);
}

id_t UniqueID::Generate()
{
    if(sExistingIDs.size() == cMaxUniqueID)
    {
        print_warning("Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUniqueID);
        sExistingIDs.clear();
    }
    id_t new_id{ID::Generate()};
    do { new_id = ID::Generate(); } while ( !sExistingIDs.insert(new_id).second );
    return new_id;
}

void UniqueID::Clear()
{ sExistingIDs.clear(); }

bool UniqueID::Push(FARG(ID) id)
{ return sExistingIDs.insert(id()).second; }

bool UniqueID::Erase(FARG(ID) id)
{ return (sExistingIDs.erase(id()) != 0); }

bool UniqueID::Contains(FARG(ID) id)
{ return sExistingIDs.contains(id()); }

bool UniqueID::IsReserved(FARG(ID) id)
{ return id < front; }

bool UniqueID::GetReservedID(FARG(std::string) in_name, ID& out)
{
    for(const auto& [uid, name] : Reserved::EmbeddedResourceNames)
        { if(!name.compare(in_name)) { out = static_cast<id_t>(uid); return true; } }
    return false;
}

bool UniqueID::GetReservedName(FARG(ID) in, std::string& out)
{
    if(const auto& found_it{Reserved::EmbeddedResourceNames.find(static_cast<id_t>(in))};
        found_it != Reserved::EmbeddedResourceNames.end())
    { out = found_it->second; return true; }
    return false;
}
