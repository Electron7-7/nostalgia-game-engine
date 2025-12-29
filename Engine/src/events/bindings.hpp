#ifndef INPUT_BINDINGS_H
#define INPUT_BINDINGS_H

#include "core/id.hpp"
#include "frozen/set.h"

struct KeyID : public ID { using ID::ID; };

// I split up the namespace member definitions so if you need to focus on a specific member/members, you can
// easily collapse all the others instead of scrolling around for ages.

// `Modifier` and `Modifiers` struct definitions.
namespace Key
{
    struct Modifier
    {
    public:
        constexpr Modifier() noexcept = default;
        constexpr Modifier(ushort inID) noexcept: id{inID} {}

        ushort id{0b000000};

        constexpr operator ushort() const noexcept { return id; }

        static constexpr ushort count{6};
    };

    struct Modifiers
    {
    public:
        constexpr Modifiers() noexcept = default;

        template<std::same_as<Modifier>... Mods> requires (sizeof...(Mods) <= Modifier::count)
            constexpr Modifiers(Mods... inMods) noexcept
            {
                for(const Modifier& mod : {inMods...})
                    { set(mod, true); }
            }

        void set(Modifier inModifier, bool isActive)
        {
            if(isActive) { mods_ |=  inModifier; }
            else         { mods_ &= ~inModifier; }
        }

        void enable(Modifier inModifier)
        { set(inModifier, true); }

        void disable(Modifier inModifier)
        { set(inModifier, false); }

        bool has(Modifier inMod) const
        { return mods_ == (mods_ | inMod); }

    private:
        ushort mods_{0};
    };
}

// Constexpr key, modifier, `AllKeys`, and `KeysCount` definitions.
namespace Key
{
    // Keys.
    constexpr KeyID
        // Numbers
        Zero{"0"}, One{"1"}, Two{"2"},   Three{"3"}, Four{"4"},
        Five{"5"}, Six{"6"}, Seven{"7"}, Eight{"8"}, Nine{"9"},
        // Letters
        A{"A"}, B{"B"}, C{"C"}, D{"D"}, E{"E"}, F{"F"}, G{"G"}, // Hey,
        H{"H"}, I{"I"}, J{"J"}, K{"K"}, L{"L"}, M{"M"}, N{"N"}, // that's
        O{"O"}, P{"P"}, Q{"Q"}, R{"R"}, S{"S"}, T{"T"}, U{"U"}, // funny.
        V{"V"}, W{"W"}, X{"X"}, Y{"Y"}, Z{"Z"},
        // Function Keys
        F1{"F1"}, F2{"F2"}, F3{"F3"}, F4{"F4"},   F5{"F5"},   F6{"F6"},
        F7{"F7"}, F8{"F8"}, F9{"F9"}, F10{"F10"}, F11{"F11"}, F12{"F12"},
        // Modifiers
        LeftShift{"LShift"},  LeftControl{"LCtrl"},  LeftAlt{"LAlt"},  LeftSuper{"LSuper"},
        RightShift{"RShift"}, RightControl{"RCtrl"}, RightAlt{"RAlt"}, RightSuper{"RSuper"},
        // Whitespace
        Enter{"Enter"}, Delete{"Delete"}, Backspace{"Backspace"}, Tab{"Tab"}, Space{"Space"},
        // Navigation
        Left{"Left"}, Right{"Right"}, Up{"Up"},     Down{"Down"},
        Home{"Home"}, End{"End"},     PgUp{"PgUp"}, PgDn{"PgDn"},
        // Other
        Escape{"Esc"},
        // Mouse Buttons
        MouseLeft{"LMouse"}, MouseRight{"RMouse"}, MouseMiddle{"Mouse3"};

    // Modifiers.
    constexpr Modifier
        Mod_Shift    {0b000001},
        Mod_Control  {0b000010},
        Mod_Alt      {0b000100},
        Mod_Super    {0b001000},
        Mod_CapsLock {0b010000},
        Mod_NumLock  {0b100000},
        Mods_None    {0b000000},
        Mods_All     {0b111111};

    // Total count of `KeyID` variables.
    inline constinit const size_t KeysCount{73};

    // `constexpr` container of all `KeyID` variables.
    inline constinit const frozen::set<KeyID, KeysCount> Keys
    {
        Zero,One,Two,Three,Four,Five,Six,Seven,Eight,Nine,
        A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
        LeftShift,LeftControl,LeftAlt,LeftSuper,RightShift,RightControl,RightAlt,RightSuper,
        Enter,Delete,Backspace,Tab,Space,Up,Down,Left,Right,Home,End,PgUp,PgDn,Escape,
        MouseLeft,MouseRight,MouseMiddle,
    };
}

