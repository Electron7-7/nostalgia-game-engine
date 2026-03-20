#include "frozen/set.h"

using Distribution_t = std::uniform_int_distribution<uint>;

static IdSet_t sActiveIDs{};

static constexpr frozen::set<uint, 21> s_cEngineRUIDs{
    UID::i_Missing(),
    UID::i_LightDebug(),
    UID::i_COMP04_5(),
    UID::i_LolBit(),
    UID::i_ShittySkyboxXn(),
    UID::i_ShittySkyboxXp(),
    UID::i_ShittySkyboxYp(),
    UID::i_ShittySkyboxYn(),
    UID::i_ShittySkyboxZn(),
    UID::i_ShittySkyboxZp(),
    UID::m_Error(),
    UID::m_Cube(),
    UID::m_Quad(),
    UID::m_Ramiel(),
    UID::m_Camera3D(),
    UID::m_DebugAxis(),
    UID::f_Verdana(),
    UID::f_DejaVuSans(),
    UID::f_Audiowide(),
    UID::o_Player(),
    UID::o_RootViewport()
};

static IdSet_t
    sActiveImageRUIDs{UID::i_Missing, UID::i_LightDebug, UID::i_COMP04_5, UID::i_LolBit,
        UID::i_ShittySkyboxXn, UID::i_ShittySkyboxXp, UID::i_ShittySkyboxYp,
        UID::i_ShittySkyboxYn, UID::i_ShittySkyboxZn, UID::i_ShittySkyboxZp
    },
    sActiveModelRUIDs{UID::m_Error, UID::m_Cube, UID::m_Quad, UID::m_Ramiel, UID::m_Camera3D, UID::m_DebugAxis},
    sActiveFontRUIDs{UID::f_Verdana, UID::f_DejaVuSans, UID::f_Audiowide},
    sActiveOtherRUIDs{UID::o_Player, UID::o_RootViewport};

static Distribution_t sDistributionUIDs{UID::front, UID::back},
    sDistributionImageRUIDs {UID::images_front, UID::images_back},
    sDistributionModelRUIDs {UID::models_front, UID::models_back},
    sDistributionFontRUIDs  {UID::fonts_front,  UID::fonts_back},
    sDistributionOtherRUIDs {UID::other_front,  UID::other_back};

static constexpr uint s_cMaxUIDs{UID::back - UID::reserved_back},
    s_cMaxImageIDs {UID::images_back - UID::images_front},
    s_cMaxModelIDs {UID::models_back - UID::models_front},
    s_cMaxFontIDs  {UID::fonts_back  - UID::fonts_front},
    s_cMaxOtherIDs {UID::other_back  - UID::other_front};

static std::random_device sRandomSeed{};
static std::mt19937 sIdEngine{sRandomSeed()};

static uint sGetRandom(Distribution_t& ioDistribution)
{
    sIdEngine.seed(sRandomSeed());
    return ioDistribution(sIdEngine);
}

static Error sGenerateUID(ID& outUID, Distribution_t& ioDistribution, IdSet_t& ioActiveSet, uint inMaxActiveIDs)
{
    if(ioActiveSet.size() == inMaxActiveIDs)
    {
        print_warning("Somehow, you have hit the maximum number of UIDs for this set ({}). Please consider removing a few. Please.", inMaxActiveIDs);
        return ERR_FULL;
    }
    uint new_id{sGetRandom(ioDistribution)};
    while(!ioActiveSet.insert(new_id).second)
        { new_id = sGetRandom(ioDistribution); }
    outUID.id(new_id);
    return OK;
}

static Error sRegisterUID(ID inID, IdSet_t& ioActiveSet, uint inMaxActiveIDs)
{
    if(ioActiveSet.size() == inMaxActiveIDs)
        { return ERR_FULL; }
    return (ioActiveSet.insert(inID()).second)
        ? OK : ERR_ALREADY_EXISTS;
}

Error UID::Generate(ID& outUID)
{ return sGenerateUID(outUID, sDistributionUIDs, sActiveIDs, s_cMaxUIDs); }

bool UID::Contains(ID inID)
{ return sActiveIDs.contains(inID); }

bool UID::Erase(ID inID)
{ return sActiveIDs.erase(inID); }

bool UID::Push(ID inID)
{
    return (IsReserved(inID))
        ? false
        : sActiveIDs.insert(inID).second;
}

void UID::Clear()
{ sActiveIDs.clear(); }

uint UID::GetRandom()
{ return sGetRandom(sDistributionUIDs); }

bool UID::IsReserved(ID inID)
{ return inID() < front; }

UID::ReservedType UID::GetReservedType(ID inID)
{
    Farg<uint> uid{inID()};
    if(uid > reserved_back)
        { return ReservedType::NotReserved; }
    else if(uid < models_front)
        { return ReservedType::Image; }
    else if(uid < fonts_front)
        { return ReservedType::Model; }
    else if(uid < other_front)
        { return ReservedType::Font; }
    else if(uid < front)
        { return ReservedType::Other; }
    return ReservedType::NotReserved;
}

Error UID::CreateReservedUID(ReservedType inType, ID inUID)
{
    switch(inType)
    {
    case ReservedType::NotReserved:
    default:
        return ERR_INVALID_TYPE;
    case ReservedType::Image:
        return sRegisterUID(inUID, sActiveImageRUIDs, s_cMaxImageIDs);
    case ReservedType::Model:
        return sRegisterUID(inUID, sActiveModelRUIDs, s_cMaxModelIDs);
    case ReservedType::Font:
        return sRegisterUID(inUID, sActiveFontRUIDs,  s_cMaxFontIDs);
    case ReservedType::Other:
        return sRegisterUID(inUID, sActiveOtherRUIDs, s_cMaxOtherIDs);
    }
}

Error UID::GenerateReservedUID(ReservedType inType, ID& outUID)
{
    switch(inType)
    {
    case ReservedType::NotReserved:
    default:
        return ERR_INVALID_TYPE;
    case ReservedType::Image:
        return sGenerateUID(outUID, sDistributionImageRUIDs, sActiveImageRUIDs, s_cMaxImageIDs);
    case ReservedType::Model:
        return sGenerateUID(outUID, sDistributionModelRUIDs, sActiveModelRUIDs, s_cMaxModelIDs);
    case ReservedType::Font:
        return sGenerateUID(outUID, sDistributionFontRUIDs, sActiveFontRUIDs, s_cMaxFontIDs);
    case ReservedType::Other:
        return sGenerateUID(outUID, sDistributionOtherRUIDs, sActiveOtherRUIDs, s_cMaxOtherIDs);
    }
}

Error UID::EraseReservedUID(ID inUID)
{
    if(s_cEngineRUIDs.contains(inUID()))
        { return ERR_NOT_ALLOWED; }
    switch(GetReservedType(inUID))
    {
    case ReservedType::NotReserved:
    default:
        return ERR_INVALID_TYPE;
    case ReservedType::Image:
        return (sActiveImageRUIDs.erase(inUID)) ? OK : ERR_NOT_FOUND;
    case ReservedType::Model:
        return (sActiveModelRUIDs.erase(inUID)) ? OK : ERR_NOT_FOUND;
    case ReservedType::Font:
        return (sActiveFontRUIDs.erase(inUID))  ? OK : ERR_NOT_FOUND;
    case ReservedType::Other:
        return (sActiveOtherRUIDs.erase(inUID)) ? OK : ERR_NOT_FOUND;
    }
}
