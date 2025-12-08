#ifndef INPUT_BINDINGS_H
#define INPUT_BINDINGS_H

#include "core/id.hpp"
#include "common/string_concepts.hpp"
#include "frozen/set.h"
#include "math/concepts.hpp"
#include "for_each_macro.hpp"
#include "va_args_count.h"

#define ENUM_LOOKUP(NAME, COUNT_NAME, ENUMS...) \
    inline static constinit const uint COUNT_NAME{VA_ARGS_COUNT(ENUMS)}; \
    inline static constinit const frozen::set<KeyID, COUNT_NAME> NAME{FOR_EACH(ADD_COMMA,ENUMS)};
#define ADD_COMMA(ITEM) ITEM,

struct KeyID final : public ConstexprID_t
{ using ConstexprID_t::ConstexprID_t; };

struct Key
{
    struct Modifier
    {
    public:
        constexpr Modifier() noexcept = default;

        ushort id{0b000000};

        static constexpr ushort max{6};

        constexpr operator ushort() const noexcept { return id; }

    private:
        friend struct Key;
        constexpr Modifier(ushort inID) noexcept: id{inID} {}
    };

    struct Modifiers
    {
    public:
        constexpr Modifiers() noexcept = default;
        template<std::same_as<Modifier>... Mods> requires (sizeof...(Mods) <= Modifier::max)
            constexpr Modifiers(Mods... inMods) noexcept
            { for(const Modifier& mod : {inMods...}) {mods_ |= static_cast<ushort>(mod);} }

        void set(Modifier inModifier, bool isActive)
        {
            if(isActive) { mods_ |=  static_cast<ushort>(inModifier); }
            else         { mods_ &= ~static_cast<ushort>(inModifier); }
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

    static constinit const Modifier
        Mod_Shift,
        Mod_Control,
        Mod_Alt,
        Mod_Super,
        Mod_CapsLock,
        Mod_NumLock,
        Mods_None,
        Mods_All;

    static constexpr KeyID
        // Numbers
        Zero{"0"}, One{"1"}, Two{"2"},   Three{"3"}, Four{"4"},
        Five{"5"}, Six{"6"}, Seven{"7"}, Eight{"8"}, Nine{"9"},
        // Letters
        A{"A"}, B{"B"}, C{"C"}, D{"D"}, E{"E"}, F{"F"}, G{"G"}, // Hey,
        H{"H"}, I{"I"}, J{"J"}, K{"K"}, L{"L"}, M{"M"}, N{"N"}, // that's
        O{"O"}, P{"P"}, Q{"Q"}, R{"R"}, S{"S"}, T{"T"}, U{"U"}, // funny.
        V{"V"}, W{"W"}, X{"X"}, Y{"Y"}, Z{"Z"},
        // Modifiers
        LeftShift{"LShift"},  LeftControl{"LCtrl"},  LeftAlt{"LAlt"},  LeftSuper{"LSuper"},
        RightShift{"RShift"}, RightControl{"RCtrl"}, RightAlt{"RAlt"}, RightSuper{"RSuper"},
        // Whitespace
        Enter{"Enter"}, Backspace{"Backspace"}, Tab{"Tab"}, Space{"Space"},
        // Other
        Escape{"Esc"},
        // Mouse Buttons
        MouseLeft{"LMouse"}, MouseRight{"RMouse"}, MouseMiddle{"Mouse3"};

    template<typename T>
        static constexpr bool IsKey(const T& inUnknownArg) noexcept
        { return false; }

    template<typename T> requires std::is_same_v<T, KeyID>
        static constexpr bool IsKey(const T& inKeyIDArg) noexcept
        { return keys.contains(inKeyIDArg); }

    template<Number T>
        static constexpr bool IsKey(const T& inKeyIDArg) noexcept
        { return keys.contains(inKeyIDArg); }

    template<StringType T>
        static constexpr bool IsKey(const T& inKeyNameArg) noexcept
        { return keys.contains(ConstexprHash(inKeyNameArg)); }

    ENUM_LOOKUP(keys, KeysCount,
        Zero,One,Two,Three,Four,Five,Six,Seven,Eight,Nine,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        LeftShift,LeftControl,LeftAlt,LeftSuper,RightShift,RightControl,RightAlt,RightSuper,
        Enter,Backspace,Tab,Space,Escape,MouseLeft,MouseRight,MouseMiddle)
};

inline constinit const Key::Modifier Key::Mod_Shift    {0b000001};
inline constinit const Key::Modifier Key::Mod_Control  {0b000010};
inline constinit const Key::Modifier Key::Mod_Alt      {0b000100};
inline constinit const Key::Modifier Key::Mod_Super    {0b001000};
inline constinit const Key::Modifier Key::Mod_CapsLock {0b010000};
inline constinit const Key::Modifier Key::Mod_NumLock  {0b100000};
inline constinit const Key::Modifier Key::Mods_None    {0b000000};
inline constinit const Key::Modifier Key::Mods_All     {0b111111};

#undef ENUM_LOOKUP
#undef ADD_COMMA
#endif // INPUT_BINDINGS_H
