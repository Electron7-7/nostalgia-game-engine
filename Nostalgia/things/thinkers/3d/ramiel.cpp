#include "./ramiel.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Ramiel::InitVariables()
{
    Super::InitVariables();
    mLocalTrans.scale = glm::vec3{120.0f};
}

void Ramiel::Ready()
{
    Super::Ready();

    ThingData _mesh_data{ThingType::MeshInstance3D, "Ramiel-MeshInstance3D-" + name()};
    _mesh_data.set_variable(UID::m_Ramiel, "Mesh");
    Theatre::Current()->SetParent(Theatre::Current()->CreateThing(_mesh_data), uid());
}

void Ramiel::SetVariables(Farg<ThingData> inData)
{
    Super::SetVariables(inData);

    inData.get_variable(mSpeed, "Speed");
    inData.get_variable(mDirection, "Direction");

    mDirection = glm::normalize(mDirection);
}

Shared<ThingData> Ramiel::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(mSpeed, "Speed");
    _data->set_variable(mDirection, "Direction");

    return _data;
}

void Ramiel::Tick()
{
    Super::Tick();
    SetPosition(Position() + (mDirection * mSpeed));
}
