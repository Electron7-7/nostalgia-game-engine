#include "./thing_data.hpp"

std::string TheatreFile::ThingData::get_log() const noexcept
{
    std::string output{std::format("<ThingData>\n\ttype: {}\n\tname: {}\n\tvariables:",
        type.name(), name)};
    for(FAUTO var : variables)
        { output += std::format("\n\t\t{}: {}", var.name, var.value.operator std::string()); }
    if(not children_variables.empty())
    {
        output += "\n\tchildren:";
        for(FAUTO var : children_variables)
            { output += std::format("\n\t\t{}: {}", var.name, var.value.operator std::string()); }
    }
    if(not parent_variable.invalid())
    {
        output += std::format("\n\t\t{}: {}",
            parent_variable.name,
            parent_variable.value.operator std::string());
    }
    return output + "\n";
}

std::string TheatreFile::ThingData::get_parsable_string() const noexcept
{
    std::string output{std::format("\n{} {}\n{{", type.name(), name)};
    for(FAUTO var : variables)
    {
        if(var.invalid())
            { continue; }
        output += std::format("\n{} = {}", var.name, var.value.get_theatre_file_string());
    }
    for(FAUTO var : children_variables)
    {
        if(var.invalid())
            { continue; }
        output += std::format("\nChild = {}", var.value.get_theatre_file_string());
    }
    if(not parent_variable.invalid())
        { output += std::format("\nParent = {}", parent_variable.value.get_theatre_file_string()); }
    return std::format("{}\n}}\n", output);
}

bool TheatreFile::ThingData::invalid() const
{ return type == ThingType::Invalid; }

ID TheatreFile::ThingData::get_parent() const
{ return parent_variable.value.operator ID(); }

IdSet_t TheatreFile::ThingData::get_children() const
{
    IdSet_t _output{};
    for(FAUTO _child : children_variables)
        { _output.insert(_child.value.operator ID()); }
    return _output;
}

void TheatreFile::ThingData::remove_parent()
{ parent_variable = {}; }

void TheatreFile::ThingData::set_parent(Sarg inName)
{ parent_variable = {inName, "Parent", VARIABLE_HINT_THING_REFERENCE}; }

Error TheatreFile::ThingData::remove_child(Sarg inName)
{
    if(auto found_it{_find_child(inName)}; found_it != children_variables.end())
    {
        children_variables.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

void TheatreFile::ThingData::add_child(Sarg inName)
{
    if(auto found_it{_find_child(inName)}; found_it != children_variables.end())
        { return; }
    children_variables.emplace_back(inName, "Child", VARIABLE_HINT_THING_REFERENCE);
}

Error TheatreFile::ThingData::remove_variable(Sarg inName)
{
    if(auto found_it{_find_variable(inName)}; found_it != variables.end())
    {
        variables.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

TheatreFile::ThingVarArray::const_iterator TheatreFile::ThingData::_find_variable(Sarg inName) const
{
    for(auto iter{variables.cbegin()}; iter != variables.cend(); ++iter)
    {
        if(unlikely(iter->name == inName))
            { return iter; }
    }
    return variables.cend();
}

TheatreFile::ThingVarArray::iterator TheatreFile::ThingData::_find_variable(Sarg inName)
{
    for(auto iter{variables.begin()}; iter != variables.end(); ++iter)
    {
        if(unlikely(iter->name == inName))
            { return iter; }
    }
    return variables.end();
}

TheatreFile::ThingVarArray::const_iterator TheatreFile::ThingData::_find_child(Sarg inName) const
{
    for(auto iter{children_variables.cbegin()}; iter != children_variables.cend(); ++iter)
    {
        if(unlikely(static_cast<std::string>(iter->value) == inName))
            { return iter; }
    }
    return children_variables.cend();
}

TheatreFile::ThingVarArray::iterator TheatreFile::ThingData::_find_child(Sarg inName)
{
    for(auto iter{children_variables.begin()}; iter != children_variables.end(); ++iter)
    {
        if(unlikely(static_cast<std::string>(iter->value) == inName))
            { return iter; }
    }
    return children_variables.end();
}
