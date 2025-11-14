#ifndef GAME_LOOP_H
#define GAME_LOOP_H

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

#endif // GAME_LOOP_H
