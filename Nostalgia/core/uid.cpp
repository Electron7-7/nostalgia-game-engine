static std::uniform_int_distribution<uint> sDistribution{UID::front, UID::back};
static std::random_device sRandomSeed{};
static std::mt19937 sIdEngine{sRandomSeed()};

uint UID::GetRandom()
{
    sIdEngine.seed(sRandomSeed());
    return sDistribution(sIdEngine);
}
