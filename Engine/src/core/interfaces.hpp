#ifndef INTERFACES_H
#define INTERFACES_H

#include "input/fwd.hpp"

#include <vector>

class Updated
{
public:
    virtual void Update() = 0;
};

class Ticked
{
public:
    virtual void Tick() = 0;
};

class HandlesInput
{
public:
    HandlesInput();
    virtual ~HandlesInput();

    virtual void Input(const InputEvent&) = 0;

    static void DelegateInputEvent(const InputEvent&);

private:
    static std::vector<HandlesInput*> m_sInstances;
};

class KnowsTheatre
{
public:
    virtual void OnTheatreEntered() = 0;
    virtual void OnTheatreExited()  = 0;
};

#endif // INTERFACES_H
