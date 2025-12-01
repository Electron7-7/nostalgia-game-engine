#ifndef INPUT_BINDINGS_H
#define INPUT_BINDINGS_H

#include "core/type_helpers.hpp"
#include "core/id.hpp"
#include "common/string_concepts.hpp"
#include "frozen/set.h"
#include "for_each_macro.hpp"
#include "math/concepts.hpp"
#include "va_args_count.h"

#define ENUM(ENUM_DEFINITION) static constexpr KeyID ENUM_DEFINITION;
#define ENUMS(ENUMS...) FOR_EACH(ENUM, ENUMS)
#define ENUM_LOOKUP(NAME, COUNT_NAME, ENUMS...) \
    inline static constinit const uint COUNT_NAME{VA_ARGS_COUNT(ENUMS)}; \
    inline static constinit const frozen::set<KeyID, COUNT_NAME> NAME{FOR_EACH(ADD_COMMA,ENUMS)};
#define ADD_COMMA(ITEM) ITEM,

struct KeyID final : public ConstexprID_t
{ using ConstexprID_t::ConstexprID_t; };

typedef FARG(KeyID) KeyArg;

template<class T>
    concept KeyID_t = is_similar<T, KeyID>;

struct Key
{
    ENUMS
    (
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
        MouseLeft{"LMouse"}, MouseRight{"RMouse"}, MouseMiddle{"Mouse3"}
    )

    enum class Modifier : ushort {
        MOD_SHIFT     = 0,
        MOD_CONTROL   = 1,
        MOD_ALT       = 2,
        MOD_SUPER     = 3,
        MOD_CAPS_LOCK = 4,
        MOD_NUM_LOCK  = 5,
    };

    struct Modifiers
    {
        std::array<bool,6> mods{false, false, false, false, false, false};

        bool AnyActive() const
        {
            for(const auto& mod : mods)
                { if(!mod) { return false; } }
            return true;
        }

        bool IsActive(Modifier inMod) const
        { return mods[static_cast<ushort>(inMod)]; }
    };

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

    ENUM_LOOKUP(keys, keys_count,
        Zero,One,Two,Three,Four,Five,Six,Seven,Eight,Nine,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
        LeftShift,LeftControl,LeftAlt,LeftSuper,RightShift,RightControl,RightAlt,RightSuper,
        Enter,Backspace,Tab,Space,Escape,MouseLeft,MouseRight,MouseMiddle)
};

namespace Input
{
    template<typename T>
        constexpr bool IsKey(const T& inArg) noexcept { return Key::IsKey(inArg); }

    constexpr uint KeysCount{Key::keys_count};
}

#undef ENUM
#undef ENUMS
#undef ENUM_LOOKUP
#undef ADD_COMMA
#endif // INPUT_BINDINGS_H
