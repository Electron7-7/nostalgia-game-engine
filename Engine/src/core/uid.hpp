#ifndef UID_H
#define UID_H

#include "core/id.hpp"
#include <sys/types.h>

namespace UID
{
    constexpr ID
    // Reserved Actor UIDs
        a_Player {(uint)0},
    // Reserved Image UIDs
        i_Missing    {1},
        i_LightDebug {2},
        i_COMP04_5   {3},
        i_LolBit     {4},
    // Reserved Model UIDs
        m_Error      {5},
        m_Cube       {6},
        m_Ramiel     {7},
    // Reserved Font UIDs
        f_Verdana    {8},
        f_DejaVuSans {9},
        f_Audiowide  {10};
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
