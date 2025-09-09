#ifndef THINKER_H
#define THINKER_H

#include "thing.hpp"

class Thinker : public Thing
{
public:
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}
};

#endif // THINKER_H
