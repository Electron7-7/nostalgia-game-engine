#ifndef UID_H
#define UID_H

#include "core/id.hpp"
#include <sys/types.h>

namespace UID
{
    enum class ReservedType : int
    { NotReserved = 0, Player, Mesh, Texture, Cubemap, Font, };

    constexpr ID
    // Reserved Non-Specific UIDs
        a_Player           {0x00},
        a_EditorViewport   {0x01},
        a_Global2DViewport {0x02},
        a_Global3DViewport {0x03},
    // Reserved Texture UIDs
        t_Missing        {0x10},
        t_LightDebug     {0x11},
        t_COMP04_5       {0x12},
        t_LolBit         {0x13},
    // Reserved Cubemap Texture UIDs
        t_ShittySkybox   {0x20},
    // Reserved Mesh UIDs
        m_Error          {0x30},
        m_Cube           {0x31},
        m_Ramiel         {0x32},
        m_Camera3D       {0x33},
    // Reserved Font UIDs
        f_Verdana        {0x40},
        f_DejaVuSans     {0x41},
        f_Audiowide      {0x42};
    constexpr uint
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
    bool IsReserved(uint);
    ReservedType GetReservedType(uint);
};

#endif // UID_H