// Function template definitions (e.g: `IsKey`)
namespace Key
{
    template<typename T>
        constexpr bool IsKey(Farg<T> inUnknownArg) noexcept
        { return false; }

    template<>
        constexpr bool IsKey(Farg<KeyID> inKeyIDArg) noexcept
        { return Keys.contains(inKeyIDArg); }

    template<>
        constexpr bool IsKey(Farg<std::string> inKeyNameArg) noexcept
        { return Keys.contains(ConstexprHash(inKeyNameArg)); }
};

constexpr const char* debug_GetKeyName(KeyID inKeyID)
{
    switch(inKeyID[])
    {
    case Key::Zero[]:
        return "Zero";
    case Key::One[]:
        return "One";
    case Key::Two[]:
        return "Two";
    case Key::Three[]:
        return "Three";
    case Key::Four[]:
        return "Four";
    case Key::Five[]:
        return "Five";
    case Key::Six[]:
        return "Six";
    case Key::Seven[]:
        return "Seven";
    case Key::Eight[]:
        return "Eight";
    case Key::Nine[]:
        return "Nine";
    case Key::A[]:
        return "A";
    case Key::B[]:
        return "B";
    case Key::C[]:
        return "C";
    case Key::D[]:
        return "D";
    case Key::E[]:
        return "E";
    case Key::F[]:
        return "F";
    case Key::G[]:
        return "G";
    case Key::H[]:
        return "H";
    case Key::I[]:
        return "I";
    case Key::J[]:
        return "J";
    case Key::K[]:
        return "K";
    case Key::L[]:
        return "L";
    case Key::M[]:
        return "M";
    case Key::N[]:
        return "N";
    case Key::O[]:
        return "O";
    case Key::P[]:
        return "P";
    case Key::Q[]:
        return "Q";
    case Key::R[]:
        return "R";
    case Key::S[]:
        return "S";
    case Key::T[]:
        return "T";
    case Key::U[]:
        return "U";
    case Key::V[]:
        return "V";
    case Key::W[]:
        return "W";
    case Key::X[]:
        return "X";
    case Key::Y[]:
        return "Y";
    case Key::Z[]:
        return "Z";
    case Key::F1[]:
        return "F1";
    case Key::F2[]:
        return "F2";
    case Key::F3[]:
        return "F3";
    case Key::F4[]:
        return "F4";
    case Key::F5[]:
        return "F5";
    case Key::F6[]:
        return "F6";
    case Key::F7[]:
        return "F7";
    case Key::F8[]:
        return "F8";
    case Key::F9[]:
        return "F9";
    case Key::F10[]:
        return "F10";
    case Key::F11[]:
        return "F11";
    case Key::F12[]:
        return "F12";
    case Key::LeftShift[]:
        return "LeftShift";
    case Key::LeftControl[]:
        return "LeftControl";
    case Key::LeftAlt[]:
        return "LeftAlt";
    case Key::LeftSuper[]:
        return "LeftSuper";
    case Key::RightShift[]:
        return "RightShift";
    case Key::RightControl[]:
        return "RightControl";
    case Key::RightAlt[]:
        return "RightAlt";
    case Key::RightSuper[]:
        return "RightSuper";
    case Key::Enter[]:
        return "Enter";
    case Key::Delete[]:
        return "Delete";
    case Key::Backspace[]:
        return "Backspace";
    case Key::Tab[]:
        return "Tab";
    case Key::Space[]:
        return "Space";
    case Key::Up[]:
        return "Up";
    case Key::Down[]:
        return "Down";
    case Key::Left[]:
        return "Left";
    case Key::Right[]:
        return "Right";
    case Key::Home[]:
        return "Home";
    case Key::End[]:
        return "End";
    case Key::PgUp[]:
        return "PgUp";
    case Key::PgDn[]:
        return "PgDn";
    case Key::Escape[]:
        return "Escape";
    case Key::MouseLeft[]:
        return "MouseLeft";
    case Key::MouseRight[]:
        return "MouseRight";
    case Key::MouseMiddle[]:
        return "MouseMiddle";
    default:
        return "N/A";
    };
}

#endif // INPUT_BINDINGS_H
