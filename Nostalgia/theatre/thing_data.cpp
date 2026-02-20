#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

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
    else if(!inVariable.thing_uid.invalid())
        { outValue = inVariable.thing_uid; return OK; }
    else if(!theatre_registry)
        { return ERR_NULLPTR; }
    else if(ID out; theatre_registry->try_GetID(inVariable.value, out))
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
    theatre_registry.reset();
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
        ID uid{};
        _get_id_variable(uid, child, ThingVarType::Child);
        if(!uid.invalid())
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

Error ThingData::remove_child(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    for(auto iter{children_variables.begin()}; iter != children_variables.end(); ++iter)
    {
        if(iter->thing_uid == inID)
        {
            children_variables.erase(iter);
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
    ThingVariable temp{inName, "", ThingVarType::ID, inValue};
    if(!inName.compare("Parent"))
        { temp.type = ThingVarType::Parent; }
    else if(!inName.compare("Child"))
        { temp.type = ThingVarType::Child; }

    if(theatre_registry and !theatre_registry->try_GetIDName(inValue, temp.value))
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
