#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Thinker::SetVariables(Farg<ThingData> inData)
{
    Super::SetVariables(inData);

    inData.get_variable(mVisible, "Visible");
    Theatre::Current()->SetParent(uid(), inData.get_parent());
    for(FAUTO child : inData.get_children())
        { Theatre::Current()->SetParent(child, uid()); }
}

Shared<ThingData> Thinker::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mVisible, "Visible");
    data->set_variable(Theatre::Current()->GetParent(uid()), "Parent");
    auto children{Theatre::Current()->GetChildren(uid())};
    for(FAUTO child_id : children)
        { data->set_variable(child_id, "Child"); }

    return data;
}

bool Thinker::Visible() const
{
    if(Parent().invalid() or Theatre::Current()->GetThinker(Parent())->Visible())
        { return mVisible; }
    return false;
}

 // If he's invincible, then how come I can see him?
void Thinker::SetVisible(bool inVisible)
{ mVisible = inVisible; }

IdSet_t Thinker::Children() const
{
    if(!Theatre::Current()) { return {}; }
    return Theatre::Current()->GetChildren(uid());
}

ID Thinker::Parent() const
{
    if(!Theatre::Current()) { return {}; }
    return Theatre::Current()->GetParent(uid());
}

Error Thinker::SetParent(ID inParentID)
{
    if(!Theatre::Current()) { return ERR_NULLPTR; }
    return Theatre::Current()->SetParent(uid(), inParentID);
}

Error Thinker::DropParent()
{
    if(!Theatre::Current()) { return ERR_NULLPTR; }
    return Theatre::Current()->DropParent(uid());
}
