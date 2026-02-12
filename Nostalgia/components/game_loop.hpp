#ifndef GAME_LOOP_COMPONENTS_H
#define GAME_LOOP_COMPONENTS_H

class OnUpdate
{
public:
    virtual void Update() = 0;
};

class OnTick
{
public:
    virtual void Tick() = 0;
};

class OnTheatreChanged
{
public:
    virtual void TheatreEntered() = 0;
    virtual void TheatreExited()  = 0;
};

#endif // GAME_LOOP_COMPONENTS_H
