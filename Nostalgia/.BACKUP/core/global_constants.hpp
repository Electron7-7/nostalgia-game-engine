#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

namespace GlobalConstants
{
    // Ready at compile time
    namespace Constexpr
    {
        constinit const char cstr_empty[]{""},
            cstr_NA[]{"N/A"},
            cstr_Invalid[]{"Invalid"},
            cstr_true[]{"true"},
            cstr_false[]{"false"},
            cstr_CommaSpace[]{", "};

        namespace VariantType
        {
            constinit const char cstr_Nil[]{"Nil"},
                cstr_String[]{"String"},
                cstr_Bool[]{"Bool"},
                cstr_Int[]{"Int"},
                cstr_Float[]{"Float"},
                cstr_Vector2[]{"Vector2"},
                cstr_Vector3[]{"Vector3"},
                cstr_Vector4[]{"Vector4"},
                cstr_Quaternion[]{"Quaternion"},
                cstr_Thing[]{"Thing"};
        }
    }

    const std::string str_empty{Constexpr::cstr_empty},
        str_NA{Constexpr::cstr_NA},
        str_Invalid{Constexpr::cstr_Invalid},
        str_true{Constexpr::cstr_true},
        str_false{Constexpr::cstr_false},
        str_CommaSpace{Constexpr::cstr_CommaSpace};

    namespace VariantType
    {
        const std::string str_Nil{Constexpr::VariantType::cstr_Nil},
            str_String{Constexpr::VariantType::cstr_String},
            str_Bool{Constexpr::VariantType::cstr_Bool},
            str_Int{Constexpr::VariantType::cstr_Int},
            str_Float{Constexpr::VariantType::cstr_Float},
            str_Vector2{Constexpr::VariantType::cstr_Vector2},
            str_Vector3{Constexpr::VariantType::cstr_Vector3},
            str_Vector4{Constexpr::VariantType::cstr_Vector4},
            str_Quaternion{Constexpr::VariantType::cstr_Quaternion},
            str_Thing{Constexpr::VariantType::cstr_Thing};
    }
}

#endif // GLOBAL_CONSTANTS_H
