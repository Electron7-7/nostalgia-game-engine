#include "thing_data.hpp"
#include "printing.hpp"
#include "thing_variable.hpp"
#include "things/thing_factory.hpp"

const ThingData ThingData::PlayerDefaults(
    "Default Player",
    ThingType::NostalgiaPlayer,
    UniqueIDs::Reserved::Player,
    {
        ThingVar{"Rotation", "0, 0, 0", ThingVar::eNumber},
        ThingVar{"Origin", "0, 0, 0", ThingVar::eNumber},
    });

ThingData::ThingData() = default;

ThingData::ThingData(const std::string& name, const std::string& type_name, ID id)
: name(name), uid(id), type_(ConstexprHash(type_name))
{
    if(!ThingFactory::IsThing(type_))
        { PRINT_ERROR("ThingData::ThingData({}, {}, {}, std::vector<ThingVar>) - Type #{} is an invalid type!", name, type_name, id, type_) }
}

ThingData::ThingData(const std::string& _name, ID _type, ID _id, const std::vector<ThingVar>& variables)
: name(_name), uid(_id), variables(variables), type_(_type)
{
    if(type_ == ThingType::NostalgiaPlayer) // ThingData::PlayerDefaults triggers the error message bc of when it's constructed
        { return; }
    if(!ThingFactory::IsThing(type_))
        { PRINT_ERROR("ThingData::ThingData({}, {}, {}, std::vector<ThingVar>) - Type '{}' is an invalid type!", name, type_, uid, ThingFactory::GetTypeName(type_)) }
}

void ThingData::AddVariable(const std::string& _name, const std::string& data, const penum_t& _type)
{
    auto variable = std::find(variables.begin(), variables.end(), _name);
    if(variable != variables.end())
    {
        variable->name  = _name;
        variable->value =  data;
        variable->type  = _type;
    }
    variables.emplace_back(_name, data, _type);
}

std::string ThingData::log(bool colored, bool indent) const
{
    std::string log;
    if(colored)
        { log = std::format("{1}(Type: {3}, ID: {5}){0} {2}{4}{0}\n", sty::Reset, sty::Bold + fg::Yellow, sty::Bold + fg::Green, ThingFactory::GetTypeName(type_), name, uid); }
    else
        {  log = std::format("({}) {} [{}]\n", ThingFactory::GetTypeName(type_), name, uid);  }
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
    if(!ThingFactory::IsThing(type))
    {
        PRINT_WARNING("ThingData::set_type - The specified type '{}' is not a known type! This data structure will not be used if its type name is invalid (meaning, you won't see '{}' in the Theatre)", ThingFactory::GetTypeName(type), name)
        return false;
    }
    return true;
}

void ThingData::clear()
{ *this = ThingData(); }

#define ASSERT_VARIABLE(LOCAL_VAR, VAR_NAME, VAR_TYPE) \
const auto& LOCAL_VAR = std::find(variables.begin(), variables.end(), VAR_NAME); \
if(LOCAL_VAR == variables.end() || LOCAL_VAR->value.empty()) \
    { return false; } \
else if(LOCAL_VAR->type != VAR_TYPE) \
    { return false; }

bool ThingData::GetReference(ID& output, const std::string& variable_name) const
{
    ASSERT_VARIABLE(variable, variable_name, ThingVar::eReference);
    unsigned int id = output;
    if(StringToNum<unsigned int>(id, variable->value))
        { output = id; return true; }
    return false;
}

bool ThingData::GetBool(bool& real_variable, const std::string& variable_name) const
{
    ASSERT_VARIABLE(variable, variable_name, ThingVar::eBool)
    if(!variable->value.compare("true"))
        { real_variable = true; return true; }
    else if(!variable->value.compare("false"))
        { real_variable = false; return true; }
    return false;
}

bool ThingData::GetString(std::string& real_variable, const std::string& variable_name) const
{
    ASSERT_VARIABLE(variable, variable_name, ThingVar::eString)
    if(variable->value.empty())
        { return false; }
    real_variable = variable->value;
    return true;
}
