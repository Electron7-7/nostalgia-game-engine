#ifndef UID_H
#define UID_H

namespace UID
{
    enum class ReservedType : int
    { NotReserved = -1, Image = 0, Model = 1, Font = 2, Other = 3 };

    constexpr uint
    // Reserved UID Boundaries
        reserved_front {0x0001},
        reserved_back  {0xFFFF}, // The total number of reserved UIDs allowed (65535)
    // Reserved UID Type Boundaries. Every type gets 16383 unique IDs (except "other", which gets 16382)
        images_front   {0x0001}, // 00001
        images_back    {0x4000}, // 16384
        models_front   {0x4001}, // 16385
        models_back    {0x8000}, // 32768
        fonts_front    {0x8001}, // 32769
        fonts_back     {0xC000}, // 49152
        other_front    {0xC001}, // 49153
        other_back     {0xFFFF}, // 65535
    // Normal UID Boundaries
        front {reserved_back + 1},
        back  {static_cast<uint>(-1)};

    constexpr ID
    // Reserved Engine Image UIDs
        i_Missing          {images_front + 0},
        i_LightDebug       {images_front + 1},
        i_COMP04_5         {images_front + 2},
        i_LolBit           {images_front + 3},
        i_ShittySkyboxXn   {images_front + 4},
        i_ShittySkyboxXp   {images_front + 5},
        i_ShittySkyboxYp   {images_front + 6},
        i_ShittySkyboxYn   {images_front + 7},
        i_ShittySkyboxZn   {images_front + 8},
        i_ShittySkyboxZp   {images_front + 9},
    // Reserved Engine Model UIDs
        m_Error            {models_front + 0},
        m_Cube             {models_front + 1},
        m_Quad             {models_front + 2},
        m_Ramiel           {models_front + 3},
        m_Camera3D         {models_front + 4},
        m_DebugAxis        {models_front + 5},
    // Reserved Engine Font UIDs
        f_Verdana          {fonts_front  + 0},
        f_DejaVuSans       {fonts_front  + 1},
        f_Audiowide        {fonts_front  + 2},
    // Reserved Engine Other UIDs
        o_Player           {other_front  + 0},
        o_RootViewport     {other_front  + 1};

    Error Generate(ID& outUID);
    bool Contains(ID inUID);
    bool Erase(ID inUID);
    bool Push(ID inUID);
    void Clear();

    uint GetRandom();
    bool IsReserved(ID inRUID);
    ReservedType GetReservedType(ID inRUID);
    /// Used to manually reserve an RUID (reserved UID).
    Error CreateReservedUID(ReservedType inType, ID inRUID);
    /**
     * Generates a random RUID within the given `ReservedType` category.
     *
     * This function should only be used *after* any and all manually reserved RUIDs have been created. If you
     * manually pick and reserve another RUID after generating a random one, there is a nonzero chance for the
     * generated RUID to conflict with the manually selected RUID. The odds of this happening are miniscule, but
     * not zero; while the programmer in me wants to remove this function to avoid conflicts, the creative
     * in me wants to leave it in to give the engine some "personality". That side won, today.
    **/
    Error GenerateReservedUID(ReservedType inType, ID& outRUID);
    Error EraseReservedUID(ID inRUID);
}

#endif // UID_H
