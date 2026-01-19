#include "parser.hpp"

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

int ThingData::get_children(ThinkerChildren& outChildren) const
{
    if(!theatre_registry) { return -1; }
    int child_count{0};
    for(FAUTO var : children_variables)
    {
        uint child_id{};
        if(!theatre_registry->try_GetID(var.value, child_id))
            { print_error("invalid child: {}", var.value); continue; }
        outChildren.emplace_back(child_id, var.name);
        ++child_count;
    }
    return child_count;
}

int ThingData::set_children(Farg<ThinkerChildren> inChildren)
{
    if(!theatre_registry) { return -1; }
    int child_count{0};
    for(FAUTO child : inChildren)
    {
        std::string child_name{};
        if(child.id.invalid()
            or !theatre_registry->try_GetIDName(child.id[], child_name))
            { continue; }
        children_variables.emplace_back(child.type.name(), child_name, ThingVarType::Child);
        ++child_count;
    }
    return child_count;
}

Error ThingData::get_parent(ThinkerRelative& outParent) const
{
    if(!theatre_registry) { return ERR_NULLPTR; }
    uint parent_id{};
    if(!theatre_registry->try_GetID(parent_variable.value, parent_id))
        { return ERR_INVALID_ID; }
    outParent = {parent_id, parent_variable.name};
    return OK;
}

Error ThingData::set_parent(Farg<ThinkerRelative> inParent)
{
    if(!theatre_registry) { return ERR_NULLPTR; }
    std::string parent_name{};
    if(inParent.id.invalid()
        or !theatre_registry->try_GetIDName(inParent.id[], parent_name))
        { return ERR_INVALID_ID; }
    parent_variable = {inParent.type.name(), parent_name, ThingVarType::Parent};
    return OK;
}
