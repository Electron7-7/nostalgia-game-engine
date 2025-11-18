#ifndef THING_DATA_H
#define THING_DATA_H

#include "thing_variable.hpp"
#include "things/types.hpp"
#include "core/safe_return.hpp"
#include "common/string_concepts.hpp"
#include "common/string_to_num.hpp"
#include "frozen/map.h"
#include "physics/enums.hpp"
#include "rendering/common.hpp"
#include "thirdparty/va_args_count.h"

#include <type_traits>

#define ADD_PENUM(penum) { penum.name(), penum },
#define PENUM_LOOKUP(NAME, ...) \
    constexpr frozen::map<std::string, penum_t, VA_ARGS_COUNT(__VA_ARGS__)/2> NAME = { __VA_ARGS__ }

PENUM_LOOKUP(gPrettyEnumLookup,
    ADD_PENUM(PhysicsBodyMotion::Dynamic)
    ADD_PENUM(PhysicsBodyMotion::Kinematic)
    ADD_PENUM(PhysicsBodyMotion::Static)
    ADD_PENUM(PhysicsBodyShape::Box)
    ADD_PENUM(PhysicsBodyShape::Capsule)
    ADD_PENUM(PhysicsBodyShape::Cylinder)
    ADD_PENUM(PhysicsBodyShape::Sphere)
);

#undef ADD_PENUM
#undef PENUM_LOOKUP

#define ADD_ENUM(_enum) { #_enum, _enum }
#define ADD_PENUM(name, _enum) { name, _enum }
#define ENUM_LOOKUP(NAME, ...) \
    constexpr frozen::map<const char*, int, VA_ARGS_COUNT(__VA_ARGS__)/2> NAME = { __VA_ARGS__ }

ENUM_LOOKUP(gEnumLookup,
    ADD_PENUM("DATA_FORMAT_NONE", RendererCommon::DATA_FORMAT_NONE)
);

#undef ADD_ENUM
#undef ADD_PENUM
#undef ENUM_LOOKUP

struct ThingData
{
public:
    static const ThingData PlayerDefaults;

    ThingData();
    ThingData(const std::string& Name, const std::string& TypeName);
    ThingData(const std::string& Name, ID Type, const std::vector<ThingVar>& Variables = {});
    ThingData(const std::string& Name, ID Type, ID, const std::vector<ThingVar>& Variables = {});

    std::string name{"Untitled Thing"};
    ID uid{};
    std::vector<ThingVar> variables{};

    bool RemoveVariable(const std::string& VariableName);
    void AddVariable(const std::string& Name, const std::string& ValueAsString, const penum_t& Type);
    void AddVariable(const ThingVar& Value, const std::string& Name);

    template<typename T, StringType... Names>
    bool GetVariable(T& output, Names... names) const
    {
        if constexpr(std::is_same_v<std::decay_t<std::string>, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetString(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_same_v<std::decay_t<ID>, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetReference(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_enum_v<T>)
        {
            for(const auto& name : {names...})
            {
                if(gEnumLookup.contains(name))
                    { output = static_cast<T>(gEnumLookup.at(name)); return true; }
            }
            return false;
        }
        else if constexpr(std::is_same_v<std::decay_t<penum_t>, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
            {
                if(auto assert_var = AssertVariable(name, ThingVar::eEnum); assert_var.Check())
                    { output.set(assert_var.Data()->enum_name.data(), assert_var.Data()->enum_value); return true; }
            }
            return false;
        }
        else if constexpr(std::is_same_v<bool, std::decay_t<T>>)
        {
            for(const auto& name : {names...})
                { if(GetBoolean(output, name)) { return true; } }
            return false;
        }
        else if constexpr(std::is_arithmetic_v<T> || GLMContainer<T>)
        {
            for(const auto& name : {names...})
            {
                if(auto assert_var = AssertVariable(name, ThingVar::eNumber); assert_var.Check() &&
                    StringToNum<T>(output, assert_var.Data()->value))
                { return true; }
            }
            return false;
        }
    }

    std::string log(bool colored = false, bool indent_items = false) const;
    ID   type() const;
    bool set_type(const std::string& TypeName);
    bool set_type(ID Type);
    void clear();

private:
    ID type_ = ThingType::Thing;

    typedef std::vector<ThingVar>::const_iterator VarIter_t;
    SafeReturn<VarIter_t> AssertVariable(const std::string& VarName, const penum_t& VarType) const;

    bool GetReference(ID& ReferenceOutput, const std::string& VariableName) const;
    bool GetBoolean(bool& BooleanOutput, const std::string& VariableName) const;
    bool GetString(std::string& StringOutput, const std::string& VariableName) const;
};

template<>
struct std::formatter<ThingData> : std::formatter<std::string>
{
    auto format(const ThingData& data, std::format_context& ctx) const
    { return std::formatter<std::string>::format(data.log(), ctx); }
};

#endif // THING_DATA_H
