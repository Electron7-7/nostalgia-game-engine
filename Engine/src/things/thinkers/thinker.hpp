#ifdef FWD_DCL
    class Thinker;
#elif !defined THINKER_H
#define THINKER_H

#include "../thing.hpp"

class Thinker : public Thing
{
public:
    // virtual void SetVariables(Farg<ThingData>) override;
    // virtual Shared<ThingData> GetVariables() const override;
};

#endif // THINKER_H
