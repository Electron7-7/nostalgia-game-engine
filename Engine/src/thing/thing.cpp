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
        { g_pTheatreManager->GetThing(child.id)->remove_parent({mUID, mType}); }
    for(auto parent : mParents)
        { g_pTheatreManager->GetThing(parent.id)->remove_child({mUID, mType}); }
    g_pTheatreManager->DestroyThing(mUID);
}

void Thing::Ready()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    for(auto child : mChildren)
        { g_pTheatreManager->GetThing(child.id)->add_parent({mUID, mType}); }
    for(auto parent : mParents)
        { g_pTheatreManager->GetThing(parent.id)->add_child({mUID, mType}); }
}

void Thing::SetVariables(Farg<ThingData> data)
{
    mUID  = data.uid;
    mName = data.name;
    mType = data.type();
    mStartingData = MakeUnique<ThingData>(data);
    data.GetChildren(mChildren);
}

Shared<ThingData> Thing::GetVariables() const
{
    auto data{MakeShared<ThingData>(mName, mType, mUID)};
    data->SetChildren(mChildren);
    return data;
}

ThingData Thing::GetStartingVariables() const
{ return *mStartingData; }

ID Thing::uid() const
{ return mUID; }

Farg<PID> Thing::type() const
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

relatives_t Thing::children() const
{ return mChildren; }

relatives_t Thing::parents() const
{ return mParents; }

Error Thing::add_child(ThingRelative inChild, bool doUpdateChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        found_it != mChildren.cend())
        { return ERR_ALREADY_EXISTS; }
    mChildren.emplace_back(inChild);
    if(doUpdateChild)
        { g_pTheatreManager->GetThing(inChild.id)->add_parent({mUID, mType}); }
    return OK;
}

Error Thing::remove_child(ThingRelative inChild, bool doUpdateChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        found_it != mChildren.cend())
    {
        if(doUpdateChild)
            { g_pTheatreManager->GetThing(found_it->id)->remove_parent({mUID, mType}); }
        mChildren.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Thing::swap_child(ThingRelative inChild, ThingRelative inNewChild, bool doUpdateChildren)
{
    if(Error status{add_child(inNewChild, doUpdateChildren)}; !status)
        { return status; }
    else if(Error status{remove_child(inChild, doUpdateChildren)}; !status)
        { remove_child(inNewChild, doUpdateChildren); return status; }
    return OK;
}

Error Thing::add_parent(ThingRelative inParent, bool doUpdateParent)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    g_pTheatreManager->GetThing(inParent.id)->add_child({mUID, mType});
    if(auto found_it{std::find(mParents.cbegin(), mParents.cend(), inParent)};
        found_it != mParents.cend())
        { return ERR_ALREADY_EXISTS; }
    mParents.emplace_back(inParent);
    if(doUpdateParent)
        { g_pTheatreManager->GetThing(inParent.id)->add_child({mUID, mType}); }
    return OK;
}

Error Thing::remove_parent(ThingRelative inParent, bool doUpdateParent)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inParent.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mParents.cbegin(), mParents.cend(), inParent)};
        found_it != mParents.cend())
    {
        if(doUpdateParent)
            { g_pTheatreManager->GetThing(found_it->id)->remove_child({mUID, mType}); }
        mParents.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Thing::swap_parent(ThingRelative inParent, ThingRelative inNewParent, bool doUpdateParents)
{
    if(Error status{add_parent(inNewParent, doUpdateParents)}; !status)
        { return status; }
    else if(Error status{remove_parent(inParent, doUpdateParents)}; !status)
        { remove_parent(inNewParent, doUpdateParents); return status; }
    return OK;
}
