#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

namespace GlobalConstants
{
    /// Ready at compile time
    namespace Init
    {
        constinit const char cstr_empty[]{""},
            cstr_NA[]{"N/A"},
            cstr_Invalid[]{"Invalid"},
            cstr_true[]{"true"},
            cstr_false[]{"false"};

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

    const std::string str_empty{Init::cstr_empty},
        str_NA{Init::cstr_NA},
        str_Invalid{Init::cstr_Invalid},
        str_true{Init::cstr_true},
        str_false{Init::cstr_false};

    namespace VariantType
    {
    const std::string str_Nil{Init::VariantType::cstr_Nil},
        str_String{Init::VariantType::cstr_String},
        str_Bool{Init::VariantType::cstr_Bool},
        str_Int{Init::VariantType::cstr_Int},
        str_Float{Init::VariantType::cstr_Float},
        str_Vector2{Init::VariantType::cstr_Vector2},
        str_Vector3{Init::VariantType::cstr_Vector3},
        str_Vector4{Init::VariantType::cstr_Vector4},
        str_Quaternion{Init::VariantType::cstr_Quaternion},
        str_Thing{Init::VariantType::cstr_Thing};
    }
}

#endif // GLOBAL_CONSTANTS_H
