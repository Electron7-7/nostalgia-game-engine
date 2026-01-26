#ifndef UID_H
#define UID_H

#include "core/id.hpp"
#include <set>
#include <random>
#include <sys/types.h>

class UID
{
public:
    enum class ReservedType : int
    { NotReserved = 0, Player, Mesh, Texture, Cubemap, Font, };

    static constexpr ID
    // Reserved Non-Specific UIDs
        a_Player         {0x01},
        a_EditorViewport {0x02},
        a_RootViewport   {0x03},
    // Reserved Texture UIDs
        t_Missing          {0x10}, // 16
        t_LightDebug       {0x11}, // 17
        t_COMP04_5         {0x12}, // 18
        t_LolBit           {0x13}, // 19
    // Reserved Cubemap Texture UIDs
        t_ShittySkybox     {0x20}, // 32 - 38
    // Reserved Mesh UIDs
        m_Error            {0x30}, // 48
        m_Cube             {0x31}, // 49
        m_Quad             {0x32}, // 50
        m_Ramiel           {0x33}, // 51
        m_Camera3D         {0x34}, // 52
        m_DebugAxis        {0x35}, // 53
    // Reserved Font UIDs
        f_Verdana          {0x40}, // 64
        f_DejaVuSans       {0x41}, // 65
        f_Audiowide        {0x42}; // 66

    static constexpr uint
    // Reserved UID Boundaries
        textures_front {t_Missing},
        textures_back  {t_LolBit},
        cubemap_back   {t_ShittySkybox},
        cubemap_front  {t_ShittySkybox},
        models_front   {m_Error},
        models_back    {m_Camera3D},
        fonts_front    {f_Verdana},
        fonts_back     {f_Audiowide},
        reserved_front {a_Player},
        reserved_back  {f_Audiowide},
    // UID Boundaries
        front {reserved_back + 1},
        back  {static_cast<uint>(-1)};

    uint Generate();
    bool PopLast();
    bool Contains(uint);
    bool Erase(uint);
    bool Push(uint);
    void Clear();

    static uint GetRandom();
    static bool IsReserved(uint);
    static ReservedType GetReservedType(uint);

private:
    std::set<uint> mActiveIDs{};

    static std::uniform_int_distribution<uint> m_sIdDistribution;
};

#endif // UID_H
