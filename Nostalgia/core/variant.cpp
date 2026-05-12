#include "./variant.hpp"
#include "theatre/theatre.hpp"

using TypeNames_t = std::unordered_map<std::string, Variant::Type>;

bool Variant::_to_bool() const
{ return not is_zero(); }

template<typename T> requires std::same_as<T, glm::vec4> or std::same_as<T, glm::quat>
    static std::string s_GetFormattedString(const T* inVal, int inSize)
    {
        if(not inVal)
            { return ""; }
        inSize = (inSize > 4) ? 4 : inSize;
        std::string _output{};
        for(int i{0}; i < inSize; ++i)
        {
            float _value{0.0f};
            if(not glm::isnan(inVal->operator[](i)))
                { _value = inVal->operator[](i); }
            if(i)
                { _output += ", "; }
            _output += std::format("{}", _value);
        }
        return _output;
    }

std::string Variant::_to_string() const
{
    switch(_type)
    {
    case NIL:
        return "<null>";
    case BOOL:
        return *std::get_if<bool>(&_data) ? GlobalConstants::str_true : GlobalConstants::str_false;
    case STRING:
        return *std::get_if<std::string>(&_data);
    case INT:
        return std::format("{}", *std::get_if<int>(&_data));
    case FLOAT:
        return std::format("{}", static_cast<float>(*std::get_if<double>(&_data)));
    case VECTOR2:
        return s_GetFormattedString(std::get_if<glm::vec4>(&_data), 2);
    case VECTOR3:
        return s_GetFormattedString(std::get_if<glm::vec4>(&_data), 3);
    case VECTOR4:
        return s_GetFormattedString(std::get_if<glm::vec4>(&_data), 4);
    case QUATERNION:
        return s_GetFormattedString(std::get_if<glm::quat>(&_data), 4);
    case THING:
        return std::get_if<ThingData>(&_data)->thing->to_string();
    default:
        return std::format("<{}>", type_name(_type));
    }
}

Sarg Variant::type_name(Variant::Type inType)
{
    switch(inType)
    {
    case NIL:
        return GlobalConstants::VariantType::str_Nil;
    case STRING:
        return GlobalConstants::VariantType::str_String;
    case BOOL:
        return GlobalConstants::VariantType::str_Bool;
    case INT:
        return GlobalConstants::VariantType::str_Int;
    case FLOAT:
        return GlobalConstants::VariantType::str_Float;
    case VECTOR2:
        return GlobalConstants::VariantType::str_Vector2;
    case VECTOR3:
        return GlobalConstants::VariantType::str_Vector3;
    case VECTOR4:
        return GlobalConstants::VariantType::str_Vector4;
    case QUATERNION:
        return GlobalConstants::VariantType::str_Quaternion;
    case THING:
        return GlobalConstants::VariantType::str_Thing;
    default:
        return GlobalConstants::str_empty;
    }
}

// Stolen from [Godot](core/variant/variant.cpp:177)
static TypeNames_t _generate_type_names() noexcept
{
    TypeNames_t _type_names{};
    for(int i{0}; i < Variant::TYPE_MAX ; ++i)
    {
        auto _type{static_cast<Variant::Type>(i)};
        _type_names[Variant::type_name(_type)] = _type;
    }
    return _type_names;
}

// Stolen from [Godot](core/variant/variant.cpp:185)
Variant::Type Variant::type_by_name(Sarg inName)
{
    static TypeNames_t _type_names{_generate_type_names()};
    if(auto found_it{_type_names.find(inName)}; found_it != _type_names.end())
        { return found_it->second; }
    return Variant::TYPE_MAX;
}

// Stolen from [Godot](core/variant/variant.cpp:192) and [Godot](core/variant/variant.cpp:535)
static bool _are_variant_types_convertible(Variant::Type inFrom, Variant::Type inTo, bool inIsStrict)
{
    if(inFrom == inTo or inTo == Variant::NIL)
        { return true; }
    else if(inFrom == Variant::TYPE_MAX or inTo == Variant::TYPE_MAX)
        { return false; }
    switch(inTo)
    {
    case Variant::STRING:
        return (inFrom != Variant::THING);
    case Variant::BOOL:
        return inFrom == Variant::INT
            or inFrom == Variant::FLOAT
            or inFrom == Variant::STRING;
        break;
    case Variant::INT:
        return inFrom == Variant::BOOL
            or inFrom == Variant::FLOAT
            or inFrom == Variant::STRING;
        break;
    case Variant::FLOAT:
        return inFrom == Variant::BOOL
            or inFrom == Variant::INT
            or inFrom == Variant::STRING;
        break;
    case Variant::VECTOR2:
    case Variant::VECTOR3:
    case Variant::VECTOR4:
    case Variant::QUATERNION:
    case Variant::THING:
    default:
        return false;
    }
}

bool Variant::is_convertible(Variant::Type inFrom, Variant::Type inTo)
{ return _are_variant_types_convertible(inFrom, inTo, false); }

bool Variant::is_convertible_strict(Variant::Type inFrom, Variant::Type inTo)
{ return _are_variant_types_convertible(inFrom, inTo, true); }

std::string Variant::get_theatre_file_string() const
{
    switch(_type)
    {
    case STRING:
        return std::format("'{}'", _to_string());
    case BOOL:
    case INT:
    case FLOAT:
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
    case QUATERNION:
        return std::format("[{}]", _to_string());
    case THING:
        if(auto _thing_data{std::get_if<ThingData>(&_data)}; _thing_data and not _thing_data->uid.invalid())
            { return std::format("<{}>", _thing_data->thing->name()); }
        [[fallthrough]];
    case NIL:
    default:
        return GlobalConstants::str_empty;
    }
}

