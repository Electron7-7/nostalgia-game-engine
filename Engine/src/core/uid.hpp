#ifndef UID_H
#define UID_H

#include "id.hpp"
#include "frozen/map.h"
#include <set>

class UID
{
public:
    // Reserved UIDs:
    //
    // Actors
    static constexpr ID a_Player{0x1};
    // Images
    static constexpr ID i_Missing{0x2};
    static constexpr ID i_LightDebug{0x3};
    static constexpr ID i_COMP04_5{0x4};
    static constexpr ID i_LolBit{0x5};
    // Models
    static constexpr ID m_Error{0x6};
    static constexpr ID m_Cube{0x7};
    static constexpr ID m_Ramiel{0x8};
    // Fonts
    static constexpr ID f_Verdana{0x9};
    static constexpr ID f_DejaVuSans{0xA};
    static constexpr ID f_Audiowide{0xB};

    // Boundaries
    static constexpr uint reserved_back{f_Audiowide};
    static constexpr uint reserved_front{a_Player};

    static constexpr frozen::map<uint, std::string, reserved_back - 1>
    embedded_data_name_lookup{
        {     i_Missing[], "MissingTexture" },
        {  i_LightDebug[], "LightTexture"   },
        {    i_COMP04_5[], "DoomTexture"    },
        {      i_LolBit[], "LolBitTexture"  },
        {       m_Error[], "ErrorModel"     },
        {        m_Cube[], "DefaultCube"    },
        {      m_Ramiel[], "RamielModel"    },
        {   f_Audiowide[], "Audiowide"      },
        {  f_DejaVuSans[], "DejaVuSans"     },
        {     f_Verdana[], "Verdana"        },
    };

    static uint Generate();
    static bool PopLast();
    static bool Contains(uint);
    static bool Erase(uint);
    static bool Push(uint);
    static void Clear();
    static bool IsReserved(uint);

    template<ID_Adjacent T>
        inline static bool try_GetDataValues(T& ioDataID, std::string& ioDataName)
        {
            for(const auto& [id, name] : embedded_data_name_lookup)
            {
                if(static_cast<uint>(ioDataID) == id || !ioDataName.compare(name))
                {
                    ioDataID = id;
                    ioDataName = name;
                    return true;
                }
            }
            return false;
        }

    // Boundaries
    static constexpr uint back{static_cast<uint>(-1)};
    static constexpr uint front{reserved_back + 1};

private:
    inline static std::set<uint> m_sActiveIDs{};
};

#endif // UID_H
