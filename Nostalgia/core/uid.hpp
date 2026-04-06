#ifndef UID_H
#define UID_H

namespace UID
{
    constexpr uint
    // UID Boundaries
        front {0x00},
        back  {static_cast<uint>(-1)},
        max_size{UID::back - UID::front};

    // UIDs for engine-embedded Resources (these are set when their repsective Resource gets registered)
    inline ID
    // Textures
        i_Missing,
        i_LightDebug,
        i_COMP04_5,
        i_LolBit,
        t_ShittySkybox,
    // Meshes
        m_Error,
        m_Cube,
        m_Quad,
        m_Ramiel,
        m_Camera3D,
        m_DebugAxis,
    // Fonts
        f_Verdana,
        f_DejaVuSans,
        f_Audiowide,
    // Other
        o_RootViewport;

    uint GetRandom();
};

#endif // UID_H
