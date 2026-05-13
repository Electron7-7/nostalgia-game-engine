static std::uniform_int_distribution<uint> sDistribution{UID::front, UID::back};
static std::random_device sRandomSeed{};
static std::mt19937 sIdEngine{sRandomSeed()};

uint UID::GetRandom()
{
    sIdEngine.seed(sRandomSeed());
    return sDistribution(sIdEngine);
}

IdVec_t UID::GetEmbeddedUIDs()
{
    return {
        t_Missing,
        t_COMP04_5,
        t_LolBit,
        t_ShittySkybox,
        m_Error,
        m_Cube,
        m_Quad,
        m_Ramiel,
        f_Verdana,
        f_DejaVuSans,
        f_Audiowide,
    };
}
