#include "./nostalgia_player.hpp"
#include "./3d/actor_3d.hpp"
#include "./2d/actor_2d.hpp"
#include "../thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void NostalgiaPlayer::Ready()
{
    if(m_pActor3D and m_pActor2D)
        { return; }

    FAUTO _children{Theatre::Current()->GetChildren(uid())};
    for(ID uid : _children)
    {
        if(Theatre::Current()->DerivedFrom(uid, ThingType::Actor3D))
        {
            if(m_pActor3D and m_pActor3D->uid() != uid)
            {
                print_warning("NostalgiaPlayer '{}' has multiple Actor3D children, but only {} will be used",
                    name(),
                    m_pActor3D->name());
                continue;
            }
            m_pActor3D = Theatre::Current()->GetThinker<Actor3D>(uid);
        }
        else if(Theatre::Current()->DerivedFrom(uid, ThingType::Actor2D))
        {
            if(m_pActor2D and m_pActor2D->uid() != uid)
            {
                print_warning("NostalgiaPlayer '{}' has multiple Actor2D children, but only {} will be used",
                    name(),
                    m_pActor2D->name());
                continue;
            }
            m_pActor2D = Theatre::Current()->GetThinker<Actor2D>(uid);
        }
    }

    if(not m_pActor3D)
    {
        m_pActor3D = Theatre::Current()->GetThinker<Actor3D>(
            Theatre::Current()->CreateThing({ThingType::Actor3D, "Player_Actor3D", {}, {name(), "Parent"}}));
    }

    if(not m_pActor2D)
    {
        m_pActor2D = Theatre::Current()->GetThinker<Actor2D>(
            Theatre::Current()->CreateThing({ThingType::Actor2D, "Player_Actor2D", {}, {name(), "Parent"}}));
    }
}

void NostalgiaPlayer::SetVariables(Farg<ThingData> inData)
{
    Super::SetVariables(inData);

    inData.get_variable(m_pActor3D, "Actor3D", "3DPlayer", "3DComponent");
    inData.get_variable(m_pActor2D, "Actor2D", "2DPlayer", "2DComponent");
}

Shared<ThingData> NostalgiaPlayer::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(m_pActor3D, "Actor3D");
    _data->set_variable(m_pActor2D, "Actor2D");

    return _data;
}

Shared<Actor3D> NostalgiaPlayer::GetActor3D() const
{
    if(not m_pActor3D)
        { return MakeShared<Actor3D>(); }
    return m_pActor3D;
}

Shared<Actor2D> NostalgiaPlayer::GetActor2D() const
{
    if(not m_pActor2D)
        { return MakeShared<Actor2D>(); }
    return m_pActor2D;
}
