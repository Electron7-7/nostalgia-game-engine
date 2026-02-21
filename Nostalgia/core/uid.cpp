std::uniform_int_distribution<uint> UID::m_sIdDistribution{UID::front, UID::back};

static std::random_device sRandomSeed{};
static std::mt19937 sIdEngine{sRandomSeed()};
static constexpr uint cMaxUID{UID::back - UID::reserved_back}; // Probably one less than the real max but it shouldn't matter

uint UID::GetRandom()
{
    sIdEngine.seed(sRandomSeed());
    return m_sIdDistribution(sIdEngine);
}

uint UID::Generate()
{
    if(mActiveIDs.size() == cMaxUID)
    {
        print_warning("Somehow, you have hit the maximum number of unique IDs ({}), so I'm gonna take the liberty of removing them all from the set :)", cMaxUID);
        mActiveIDs.clear();
    }
    uint new_id{UID::GetRandom()};
    while(!mActiveIDs.insert(new_id).second)
        { new_id = UID::GetRandom(); }
    return new_id;
}

void UID::Clear()
{ mActiveIDs.clear(); }

bool UID::Push(uint id)
{
    return (IsReserved(id))
        ? false
        : mActiveIDs.insert(id).second;
}

bool UID::Erase(uint id)
{ return mActiveIDs.erase(id); }

UID::ReservedType UID::GetReservedType(uint id)
{
    if(id > reserved_back)
        { return ReservedType::NotReserved; }
    else if(id == a_Player)
        { return ReservedType::Player; }
    else if(id < cubemap_front)
        { return ReservedType::Texture; }
    else if(id < models_front)
        { return ReservedType::Cubemap; }
    else if(id < fonts_front)
        { return ReservedType::Mesh; }
    else if(id < front)
        { return ReservedType::Font; }
    return ReservedType::NotReserved;
}

bool UID::IsReserved(uint id)
{ return id < front; }

bool UID::Contains(uint id)
{ return mActiveIDs.contains(id); }
