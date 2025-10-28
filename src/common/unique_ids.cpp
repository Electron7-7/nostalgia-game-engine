#include "common/ids.hpp"
#include "common/printing.hpp"

#include <random>
#include <set>

static std::random_device sRandomSeed;
static std::mt19937 sIdEngine(sRandomSeed());
static std::uniform_int_distribution<id_t> sIdDistribution{(id_t)UniqueIDs::front, (id_t)UniqueIDs::back};
static std::set<ID> sExistingIDs{};
static constexpr unsigned int cMaxUniqueIDs{(id_t)UniqueIDs::back - (id_t)UniqueIDs::Reserved::back}; // Probably one less than the real max but it shouldn't matter

id_t ID::Generate()
{
    sIdEngine.seed(sRandomSeed());
    return sIdDistribution(sIdEngine);
}

id_t UniqueIDs::Generate()
{
    if(sExistingIDs.size() == cMaxUniqueIDs)
    {
        print_warning("UniqueIDs::Generate - Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUniqueIDs);
        sExistingIDs.clear();
    }
    id_t new_id{ID::Generate()};
    do { new_id = ID::Generate(); } while ( !sExistingIDs.insert(new_id).second );
    return new_id;
}

void UniqueIDs::Clear()
{ sExistingIDs.clear(); }

bool UniqueIDs::Push(const ID& id)
{ return sExistingIDs.insert(id).second; }

bool UniqueIDs::Erase(const ID& id)
{ return (sExistingIDs.erase(id) != 0); }

bool UniqueIDs::Contains(const ID& id)
{ return sExistingIDs.contains(id); }

bool UniqueIDs::IsReserved(const ID& id)
{ return id < front; }

bool UniqueIDs::GetReservedID(const std::string& in_name, ID& out)
{
    for(auto [uid, name] : Reserved::EmbeddedResourceNames)
        { if(!name.compare(in_name)) { out = uid; return true; } }
    return false;
}

bool UniqueIDs::GetReservedName(const ID& in, std::string& out)
{
    if(!Reserved::EmbeddedResourceNames.contains(in))
        { return false; }
    out = Reserved::EmbeddedResourceNames.at(in);
    return true;
}
