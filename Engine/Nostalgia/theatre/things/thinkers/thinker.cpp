#include "theatre/theatre.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

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
{
    if(Parent().invalid() or my_theatre()->GetThinker(Parent())->Visible())
        { return mVisible; }
    return false;
}

 // If he's invincible, then how come I can see him?
void Thinker::SetVisible(bool inVisible)
{ mVisible = inVisible; }

IdSet_t Thinker::Children() const
{
    if(!my_theatre()) { return {}; }
    return my_theatre()->GetChildren(mUID);
}

ID Thinker::Parent() const
{
    if(!my_theatre()) { return {}; }
    return my_theatre()->GetParent(mUID);
}

Error Thinker::SetParent(ID inParentID)
{
    if(!my_theatre()) { return ERR_NULLPTR; }
    return my_theatre()->SetParent(mUID, inParentID);
}

Error Thinker::DropParent()
{
    if(!my_theatre()) { return ERR_NULLPTR; }
    return my_theatre()->DropParent(mUID);
}
