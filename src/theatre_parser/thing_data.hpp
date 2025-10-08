#ifndef THING_DATA_H
#define THING_DATA_H

#include "thing_variable.hpp"
#include "string_to_num.hpp"
#include "safe_return.hpp"
#include "things/types.hpp"

#include <vector>
#include <set>
#include <string>
#include <format>

struct ThingData
{
public:
    static const ThingData PlayerDefaults;

    ThingData();
    ThingData(const std::string& Name, const std::string& TypeName);
    ThingData(const std::string& Name, ID Type, ID, const std::vector<ThingVar>& Variables = {});

    void AddVariable(const std::string& Name, const std::string& Value, const penum_t& Type);

    std::string name = "Untitled Thing";
    ID uid = ID::None;
    std::vector<ThingVar> variables = {};

    std::string log(bool colored = false, bool indent_items = false) const;
    ID   type() const;
    bool set_type(const std::string& TypeName);
    bool set_type(ID Type);
    void clear();

    bool GetReference(ID& Output, const std::string& VarName) const;
    bool GetBool(bool& Output, const std::string& VarName) const;
    bool GetString(std::string& Output, const std::string& VarName) const;

    template<typename T>
    bool GetNumber(T& Output, const std::string& VarName) const
    {
        auto assert_var = AssertVariable(VarName, {ThingVar::eNumber});
        if(!SafeStatus::Check(assert_var.Status()))
            { return false; }
        StringToNum<T>(Output, assert_var.Data()->value);
        return true;
    }

private:
    ID type_ = ThingType::Thing;

    typedef std::vector<ThingVar>::const_iterator VarIter_t;
    SafeReturn<VarIter_t> AssertVariable(const std::string& VarName, const std::set<penum_t>& OneOrMoreTypes) const;
};

template<>
struct std::formatter<ThingData> : std::formatter<std::string>
{
    auto format(const ThingData& data, std::format_context& ctx) const
    { return std::formatter<std::string>::format(data.log(), ctx); }
};

#endif // THING_DATA_H
