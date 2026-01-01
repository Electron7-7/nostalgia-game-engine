#ifndef UID_H
#define UID_H

#include "core/id.hpp"
#include <sys/types.h>

namespace UID
{
    enum class ReservedType : int
    { Player = 1, Model = 2, Image = 3, Font = 4, NotReserved = 0 };

    constexpr ID
    // Reserved Player UID
        a_Player {(uint) 0x0},
        a_EditorCamera  {0x1},
    // Reserved Image UIDs
        i_Missing    {0x11},
        i_LightDebug {0x12},
        i_COMP04_5   {0x13},
        i_LolBit     {0x14},
    // Reserved Model UIDs
        m_Error      {0x21},
        m_Cube       {0x22},
        m_Ramiel     {0x23},
        m_Camera3D   {0x24},
    // Reserved Font UIDs
        f_Verdana    {0x31},
        f_DejaVuSans {0x32},
        f_Audiowide  {0x33};
    constexpr uint
    // Reserved UID Boundaries
        images_front   {i_Missing},
        images_back    {i_LolBit},
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
