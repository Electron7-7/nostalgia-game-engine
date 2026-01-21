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
    else if(uint out; theatre_registry->try_GetID(inVariable.value, out))
        { outValue = out; return OK; }
    return ERR_INVALID;
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
