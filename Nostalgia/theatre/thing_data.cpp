#include "./theatre.hpp"

using namespace TheatreFile;

std::string ThingData::get_log() const noexcept
{
    std::string output{std::format("<ThingData>\n\ttype: {}\n\tname: {}\n\tuid: {}\n\tvariables:",
        type.name(), name, uid())};
    for(FAUTO var : variables)
        { output += std::format("\n\t\t{}", var.get_log()); }
    if(!children_variables.empty())
    {
        output += "\n\tchildren_variables:";
        for(FAUTO var : children_variables)
            { output += std::format("\n\t\t{}", var.get_log()); }
    }
    if(!parent_variable.invalid())
        { output += std::format("\n\t\n\tparent_variable: {}", parent_variable.get_log()); }
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
{ variables.emplace_back(inName, inValue, ThingVarType::String); }

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

void ThingData::clear()
{
    type = {};
    uid  = {};
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
        { children_variables.push_back(temp); }
    else if(temp.type == ThingVarType::Parent)
        { parent_variable = temp; }
    else
        { variables.push_back(temp); }
    return OK;
}

void ThingData::set_variable(bool inValue, Sarg inName)
{ variables.emplace_back(inName, std::format("{}", inValue), ThingVarType::Bool); }

Error ThingData::set_variable(Shared<FileData> inValue, Sarg inName)
{
    if(not inValue->HasPath())
    {
        print_warning("I have yet to fully implement a good way of being able to save embedded file data that isn't comparing the actual binary data of one FileData variable to every single stored FileData object in VariableRegistry, so it'll *attempt* to do that, but don't expect any success... for now.");
        if(FAUTO data_name{VariableRegistry::GetResourceDataName(inValue)}; not data_name.empty())
        {
            variables.emplace_back(inName, data_name, ThingVarType::ID);
            return OK;
        }
        return ERR_NOT_FOUND;
    }
    variables.emplace_back(inName, inValue->Path(), ThingVarType::String);
    return OK;
}
