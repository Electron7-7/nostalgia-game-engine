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
{
    Thing::SetVariables(inData);

    inData.get_variable(mVisible, "Visible");
}

Shared<ThingData> Thinker::GetVariables() const
{
    auto data{Thing::GetVariables()};
    data->set_variable(mVisible, "Visible");
    return data;
}

bool Thinker::Visible() const
{ return mVisible; }

 // If he's invincible, then how come I can see him?
void Thinker::SetVisible(bool inVisible)
{ mVisible = inVisible; }

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
