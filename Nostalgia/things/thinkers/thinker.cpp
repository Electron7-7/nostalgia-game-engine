#include "./thinker.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Thinker::InitVariables()
{
    Super::InitVariables();
}

void Thinker::SetVariables(Farg<ThingData> inData)
{
    Super::SetVariables(inData);

    inData.get_variable(mVisible, "Visible");
    inData.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Thinker::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mVisible, "Visible");
    data->set_variable(Theatre::Current()->GetParent(uid()), "Parent");
    auto children{Theatre::Current()->GetChildren(uid())};
    for(FAUTO child_id : children)
        { data->set_variable(child_id, "Child"); }

    data->set_variable(mDebugHighlight, "DebugHighlight");
    return data;
}

bool Thinker::Visible() const
{
    if(auto _parent{Parent()}; _parent.invalid() or Theatre::Current()->GetThinker(_parent)->Visible())
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

Farg<glm::vec4> Thinker::DebugHighlight() const
{
    return (this->IsHighlighted())
        ? mDebugHighlight
        : m_sNoHighlight;
}

void Thinker::DebugHighlight(Farg<glm::vec4> inColor)
{ mDebugHighlight = inColor; }

void Thinker::IsHighlighted(bool inHighlighted)
{ mIsHighlighted = inHighlighted; }

bool Thinker::IsHighlighted() const
{
    auto _parent{Parent()};
    return mIsHighlighted or (not _parent.invalid() and Theatre::Current()->GetThinker(_parent)->IsHighlighted());
}
