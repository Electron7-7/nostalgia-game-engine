#include "thing.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"

Thing::Thing()  noexcept = default;
Thing::~Thing() noexcept = default;

void Thing::Free()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    for(auto child : mChildren)
        { if(child.is_unique) { g_pTheatreManager->GetThing(child.id)->unset_parent(); } }
    g_pTheatreManager->GetThing(mParentID)->remove_child(mUID);
    g_pTheatreManager->DestroyThing(mUID);
}

void Thing::Ready()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    for(auto child : mChildren)
        { if(child.is_unique) { g_pTheatreManager->GetThing(child.id)->set_parent(mUID); } }
}

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
    data.GetChildren(mChildren);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>(mName, mType, mUID)};
    data->SetChildren(mChildren);
    return data;
}

ID Thing::uid() const
{ return mUID; }

Farg<TTID> Thing::type() const
{ return mType; }

Farg<std::string> Thing::name() const
{ return mName; }

const char* const Thing::c_name() const
{ return mName.data(); }

bool Thing::uid(ID inID)
{
    if(inID == mUID)
        { return true; }
    else if(inID.invalid())
        { inID = UID::Generate(); }
    else if(inID != UID::a_Player)
        { UID::Push(inID[]); }
    UID::Erase(mUID[]);
    mUID = inID;
    return true;
}

void Thing::name(Sarg inName)
{ mName = inName; }

Farg<children_t> Thing::children() const
{ return mChildren; }

ID Thing::parent() const
{ return mParentID; }

Error Thing::add_child(ThingChild inChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        inChild.is_unique and found_it != mChildren.cend())
        { return ERR_ALREADY_EXISTS; }
    mChildren.emplace_back(inChild);
    if(inChild.is_unique)
        { g_pTheatreManager->GetThing(inChild.id)->set_parent(mUID); }
    return OK;
}

Error Thing::remove_child(ThingChild inChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        found_it != mChildren.cend())
    {
        if(found_it->is_unique)
            { g_pTheatreManager->GetThing(found_it->id)->unset_parent(); }
        mChildren.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Thing::swap_child(ThingChild inChild, ThingChild inNewChild)
{
    if(Error status{add_child(inNewChild)}; !status)
        { return status; }
    else if(Error status{remove_child(inChild)}; !status)
        { remove_child(inNewChild); return status; }
    return OK;
}

Error Thing::set_parent(ID inID)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    g_pTheatreManager->GetThing(mParentID)->remove_child(mUID);
    mParentID = inID;
    g_pTheatreManager->GetThing(mParentID)->add_child(mUID);
    return OK;
}

Error Thing::unset_parent()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    auto parent{g_pTheatreManager->GetThing(mParentID)};
    parent->remove_child(mUID);
    return set_parent(parent->mParentID);
}
