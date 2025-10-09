#ifndef IDS_H
#define IDS_H

#include "embedded/names.hpp"
#include "frozen/map.h"

#include <string>
#include <format>
#include <map>

typedef unsigned int id_t;

struct ID
{
public:
    constexpr ID(const id_t& id = None): id_(id) {}

    constexpr operator const id_t&() const
    { return id_; }

    static constexpr id_t None    = static_cast<unsigned int>(-2); // Same as `UINT_MAX - 1`
    static constexpr id_t Invalid = static_cast<unsigned int>(-1); // Same as `UINT_MAX`
    static constexpr id_t front   = 0;
    static constexpr id_t back    = None - 1;

private:
    friend struct std::formatter<ID>;
    id_t id_ = ID::None;
};

template<>
struct std::formatter<ID> : std::formatter<id_t>
{
    auto format(const ID& id, std::format_context& ctx) const
    { return std::formatter<id_t>::format(id.id_, ctx); }
};

struct UniqueIDs
{
    static ID Generate();
    static bool PopLast();
    static bool Contains(ID UID);
    static bool IsReserved(ID UID);
    static bool GetReservedIDName(ID UID, std::string& Output);
    static bool Erase(ID UID);
    static bool Push(ID UID);
    static void Clear();

    // Reserved UIDs
    struct Reserved
    {
        static constexpr ID Player = 0x1;
        // Images
        static constexpr ID i_Missing    = 0x2;
        static constexpr ID i_LightDebug = 0x3;
        static constexpr ID i_COMP04_5   = 0x4;
        static constexpr ID i_LolBit     = 0x5;
        // Models
        static constexpr ID m_Error  = 0x6;
        static constexpr ID m_Cube   = 0x7;
        static constexpr ID m_Ramiel = 0x8;
        // Fonts
        static constexpr ID f_Verdana    = 0x9;
        static constexpr ID f_DejaVuSans = 0xA;
        static constexpr ID f_Audiowide  = 0xB;
        // Boundaries
        static constexpr ID front = Player;
        static constexpr ID back  = f_Audiowide;

        inline static const std::map<std::string, id_t>
        ResourceNameToUIDMap =
        {
            { Images::Name::Missing,    i_Missing    },
            { Images::Name::LightDebug, i_LightDebug },
            { Images::Name::COMP04_5,   i_COMP04_5   },
            { Images::Name::LolBit,     i_LolBit     },
            { Models::Name::Error,      m_Error      },
            { Models::Name::Cube,       m_Cube       },
            { Models::Name::Ramiel,     m_Ramiel     },
            { Fonts::Name::Audiowide,   f_Audiowide  },
            { Fonts::Name::DejaVuSans,  f_DejaVuSans },
            { Fonts::Name::Verdana,     f_Verdana    },
        };
    };

    // Boundaries
    static constexpr ID front = (Reserved::back + 1);
    static constexpr ID back  = (ID::Invalid);
};

struct BindingIDs
{
public:
    static constexpr bool Exists(ID BindingID)
    { return BindingID < end; } // `ID` stores an `unsigned int`, so `BindingID` can never be negative
    static constexpr bool Exists(const std::string& Name)
    { return cBindingNames.contains(Name); }
    static constexpr ID GetID(const std::string& Name)
    {
        if(!Exists(Name))
            { return ID::Invalid; }
        return cBindingNames.at(Name);
    }
    static constexpr std::string GetName(ID BindingID)
    {
        static constexpr std::string s_cInvalidName = "Invalid InputID";
        for(const auto& [name, id] : cBindingNames)
            { if(BindingID == id) { return name; } }
        return s_cInvalidName;
    }

