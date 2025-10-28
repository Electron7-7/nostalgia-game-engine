#include "thing_data.hpp"
#include "thing_variable.hpp"
#include "things/thing_factory.hpp"
#include "common/globals.hpp"
#include "common/printing.hpp"
#include "common/colors.hpp"

const ThingData ThingData::PlayerDefaults(
    "Default Player",
    ThingType::NostalgiaPlayer,
    UniqueIDs::Reserved::Player,
    {
        ThingVar{glm::vec3(0.0f), "Rotation"},
        ThingVar{glm::vec3(0.0f), "Origin"},
    });

ThingData::ThingData() = default;

ThingData::ThingData(const std::string& _name, const std::string& type_name)
: name{_name}, type_{ConstexprHash(type_name)}
{
    if(!g_pThingFactory->IsThing(type_))
        { print_error("ThingData::ThingData({}, {}, std::vector<ThingVar>) - Type #{} is an invalid type!", name, type_name, type_); }
}

ThingData::ThingData(const std::string& _name, ID _type, ID _id, const std::vector<ThingVar>& _variables)
: name{_name}, uid{_id}, variables(_variables), type_{_type}
{
    if(type_ == ThingType::NostalgiaPlayer) // ThingData::PlayerDefaults triggers the error message bc of when it's constructed
        { return; }
    if(!g_pThingFactory->IsThing(type_))
        { print_error("ThingData::ThingData({}, {}, {}, std::vector<ThingVar>) - Type '{}' is an invalid type!", name, type_, uid, g_pThingFactory->GetTypeName(type_)); }
}

ThingData::ThingData(const std::string& _name, ID _type, const std::vector<ThingVar>& _variables)
: name{_name}, variables(_variables), type_{_type} {}

bool ThingData::RemoveVariable(const std::string& _name)
{
    if(auto iter = std::find(variables.cbegin(), variables.cend(), _name);
        iter != variables.cend())
    { variables.erase(iter); return true; }
    return false;
}

void ThingData::AddVariable(const ThingVar& variable, const std::string& _name)
{
    RemoveVariable(_name);
    variables.push_back(variable); variables.back().name = _name;
}

void ThingData::AddVariable(const std::string& _name, const std::string& value_as_string, const penum_t& type)
{
    RemoveVariable(_name);
    variables.emplace_back();
    variables.back().name  = _name;
    variables.back().value = value_as_string;
    variables.back().type  = type;
}

std::string ThingData::log(bool colored, bool indent) const
{
    std::string log;
    if(colored)
        { log = std::format("{1}(Type: {3}, ID: {5}){0} {2}{4}{0}\n", Sty::Reset, Sty::Bold + Fg::Yellow, Sty::Bold + Fg::Green, g_pThingFactory->GetTypeName(type_), name, (uid.invalid()) ? "ID::Invalid" : std::to_string((uint)uid)); }
    else
        {  log = std::format("({}) {} [{}]\n", g_pThingFactory->GetTypeName(type_), name, uid);  }
    for(const auto& var : variables)
    {
        if(indent)
            { log.push_back('\t'); }
        log.append(std::format("{}\n", var.log(colored)));
    }
    return log;
}

ID ThingData::type() const
{ return type_; }

bool ThingData::set_type(const std::string& type_name)
{ return set_type(ConstexprHash(type_name)); }

bool ThingData::set_type(ID type)
{
    type_ = type;
    if(!g_pThingFactory->IsThing(type))
    {
        print_warning("ThingData::set_type - The specified type '{}' is not a known type! This data structure will not be used if its type name is invalid (meaning, you won't see '{}' in the Theatre)", g_pThingFactory->GetTypeName(type), name);
        return false;
    }
    return true;
}

void ThingData::clear()
{ *this = ThingData(); }

bool ThingData::GetReference(ID& output, const std::string& _name) const
{
    if(auto assert_var = AssertVariable(_name, ThingVar::eReference); assert_var.Check())
        { output = assert_var.Data()->reference_id; return true; }
    return false;
}

bool ThingData::GetPrettyEnum(penum_t& output, const std::string& _name) const
{
    if(auto assert_var = AssertVariable(_name, ThingVar::ePrettyEnum); assert_var.Check())
        { output = assert_var.Data()->pretty_enum; return true; }
    return false;
}

bool ThingData::GetBoolean(bool& output, const std::string& _name) const
{
    if(auto assert_var = AssertVariable(_name, ThingVar::eBool); assert_var.Check())
    {
        if(!assert_var.Data()->value.compare(gTrue))
            { return output = true; }
        else if(!assert_var.Data()->value.compare(gFalse))
            { return !(output = false); }
    }
    return false;
}

bool ThingData::GetString(std::string& output, const std::string& _name) const
{
    auto assert_var = AssertVariable(_name, {ThingVar::eString});
    if(!assert_var.Check() || assert_var.Data()->value.empty())
        { return false; }
    output = assert_var.Data()->value;
    return true;
}

SafeReturn<ThingData::VarIter_t> ThingData::AssertVariable(const std::string& _name, const penum_t& type) const
{
    auto iter = std::find(variables.cbegin(), variables.cend(), _name);
    auto status = Status::NO_ERR;
    if(iter == variables.cend())
        { status = Status::ThingDataINVALID_VARIABLE_NAME; }
    else if(iter->type != type)
        { status = Status::ThingDataINVALID_VARIABLE_TYPE; }
    else if(iter->value.empty())
        { status = Status::ThingDataVARIABLE_VALUE_EMPTY; }
    return {iter, status};
}
