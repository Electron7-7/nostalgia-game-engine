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

ThingData::ThingData(const std::string& name, const std::string& type_name)
: name(name), type_(ConstexprHash(type_name))
{
    if(!ThingFactory::IsThing(type_))
        { PRINT_ERROR("ThingData::ThingData({}, {}, std::vector<ThingVar>) - Type #{} is an invalid type!", name, type_name, type_) }
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

bool ThingData::GetReference(ID& output, const std::string& name) const
{
    if(auto assert_var = AssertVariable(name, {ThingVar::eReferenceE, ThingVar::eReferenceT});
        SafeStatus::Check(assert_var.Status()))
    {
        unsigned int id = output;
        if(StringToNum<unsigned int>(id, assert_var.Data()->value))
            { output = id; return true; }
    }
    return false;
}

bool ThingData::GetBool(bool& output, const std::string& name) const
{
    if(auto assert_var = AssertVariable(name, {ThingVar::eBool});
        SafeStatus::Check(assert_var.Status()))
    {
        if(!assert_var.Data()->value.compare("true"))
            { return output = true; }
        else if(!assert_var.Data()->value.compare("false"))
            { return !(output = false); }
    }
    return false;
}

bool ThingData::GetString(std::string& output, const std::string& name) const
{
    auto assert_var = AssertVariable(name, {ThingVar::eString});
    if(!SafeStatus::Check(assert_var.Status()))
        { return false; }
    output = assert_var.Data()->value;
    return true;
}

SafeReturn<ThingData::VarIter_t> ThingData::AssertVariable(const std::string& name, const std::set<penum_t>& types) const
{
    auto iter = std::find(variables.cbegin(), variables.cend(), name);
    auto status = Status::NO_ERR;
    if(iter == variables.cend())
        { status = Status::ThingDataINVALID_VARIABLE_NAME; }
    else if(!types.contains(iter->type))
        { status = Status::ThingDataINVALID_VARIABLE_TYPE; }
    else if(iter->value.empty())
        { status = Status::ThingDataVARIABLE_VALUE_EMPTY; }
    return {iter, status};
}
