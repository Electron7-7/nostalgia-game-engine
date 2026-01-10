#include "thinker.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"

void Thinker::Free()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    for(auto child : mChildren)
        { g_pTheatreManager->CurrentTheatre()->GetThinker(child.id)->remove_parent({mUID, mType}); }
    g_pTheatreManager->CurrentTheatre()->GetThinker(mParent.id)->remove_child({mUID, mType});
    g_pTheatreManager->CurrentTheatre()->DestroyThing(mUID);
}

void Thinker::Ready()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    for(auto child : mChildren)
        { g_pTheatreManager->CurrentTheatre()->GetThinker(child.id)->set_parent({mUID, mType}); }
    g_pTheatreManager->CurrentTheatre()->GetThinker(mParent.id)->add_child({mUID, mType});
}

void Thinker::SetVariables(Farg<ThingData> inData)
{
    Thing::SetVariables(inData);
    inData.GetChildren(mChildren);
    inData.GetParent(mParent);
}

Shared<ThingData> Thinker::GetVariables() const
{
    auto outData{Thing::GetVariables()};
    outData->SetChildren(mChildren);
    outData->SetParent(mParent);
    return outData;
}

ThinkerChildren Thinker::Children() const
{ return mChildren; }

ThinkerRelative Thinker::Parent() const
{ return mParent; }

Error Thinker::add_child(ThinkerRelative inChild, bool doUpdateChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        found_it != mChildren.cend())
        { return ERR_ALREADY_EXISTS; }
    mChildren.emplace_back(inChild);
    if(doUpdateChild)
        { g_pTheatreManager->CurrentTheatre()->GetThinker(inChild.id)->set_parent({mUID, mType}, false); }
    return OK;
}

Error Thinker::remove_child(ThinkerRelative inChild, bool doUpdateChild)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inChild.id.invalid())
        { return ERR_INVALID_ID; }
    else if(auto found_it{std::find(mChildren.cbegin(), mChildren.cend(), inChild)};
        found_it != mChildren.cend())
    {
        if(doUpdateChild)
            { g_pTheatreManager->CurrentTheatre()->GetThinker(found_it->id)->remove_parent({mUID, mType}, false); }
        mChildren.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error Thinker::swap_child(ThinkerRelative inChild, ThinkerRelative inNewChild, bool doUpdateChildren)
{
    if(Error status1{add_child(inNewChild, doUpdateChildren)}; !status1)
        { return status1; }
    else if(Error status2{remove_child(inChild, doUpdateChildren)}; !status2)
        { remove_child(inNewChild, doUpdateChildren); return status2; }
    return OK;
}

Error Thinker::set_parent(ThinkerRelative inParent, bool doUpdateParent)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(mParent != inParent)
    {
        g_pTheatreManager->CurrentTheatre()->GetThinker(mParent.id)->remove_child({mUID, mType}, false);
        g_pTheatreManager->CurrentTheatre()->GetThinker(inParent.id)->add_child({mUID, mType}, false);
        mParent = inParent;
    }
    return OK;
}

Error Thinker::remove_parent(ThinkerRelative inParent, bool doUpdateParent)
{
    LockGuard<RMutex> lock{mChildrenMutex};
    if(inParent.id.invalid() or inParent != mParent)
        { return ERR_INVALID_ID; }
    else if(doUpdateParent)
        { g_pTheatreManager->CurrentTheatre()->GetThinker(mParent.id)->remove_child({mUID, mType}, false); }
    mParent = {};
    return OK;
}

Error Thinker::swap_parent(ThinkerRelative inParent, ThinkerRelative inNewParent, bool doUpdateParents)
{
    if(Error status1{set_parent(inNewParent, doUpdateParents)}; !status1)
        { return status1; }
    if(Error status2{remove_parent(inParent, doUpdateParents)}; !status2)
        { remove_parent(inNewParent, doUpdateParents); return status2; }
    return OK;
}