bool Variant::is_null() const
{ return _type != THING or not std::get_if<ThingData>(&_data)->thing; }

bool Variant::is_zero() const
{
    switch(_type)
    {
    case NIL:
        return true;
    case BOOL:
        return not *std::get_if<bool>(&_data);
    case INT:
        return not *std::get_if<int>(&_data);
    case FLOAT:
        return not *std::get_if<double>(&_data);
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
        return *std::get_if<glm::vec4>(&_data) == glm::vec4{};
    case QUATERNION:
        return *std::get_if<glm::quat>(&_data) == glm::quat{};
    case THING:
        return std::get_if<ThingData>(&_data)->thing->invalid();
    default:
        return false;
    }
}

Variant::operator bool() const
{ return not is_zero(); }

Variant::operator int() const
{ return _to_int<int>(); }

Variant::operator uint() const
{ return _to_int<uint>(); }

Variant::operator ID() const
{
    if(_type == INT)
        { return {_to_int<uint>()}; }
    else if(_type == THING)
        { return std::get_if<ThingData>(&_data)->uid; }
    else if(_type == STRING)
        { return Theatre::Current()->GetUID(*std::get_if<std::string>(&_data)); }
    return {ID::Invalid};
}

Variant::operator float() const
{ return _to_float<float>(); }

Variant::operator double() const
{ return _to_float<double>(); }

Variant::operator std::string() const
{ return _to_string(); }

Variant::operator glm::vec2() const
{
    switch(_type)
    {
    default:
        return glm::vec2{};
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
        {
            const auto* _vec{std::get_if<glm::vec4>(&_data)};
            return glm::vec2{_vec->operator[](0), _vec->operator[](1)};
        }
    case QUATERNION:
        {
            const auto* _quat{std::get_if<glm::quat>(&_data)};
            return glm::vec2{_quat->operator[](0), _quat->operator[](1)};
        }
    }
}

Variant::operator glm::vec3() const
{
    switch(_type)
    {
    default:
        return glm::vec3{};
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
        {
            const auto* _vec{std::get_if<glm::vec4>(&_data)};
            return glm::vec3{_vec->operator[](0), _vec->operator[](1), _vec->operator[](2)};
        }
    case QUATERNION:
        {
            const auto* _quat{std::get_if<glm::quat>(&_data)};
            return glm::vec3{_quat->operator[](0), _quat->operator[](1), _quat->operator[](2)};
        }
    }
}

Variant::operator glm::vec4() const
{
    switch(_type)
    {
    default:
        return glm::vec4{};
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
        return *std::get_if<glm::vec4>(&_data);
    case QUATERNION:
        {
            // 'quat' instead of '_quat' so it'd fit on my screen in one line :)
            const auto* quat{std::get_if<glm::quat>(&_data)};
            return glm::vec4{quat->operator[](0), quat->operator[](1), quat->operator[](2), quat->operator[](3)};
        }
    }
}

Variant::operator glm::quat() const
{
    switch(_type)
    {
    default:
        return glm::quat{};
    case QUATERNION:
        return *std::get_if<glm::quat>(&_data);
    case VECTOR2:
    case VECTOR3:
    case VECTOR4:
        {
            const auto* _vec{std::get_if<glm::vec4>(&_data)};
            return glm::quat{_vec->operator[](0), _vec->operator[](1), _vec->operator[](2), _vec->operator[](3)};
        }
    }
}

Variant::operator BitMask() const
{ return BitMask{_to_int<int>()}; }

Variant::operator FileData() const
{
    if(_type == STRING)
        { return FileData{*std::get_if<std::string>(&_data)}; }
    return FileData{};
}

Variant::operator Shared<Thing>() const
{
    if(_type == THING)
        { return std::get_if<ThingData>(&_data)->thing; }
    else if(_type == STRING)
        { return Theatre::Current()->GetThing(operator std::string()); }
    else if(_type == INT or _type == FLOAT)
        { return Theatre::Current()->GetThing(operator ID()); }
    return Thing::Invalid();
}

Variant::Variant(bool inValue):
    _type{BOOL}, _data{inValue} {}

Variant::Variant(int inValue):
    _type{INT}, _data{inValue} {}

Variant::Variant(uint inValue):
    _type{INT}, _data{int(inValue)} {}

Variant::Variant(ID inValue):
    Variant{inValue()} {}

Variant::Variant(float inValue):
    _type{FLOAT}, _data{inValue} {}

Variant::Variant(double inValue):
    Variant{float(inValue)} {}

Variant::Variant(Sarg inValue):
    _type{STRING}, _data{inValue} {}

Variant::Variant(Farg<glm::vec2> inValue):
    _type{VECTOR2}, _data{glm::vec4{inValue, 0.0f, 0.0f}} {}

Variant::Variant(Farg<glm::vec3> inValue):
    _type{VECTOR3}, _data{glm::vec4{inValue, 0.0f}} {}

Variant::Variant(Farg<glm::vec4> inValue):
    _type{VECTOR4}, _data{inValue} {}

Variant::Variant(Farg<glm::quat> inValue):
    _type{QUATERNION}, _data{inValue} {}

Variant::Variant(BitMask inValue):
    _type{INT}, _data{inValue.get()} {}

Variant::Variant(Farg<FileData> inValue):
    _type{STRING}, _data{inValue.filepath()} {}

Variant::Variant(Farg<Shared<Thing>> inValue):
    _type{THING}, _data{(inValue) ? ThingData{inValue->uid(), inValue} : ThingData{}} {}
