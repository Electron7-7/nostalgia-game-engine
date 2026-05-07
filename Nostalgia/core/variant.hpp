#ifndef VARIANT_H
#define VARIANT_H

// Forward Declaration
class Thing;

class Variant
{
public:
    enum Type : short
    {
        NIL,
        BOOL,
        INT,
        FLOAT,
        STRING,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        QUATERNION,
        // COLOR,
        THING,
        TYPE_MAX,
    };

    inline Type type() const
    { return _type; }

    static Sarg type_name(Variant::Type inType);
    static Variant::Type type_by_name(Sarg inName);
    static bool is_convertible(Variant::Type inFrom, Variant::Type inTo);
    static bool is_convertible_strict(Variant::Type inFrom, Variant::Type inTo);

    std::string get_theatre_file_string() const;

    bool is_null() const;
    bool is_zero() const;

    operator bool() const;
    operator int() const;
    operator uint() const;
    operator ID() const;
    operator float() const;
    operator double() const;
    operator std::string() const;
    operator glm::vec2() const;
    operator glm::vec3() const;
    operator glm::vec4() const;
    operator glm::quat() const;
    operator BitMask() const;
    operator FileData() const;
    operator Shared<Thing>() const;

    template<IsEnum T>
        operator T() const
        {
            switch(_type)
            {
            case STRING:
                {
                    long _value{};
                    if(EnumRegistry::GetEnum(operator std::string(), _value) == OK)
                        { return static_cast<T>(_value); }
                }
                [[fallthrough]];
            default:
                return T{};
            case INT:
            case FLOAT:
                return static_cast<T>(_to_int<int>());
            }
        }

    Variant(bool);
    Variant(int);
    Variant(uint);
    Variant(ID);
    Variant(float);
    Variant(double);
    Variant(Sarg);
    Variant(Farg<glm::vec2>);
    Variant(Farg<glm::vec3>);
    Variant(Farg<glm::vec4>);
    Variant(Farg<glm::quat>);
    Variant(BitMask);
    Variant(Farg<FileData>);
    Variant(Farg<Shared<Thing>>);

    template<IsEnum T>
        Variant(T inEnum):
            Variant(static_cast<int>(inEnum)) {}

    // This code is stolen directly from Godot because I feel like it'll help avoid some headaches down the line.
    // Original Comment in 'godot/core/variant/variant.h:430':
    //      Make sure Variant is not implicitly cast when accessing it with bracket notation (GH-49469).
    Variant& operator[](Farg<Variant>) = delete;
    Farg<Variant> operator[](Farg<Variant>) const = delete;

private:
    struct ThingData
    {
        ID uid{ID::Invalid};
        Shared<Thing> thing{nullptr};
    };

    Type _type{NIL};
    std::variant<bool, int, double, glm::vec4, glm::quat, std::string, ThingData> _data;

    template<typename T> requires std::integral<T>
        T _to_int() const
        {
            switch(_type)
            {
            default:
                return 0;
            case BOOL:
                return static_cast<T>(*std::get_if<bool>(&_data));
            case INT:
                return T(*std::get_if<int>(&_data));
            case FLOAT:
                return T(*std::get_if<double>(&_data));
            case STRING:
                T _out{0};
                StringToNum(_out, *std::get_if<std::string>(&_data));
                return _out;
            }
        }

    template<typename T> requires std::floating_point<T>
        T _to_float() const
        {
            switch(_type)
            {
            default:
                return 0;
            case BOOL:
                return static_cast<T>(*std::get_if<bool>(&_data));
            case INT:
                return T(*std::get_if<int>(&_data));
            case FLOAT:
                return T(*std::get_if<double>(&_data));
            case STRING:
                T _out{0.0};
                StringToNum(_out, *std::get_if<std::string>(&_data));
                return _out;
            }
        }

    bool _to_bool() const;
    std::string _to_string() const;

    // This code is (mostly) stolen from Godot because I feel like it'll help avoid some headaches down the line.
    // Original Comment in 'godot/core/variant/variant.h:398':
    //      Avoid accidental conversion. If you reached this point, it's because you most likely forgot to
    //      dereference a Variant pointer (so add * like this: *variant_pointer).
    Variant(const Variant *)  = delete;
    Variant(const Variant **) = delete;
};

template<typename T>
    concept Variant_t = std::constructible_from<Variant, T> and std::convertible_to<Variant, T>;

#endif // VARIANT_H
