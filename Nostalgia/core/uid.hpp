#ifndef UID_H
#define UID_H

/// Forward Declaration
class Thing;

namespace UID
{
    constexpr uint
    // UID Boundaries
        front {0x00},
        back  {static_cast<uint>(-1)};

    // UIDs for engine-embedded Resources (these are set when their repsective Resource gets registered)
    inline ID
    // Textures
        t_Missing,
        t_LightDebug,
        t_COMP04_5,
        t_LolBit,
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
        o_Player,
        o_RootViewport;

    Error Generate(Shared<Thing> inThing);
    ID    Generate();
    bool  Contains(ID inUID);
    bool  Erase(ID inUID);
    bool  Push(ID inUID);
    void  Clear();
    uint  GetRandom();
}

#endif // UID_H
