#ifndef IDS_H
#define IDS_H

#include "embedded/names.hpp"
#include "frozen/set.h"
#include "common/hash.hpp"

#include <string>
#include <format>
#include <map>

typedef unsigned int id_t;

struct ID
{
public:
    constexpr ID() = default;

    constexpr ID(id_t id):
        id_{id}, name_{""} {}

    explicit constexpr ID(const std::string& from_name):
        id_{ConstexprHash(from_name.data())}, name_{from_name} {}

    constexpr operator const id_t&() const
    { return id_; }

    constexpr const std::string& name() const
    { return name_; }

    constexpr bool invalid() const
    { return id_ == ID::Invalid; }

    static constexpr id_t Invalid {static_cast<unsigned int>(-1)}; // Same as `UINT_MAX`
    static constexpr id_t front   {0};
    static constexpr id_t back    {Invalid - 1};

private:
    id_t id_{ID::Invalid};
    std::string name_{"ID::Invalid"};
};

template<>
struct std::formatter<ID> : std::formatter<id_t>
{
    auto format(ID id, std::format_context& ctx) const
    { return std::formatter<id_t>::format(static_cast<id_t>(id), ctx); }
};

template<typename T>
concept CanBeID = requires{
    (std::is_same_v<T,id_t> || std::is_same_v<T,ID>) &&
    (std::is_same_v<T,const char*> || std::is_same_v<T,std::string>);
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
    constexpr ID KeyZERO         {"0"};
    constexpr ID KeyONE          {"1"};
    constexpr ID KeyTWO          {"2"};
    constexpr ID KeyTHREE        {"3"};
    constexpr ID KeyFOUR         {"4"};
    constexpr ID KeyFIVE         {"5"};
    constexpr ID KeySIX          {"6"};
    constexpr ID KeySEVEN        {"7"};
    constexpr ID KeyEIGHT        {"8"};
    constexpr ID KeyNINE         {"9"};
    constexpr ID KeyA            {"A"};
    constexpr ID KeyB            {"B"};
    constexpr ID KeyC            {"C"};
    constexpr ID KeyD            {"D"};
    constexpr ID KeyE            {"E"};
    constexpr ID KeyF            {"F"};
    constexpr ID KeyG            {"G"};
    constexpr ID KeyH            {"H"};
    constexpr ID KeyI            {"I"};
    constexpr ID KeyJ            {"J"};
    constexpr ID KeyK            {"K"};
    constexpr ID KeyL            {"L"};
    constexpr ID KeyM            {"M"};
    constexpr ID KeyN            {"N"};
    constexpr ID KeyO            {"O"};
    constexpr ID KeyP            {"P"};
    constexpr ID KeyQ            {"Q"};
    constexpr ID KeyR            {"R"};
    constexpr ID KeyS            {"S"};
    constexpr ID KeyT            {"T"};
    constexpr ID KeyU            {"U"};
    constexpr ID KeyV            {"V"};
    constexpr ID KeyW            {"W"};
    constexpr ID KeyX            {"X"};
    constexpr ID KeyY            {"Y"};
    constexpr ID KeyZ            {"Z"};
    constexpr ID KeyLEFTSHIFT    {"LShift"};
    constexpr ID KeyRIGHTSHIFT   {"RShift"};
    constexpr ID KeyLEFTCONTROL  {"LCtrl"};
    constexpr ID KeyRIGHTCONTROL {"RCtrl"};
    constexpr ID KeyLEFTALT      {"LAlt"};
    constexpr ID KeyRIGHTALT     {"RAlt"};
    constexpr ID KeyFUNCTION     {"Fn"};
    constexpr ID KeyLEFTSUPER    {"LSuper"};
    constexpr ID KeyRIGHTSUPER   {"RSuper"};
    constexpr ID KeyENTER        {"Enter"};
    constexpr ID KeyBACKSPACE    {"Backspace"};
    constexpr ID KeyTAB          {"Tab"};
    constexpr ID KeySPACE        {"Space"};
    constexpr ID KeyESC          {"Escape"};
    constexpr ID MouseLEFT       {"LeftMouse"};
    constexpr ID MouseRIGHT      {"RightMouse"};
    constexpr ID MouseMIDDLE     {"MiddleMouse"};

    constexpr uint KeyIDsCount{50};
    constexpr uint MouseButtonIDsCount{3};

    // TODO: Expand this list
    constexpr frozen::set<ID, KeyIDsCount>
    KeyIDs {
        KeyZERO, KeyONE, KeyTWO, KeyTHREE, KeyFOUR, KeyFIVE, KeySIX, KeySEVEN, KeyEIGHT, KeyNINE,
        KeyLEFTSHIFT, KeyRIGHTSHIFT, KeyLEFTCONTROL,  KeyLEFTALT,  KeyFUNCTION,  KeyENTER, KeyESC,
        KeyLEFTSUPER, KeyRIGHTSUPER, KeyRIGHTCONTROL, KeyRIGHTALT, KeyBACKSPACE, KeySPACE, KeyTAB,
        KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL, KeyM,
        KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,
    };

    // TODO: Expand this list
    constexpr frozen::set<ID, MouseButtonIDsCount>
    MouseButtonIDs {
        MouseLEFT, MouseRIGHT, MouseMIDDLE,
    };

    template<CanBeID T>
    constexpr bool IsKey(T binding)
    { return KeyIDs.contains(ID{binding}); }

    template<CanBeID T>
    constexpr bool IsMouseButton(T binding)
    { return MouseButtonIDs.contains(ID{binding}); }

    template<CanBeID T>
    constexpr bool IsBinding(T binding)
    { return IsKey(binding) || IsMouseButton(binding); }

    constexpr void GetAllBindingIDs(std::vector<ID>& output)
    {
        output.reserve(KeyIDsCount + MouseButtonIDsCount);
        std::merge(BindingIDs::KeyIDs.cbegin(), BindingIDs::KeyIDs.cend(),
            BindingIDs::MouseButtonIDs.cbegin(), BindingIDs::MouseButtonIDs.cend(),
            std::inserter(output, output.begin()));
    }
};

#endif // IDS_H
