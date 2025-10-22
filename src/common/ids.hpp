#ifndef IDS_H
#define IDS_H

#include "embedded/names.hpp"
#include "frozen/set.h"
#include "frozen/map.h"
#include "common/hash.hpp"

#include <string>
#include <format>

struct ID
{
public:
    constexpr ID() = default;

    constexpr ID(id_t id):
        id_{id}, name_{""} {}

    constexpr ID(const std::string& name):
        id_{ConstexprHash(name.data())}, name_{name} {}

    constexpr operator const id_t&() const
    { return id_; }

    constexpr const std::string& name() const
    { return name_; }
    constexpr const char* c_name() const
    { return name_.data(); }

    constexpr bool invalid() const
    { return id_ == ID::Invalid; }

    static constexpr id_t Invalid {static_cast<unsigned int>(-1)}; // Same as `UINT_MAX`
    static constexpr id_t front   {0};
    static constexpr id_t back    {Invalid - 1};

private:
    id_t id_{ID::Invalid};
    std::string name_{""};
};

template<>
struct std::formatter<ID> : std::formatter<id_t>
{
    auto format(ID id, std::format_context& ctx) const
    { return std::formatter<id_t>::format(static_cast<id_t>(id), ctx); }
};

namespace UniqueIDs
{
    extern id_t Generate();
    extern bool PopLast();
    extern bool Contains(const ID&);
    extern bool Erase(const ID&);
    extern bool Push(const ID&);
    extern void Clear();
    extern bool IsReserved(const ID&);
    extern bool GetReservedID(const std::string& inName, ID& outUID);
    extern bool GetReservedName(const ID& inUID, std::string& outName);

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

        constexpr frozen::map<ID, std::string, 10>
        EmbeddedResourceNames =
        {
            {     i_Missing, Images::Name::Missing    },
            {  i_LightDebug, Images::Name::LightDebug },
            {    i_COMP04_5, Images::Name::COMP04_5   },
            {      i_LolBit, Images::Name::LolBit     },
            {       m_Error, Models::Name::Error      },
            {        m_Cube, Models::Name::Cube       },
            {      m_Ramiel, Models::Name::Ramiel     },
            {   f_Audiowide, Fonts::Name::Audiowide   },
            {  f_DejaVuSans, Fonts::Name::DejaVuSans  },
            {     f_Verdana, Fonts::Name::Verdana     },
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

    constexpr bool IsKey(const ID& BindingID)
    { return KeyIDs.contains(BindingID); }

    constexpr bool IsMouseButton(const ID& BindingID)
    { return MouseButtonIDs.contains(BindingID); }

    constexpr bool IsBinding(const ID& BindingID)
    { return IsKey(BindingID) || IsMouseButton(BindingID); }

    constexpr void GetAllBindingIDs(std::vector<ID>& outVector)
    {
        outVector.reserve(KeyIDsCount + MouseButtonIDsCount);
        std::merge(BindingIDs::KeyIDs.cbegin(), BindingIDs::KeyIDs.cend(),
            BindingIDs::MouseButtonIDs.cbegin(), BindingIDs::MouseButtonIDs.cend(),
            std::inserter(outVector, outVector.begin()));
    }
};

#endif // IDS_H
