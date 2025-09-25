#ifndef ID_H
#define ID_H

typedef unsigned int id_t;

namespace IDs
{
    constexpr id_t None        = 0;
    constexpr id_t Player      = 1;
    constexpr id_t iMissing    = 2;
    constexpr id_t iLightDebug = 3;
    constexpr id_t iCOMP04_5   = 4;
    constexpr id_t iLolBit     = 5;
    constexpr id_t mError      = 6;
    constexpr id_t mCube       = 7;
    constexpr id_t mRamiel     = 8;
    constexpr id_t fVerdana    = 9;
    constexpr id_t fDejaVuSans = 10;
    constexpr id_t fAudiowide  = 11;
    constexpr id_t End         = fAudiowide;
}

struct ID
{
    static id_t GetNewID();
    static void ClearIDs();
    static bool AddID(id_t ID);
    static bool RemoveID(id_t ID);
    static bool Contains(id_t ID);
};

#endif // ID_H
