#ifndef UID_H
#define UID_H

#include "core/id.hpp"
#include <sys/types.h>

namespace UID
{
    constexpr ID
    // Reserved Player UID
        a_Player {(uint)0x0},
    // Reserved Image UIDs
        i_Missing    {0x1},
        i_LightDebug {0x2},
        i_COMP04_5   {0x3},
        i_LolBit     {0x4},
    // Reserved Model UIDs
        m_Error      {0x11},
        m_Cube       {0x12},
        m_Ramiel     {0x13},
    // Reserved Font UIDs
        f_Verdana    {0x21},
        f_DejaVuSans {0x22},
        f_Audiowide  {0x23};
    constexpr uint
    // Reserved UID Boundaries
        reserved_back  {f_Audiowide},
        reserved_front {a_Player},
    // UID Boundaries
        back  {static_cast<uint>(-1)},
        front {reserved_back + 1};

    uint Generate();
    bool PopLast();
    bool Contains(uint);
    bool Erase(uint);
    bool Push(uint);
    void Clear();
    bool IsReserved(uint);
};

#endif // UID_H
