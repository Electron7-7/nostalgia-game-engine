#define LOCK LockGuard<RMutex> lock{sMutex}

using Distribution_t = std::uniform_int_distribution<uint>;

static RMutex sMutex{};
static IdSet_t sActiveUIDs{};
static Distribution_t sDistribution{UID::front, UID::back};
static std::random_device sRandomSeed{};
static std::mt19937 sIdEngine{sRandomSeed()};

static constexpr uint s_cMaxUIDs{UID::back - UID::front};

Error UID::Generate(Shared<Thing> inThing)
{
    LOCK;
    if(ID uid{Generate()}; not uid.invalid())
    {
        inThing->mUID = uid;
        return OK;
    }
    return ERR_FULL;
}

ID UID::Generate()
{
    LOCK;
    if(sActiveUIDs.size() == s_cMaxUIDs)
    {
        print_warning("Somehow, you have hit the maximum number of UIDs for this set ({}). Please consider removing a few. Please.", s_cMaxUIDs);
        return ID::Invalid;
    }
    uint new_id{GetRandom()};
    while(not sActiveUIDs.insert(new_id).second)
        { new_id = GetRandom(); }
    return new_id;
}

bool UID::Contains(ID inID)
{
    LOCK;
    return sActiveUIDs.contains(inID);
}

bool UID::Erase(ID inID)
{
    LOCK;
    if(sActiveUIDs.size() == 1 and sActiveUIDs.contains(inID))
    {
        sActiveUIDs.clear();
        return true;
    }
    return sActiveUIDs.erase(inID);
}

bool UID::Push(ID inID)
{
    LOCK;
    return sActiveUIDs.insert(inID).second;
}

void UID::Clear()
{
    LOCK;
    sActiveUIDs.clear();
}

uint UID::GetRandom()
{
    LOCK;
    sIdEngine.seed(sRandomSeed());
    return sDistribution(sIdEngine);
}
