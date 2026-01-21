#include "thinker.hpp"
#include "theatre/theatre.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Thinker::Free()
{
    LockGuard<RMutex> lock{mChildrenMutex};
    m_pRootTheatre->DestroyThing(mUID);
}

void Thinker::SetVariables(Farg<ThingData> inData)
{ Thing::SetVariables(inData); }

Shared<ThingData> Thinker::GetVariables() const
{ return Thing::GetVariables(); }

IdSet_t Thinker::Children() const
{
    if(!m_pRootTheatre) { return {}; }
    return m_pRootTheatre->GetChildren(mUID);
}

ID Thinker::Parent() const
{
    if(!m_pRootTheatre) { return {}; }
    return m_pRootTheatre->GetParent(mUID);
}

Error Thinker::SetParent(ID inParentID)
{
    if(!m_pRootTheatre) { return ERR_NULLPTR; }
    return m_pRootTheatre->SetParent(mUID, inParentID);
}

Error Thinker::DropParent()
{
    if(!m_pRootTheatre) { return ERR_NULLPTR; }
    return m_pRootTheatre->DropParent(mUID);
}
