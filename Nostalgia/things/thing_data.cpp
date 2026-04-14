#include "./thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"

#define FIND_VAR(VAR_NAME, VECTOR) \
    std::find_if(VECTOR.begin(), VECTOR.end(), \
        [VAR_NAME](Farg<ThingVariable> var_it) \
            { return not var_it.name.compare(VAR_NAME); } )

#define FOUND_VAR(VAR_NAME) \
    auto found_it{FIND_VAR(VAR_NAME, variables)}; found_it != variables.end()

#define FOUND_CHILD(CHILD_NAME) \
    auto found_it{FIND_VAR(CHILD_NAME, children_variables)}; found_it != children_variables.end()

using namespace TheatreFile;

Farg<ThingVariable> ThingData::_get_variable(std::initializer_list<std::string> inNames) const
{
    static ThingVariable invalid_variable{};
    for(FAUTO name : inNames)
    {
        for(FAUTO var : variables)
        {
            if(var.name.compare(name))
                { continue; }
            return var;
        }
    }
    return invalid_variable;
}

Error ThingData::_get_id_variable(ID& outValue,
    Farg<ThingVariable> inVariable,
    ThingVarType inType) const
{
    if(inVariable.type != inType)
        { return ERR_MISMATCHED_TYPES; }
    else if(ID out{Theatre::Current()->GetUID(inVariable.value)}; not out.invalid())
        { outValue = out; return OK; }
    return ERR_INVALID;
}

Shared<Resource> ThingData::_try_get_resource(Sarg inName) const
{ return Theatre::Current()->GetResource(Theatre::Current()->GetUID(inName)); }

Shared<Thinker> ThingData::_try_get_thinker(Sarg inName) const
{ return Theatre::Current()->GetThinker(Theatre::Current()->GetUID(inName)); }

bool ThingData::invalid() const
{ return name.empty() or not ThingFactory::IsThing(type); }

std::string ThingData::get_log() const noexcept
{
    std::string output{std::format("<ThingData>\n\ttype: {}\n\tname: {}\n\tvariables:",
        type.name(), name)};
    for(FAUTO var : variables)
        { output += std::format("\n\t\t{}", var.get_log()); }
    if(!children_variables.empty())
    {
        output += "\n\tchildren:";
        for(FAUTO var : children_variables)
            { output += std::format("\n\t\t{}", var.get_log()); }
    }
    if(!parent_variable.invalid())
        { output += std::format("\n\t\n\tparent: {}", parent_variable.get_log()); }
    return output + "\n";
}

std::string ThingData::get_parsable_string() const noexcept
{
    std::string output{std::format("\n{} {}\n{{", type.name(), name)};
    for(FAUTO var : variables)
    {
        if(var.invalid())
            { continue; }
        output += std::format("\n{}", var.get_parsable_string());
    }
    for(FAUTO var : children_variables)
    {
        if(var.invalid())
            { continue; }
        output += std::format("\n{}", var.get_parsable_string());
    }
    if(!parent_variable.invalid())
        { output += std::format("\n{}", parent_variable.get_parsable_string()); }
    return std::format("{}\n}}\n", output);
}

void ThingData::set_variable(Sarg inValue, Sarg inName)
{
    if(not inValue.empty())
    {
        if(FOUND_VAR(inName))
            { found_it->value = inName; }
        else
            { variables.emplace_back(inName, inValue, ThingVarType::String); }
    }
}

void ThingData::clear()
{
    type = {};
    name.clear();
    variables.clear();
    parent_variable.clear();
    children_variables.clear();
}

ID ThingData::get_parent() const
{
    ID out{};
    _get_id_variable(out, parent_variable, ThingVarType::Parent);
    return out;
}

IdSet_t ThingData::get_children() const
{
    IdSet_t children{};
    for(FAUTO child : children_variables)
    {
        if(ID uid{}; _get_id_variable(uid, child, ThingVarType::Child) == OK)
            { children.insert(uid); }
    }
    return children;
}

Error ThingData::remove_variable(Sarg inName)
{
    for(auto iter{variables.begin()}; iter != variables.end(); ++iter)
    {
        if(!iter->name.compare(inName))
        {
            variables.erase(iter);
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Error ThingData::remove_child(Sarg inName)
{
    for(auto iter{children_variables.begin()}; iter != children_variables.end(); ++iter)
    {
        if(iter->name.compare(inName))
        {
            children_variables.erase(iter);
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Error ThingData::set_variable(ID inValue, Sarg inName)
{
    ThingVariable temp{inName, "", ThingVarType::ID};
    if(!inName.compare("Parent"))
        { temp.type = ThingVarType::Parent; }
    else if(!inName.compare("Child"))
        { temp.type = ThingVarType::Child; }

    if(FAUTO name{Theatre::Current()->GetName(inValue)}; not name.empty())
        { temp.value = name; }
    else
        { return ERR_INVALID_ID; }

    if(temp.type == ThingVarType::Child)
    {
        Sarg _name{temp.value};
        if(FOUND_CHILD(_name))
            { *found_it = temp; }
        else
            { children_variables.push_back(temp); }
    }
    else if(temp.type == ThingVarType::Parent)
        { parent_variable = temp; }
    else if(FOUND_VAR(inName))
        { *found_it = temp; }
    else
        { variables.push_back(temp); }
    return OK;
}

void ThingData::set_variable(bool inValue, Sarg inName)
{
    std::string _value{std::format("{}", inValue)};
    if(FOUND_VAR(inName))
        { found_it->value = _value; }
    else
        { variables.emplace_back(inName, _value, ThingVarType::Bool); }
}

Error ThingData::set_variable(Shared<FileData> inValue, Sarg inName)
{
    if(not inValue->has_filepath())
        { return ERR_INVALID_PATH; }
    std::string _value{inValue->filepath()};
    if(FOUND_VAR(inName))
        { found_it->value = _value; }
    else
        { variables.emplace_back(inName, _value, ThingVarType::String);}
    return OK;
}


Error ThingData::set_enum_variable(Sarg inEnumName, Sarg inName)
{
    if(EnumRegistry::Contains(inEnumName))
    {
        if(FOUND_VAR(inName))
            { found_it->value = inEnumName; }
        else
            { variables.emplace_back(inName, inEnumName, ThingVarType::Enum); }
        return OK;
    }
    return ERR_INVALID;
}