    static constexpr ID KeyZERO         = 0x00;
    static constexpr ID KeyONE          = 0x01;
    static constexpr ID KeyTWO          = 0x02;
    static constexpr ID KeyTHREE        = 0x03;
    static constexpr ID KeyFOUR         = 0x04;
    static constexpr ID KeyFIVE         = 0x05;
    static constexpr ID KeySIX          = 0x06;
    static constexpr ID KeySEVEN        = 0x07;
    static constexpr ID KeyEIGHT        = 0x08;
    static constexpr ID KeyNINE         = 0x09;
    static constexpr ID KeyA            = 0x0A;
    static constexpr ID KeyB            = 0x0B;
    static constexpr ID KeyC            = 0x0C;
    static constexpr ID KeyD            = 0x0D;
    static constexpr ID KeyE            = 0x0E;
    static constexpr ID KeyF            = 0x0F;
    static constexpr ID KeyG            = 0x10;
    static constexpr ID KeyH            = 0x11;
    static constexpr ID KeyI            = 0x12;
    static constexpr ID KeyJ            = 0x13;
    static constexpr ID KeyK            = 0x14;
    static constexpr ID KeyL            = 0x15;
    static constexpr ID KeyM            = 0x16;
    static constexpr ID KeyN            = 0x17;
    static constexpr ID KeyO            = 0x18;
    static constexpr ID KeyP            = 0x19;
    static constexpr ID KeyQ            = 0x1A;
    static constexpr ID KeyR            = 0x1B;
    static constexpr ID KeyS            = 0x1C;
    static constexpr ID KeyT            = 0x1D;
    static constexpr ID KeyU            = 0x1E;
    static constexpr ID KeyV            = 0x1F;
    static constexpr ID KeyW            = 0x20;
    static constexpr ID KeyX            = 0x21;
    static constexpr ID KeyY            = 0x22;
    static constexpr ID KeyZ            = 0x23;
    static constexpr ID KeyLEFTSHIFT    = 0x24;
    static constexpr ID KeyRIGHTSHIFT   = 0x25;
    static constexpr ID KeyLEFTCONTROL  = 0x26;
    static constexpr ID KeyRIGHTCONTROL = 0x27;
    static constexpr ID KeyLEFTALT      = 0x28;
    static constexpr ID KeyRIGHTALT     = 0x29;
    static constexpr ID KeyFUNCTION     = 0x2A;
    static constexpr ID KeyLEFTSUPER    = 0x2B;
    static constexpr ID KeyRIGHTSUPER   = 0x2C;
    static constexpr ID KeyENTER        = 0x2D;
    static constexpr ID KeyBACKSPACE    = 0x3E;
    static constexpr ID KeyTAB          = 0x3F;
    static constexpr ID KeySPACE        = 0x30;
    static constexpr ID KeyESC          = 0x31;
    static constexpr ID MouseLEFT       = 0x32;
    static constexpr ID MouseRIGHT      = 0x33;
    static constexpr ID MouseMIDDLE     = 0x34;
    static constexpr ID MouseMotionX    = 0x35;
    static constexpr ID MouseMotionY    = 0x36;

    static constexpr ID front = KeyZERO;
    static constexpr ID back  = MouseMotionY;
    static constexpr ID end   = back + 1;

    static constexpr ID KeysFront = KeyZERO;
    static constexpr ID KeysBack  = KeyESC;
    static constexpr ID KeysEnd   = KeysBack + 1;
    static constexpr ID KeysCount = KeysEnd - KeysFront;

    static constexpr ID MouseButtonsFront = MouseLEFT;
    static constexpr ID MouseButtonsBack  = MouseMIDDLE;
    static constexpr ID MouseButtonsEnd   = MouseButtonsBack + 1;
    static constexpr ID MouseButtonsCount = MouseButtonsEnd - MouseButtonsFront;

    static constexpr ID MouseMotionFront = MouseMotionX;
    static constexpr ID MouseMotionBack  = MouseMotionY;
    static constexpr ID MouseMotionEnd   = MouseMotionBack + 1;
    static constexpr ID MouseMotionCount = MouseMotionEnd - MouseMotionFront;

    // Since this map accounts for both upper and lower case letters, we need to add `26` to the size
    static constinit const frozen::map<std::string, ID, end + 26> cBindingNames;
};

#endif // IDS_H
