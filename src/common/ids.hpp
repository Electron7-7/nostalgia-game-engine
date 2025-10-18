#ifndef IDS_H
#define IDS_H

#include "embedded/names.hpp"
#include "hash.hpp"

#include <string>
#include <format>
#include <map>

typedef unsigned int id_t;

struct ID
{
public:
    constexpr ID(id_t id = ID::Invalid): id_(id) {}
    explicit constexpr ID(const char* from_name): id_(ConstexprHash(from_name)) {}
    explicit constexpr ID(const std::string& from_name): id_(ConstexprHash(from_name)) {}

    constexpr operator const id_t&() const
    { return id_; }

    constexpr bool invalid() const
    { return id_ == ID::Invalid; }

    static constexpr id_t Invalid = static_cast<unsigned int>(-1); // Same as `UINT_MAX`
    static constexpr id_t front   = 0;
    static constexpr id_t back    = Invalid - 1;

private:
    friend struct std::formatter<ID>;
    id_t id_{ID::Invalid};
};

template<>
struct std::formatter<ID> : std::formatter<id_t>
{
    auto format(ID id, std::format_context& ctx) const
    { return std::formatter<id_t>::format(id.id_, ctx); }
};

namespace UniqueIDs
{
    extern ID Generate();
    extern bool PopLast();
    extern bool Contains(ID UID);
    extern bool IsReserved(ID UID);
    extern bool GetReservedIDName(ID UID, std::string& Output);
    extern bool Erase(ID UID);
    extern bool Push(ID UID);
    extern void Clear();

    // Reserved UIDs
    namespace Reserved
    {
        constexpr ID Player = 0x1;
        // Images
        constexpr ID i_Missing    = 0x2;
        constexpr ID i_LightDebug = 0x3;
        constexpr ID i_COMP04_5   = 0x4;
        constexpr ID i_LolBit     = 0x5;
        // Models
        constexpr ID m_Error  = 0x6;
        constexpr ID m_Cube   = 0x7;
        constexpr ID m_Ramiel = 0x8;
        // Fonts
        constexpr ID f_Verdana    = 0x9;
        constexpr ID f_DejaVuSans = 0xA;
        constexpr ID f_Audiowide  = 0xB;
        // Boundaries
        constexpr ID front = Player;
        constexpr ID back  = f_Audiowide;

        const std::map<std::string, ID>
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
    constexpr ID front = (Reserved::back + 1);
    constexpr ID back  = (ID::Invalid);
};

namespace BindingIDs
{
    constexpr ID KeyZERO         = 0x00;
    constexpr ID KeyONE          = 0x01;
    constexpr ID KeyTWO          = 0x02;
    constexpr ID KeyTHREE        = 0x03;
    constexpr ID KeyFOUR         = 0x04;
    constexpr ID KeyFIVE         = 0x05;
    constexpr ID KeySIX          = 0x06;
    constexpr ID KeySEVEN        = 0x07;
    constexpr ID KeyEIGHT        = 0x08;
    constexpr ID KeyNINE         = 0x09;
    constexpr ID KeyA            = 0x0A;
    constexpr ID KeyB            = 0x0B;
    constexpr ID KeyC            = 0x0C;
    constexpr ID KeyD            = 0x0D;
    constexpr ID KeyE            = 0x0E;
    constexpr ID KeyF            = 0x0F;
    constexpr ID KeyG            = 0x10;
    constexpr ID KeyH            = 0x11;
    constexpr ID KeyI            = 0x12;
    constexpr ID KeyJ            = 0x13;
    constexpr ID KeyK            = 0x14;
    constexpr ID KeyL            = 0x15;
    constexpr ID KeyM            = 0x16;
    constexpr ID KeyN            = 0x17;
    constexpr ID KeyO            = 0x18;
    constexpr ID KeyP            = 0x19;
    constexpr ID KeyQ            = 0x1A;
    constexpr ID KeyR            = 0x1B;
    constexpr ID KeyS            = 0x1C;
    constexpr ID KeyT            = 0x1D;
    constexpr ID KeyU            = 0x1E;
    constexpr ID KeyV            = 0x1F;
    constexpr ID KeyW            = 0x20;
    constexpr ID KeyX            = 0x21;
    constexpr ID KeyY            = 0x22;
    constexpr ID KeyZ            = 0x23;
    constexpr ID KeyLEFTSHIFT    = 0x24;
    constexpr ID KeyRIGHTSHIFT   = 0x25;
    constexpr ID KeyLEFTCONTROL  = 0x26;
    constexpr ID KeyRIGHTCONTROL = 0x27;
    constexpr ID KeyLEFTALT      = 0x28;
    constexpr ID KeyRIGHTALT     = 0x29;
    constexpr ID KeyFUNCTION     = 0x2A;
    constexpr ID KeyLEFTSUPER    = 0x2B;
    constexpr ID KeyRIGHTSUPER   = 0x2C;
    constexpr ID KeyENTER        = 0x2D;
    constexpr ID KeyBACKSPACE    = 0x3E;
    constexpr ID KeyTAB          = 0x3F;
    constexpr ID KeySPACE        = 0x30;
    constexpr ID KeyESC          = 0x31;
    constexpr ID MouseLEFT       = 0x32;
    constexpr ID MouseRIGHT      = 0x33;
    constexpr ID MouseMIDDLE     = 0x34;
    constexpr ID MouseMotionX    = 0x35;
    constexpr ID MouseMotionY    = 0x36;

    constexpr ID front = KeyZERO;
    constexpr ID back  = MouseMotionY;
    constexpr ID end   = back + 1;

    constexpr ID KeysFront = KeyZERO;
    constexpr ID KeysBack  = KeyESC;
    constexpr ID KeysEnd   = KeysBack + 1;
    constexpr ID KeysCount = KeysEnd - KeysFront;

    constexpr ID MouseButtonsFront = MouseLEFT;
    constexpr ID MouseButtonsBack  = MouseMIDDLE;
    constexpr ID MouseButtonsEnd   = MouseButtonsBack + 1;
    constexpr ID MouseButtonsCount = MouseButtonsEnd - MouseButtonsFront;

    constexpr ID MouseMotionFront = MouseMotionX;
    constexpr ID MouseMotionBack  = MouseMotionY;
    constexpr ID MouseMotionEnd   = MouseMotionBack + 1;
    constexpr ID MouseMotionCount = MouseMotionEnd - MouseMotionFront;
};

#endif // IDS_H
