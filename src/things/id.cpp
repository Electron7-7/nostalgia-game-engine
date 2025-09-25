#include "id.hpp"

#include <set>
#include <random>

static std::random_device s_RandomDevice;
static std::set<size_t> s_ExistingIDs = {};

id_t ID::GetNewID()
{
    std::mt19937 engine(s_RandomDevice());
    std::uniform_int_distribution<id_t> distribution(IDs::End + 1);

    id_t return_value = distribution(engine);
    while(s_ExistingIDs.contains(return_value))
        { return_value = distribution(engine); }

    s_ExistingIDs.insert(return_value);
    return return_value;
}

void ID::ClearIDs()
{ s_ExistingIDs.clear(); }

bool ID::AddID(id_t id)
{
    if(Contains(id))
        { return false; }
    s_ExistingIDs.insert(id);
    return true;
}

bool ID::RemoveID(id_t id)
{ return(s_ExistingIDs.erase(id)); }

bool ID::Contains(id_t id)
{ return s_ExistingIDs.contains(id); }
