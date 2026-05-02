#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Nostalgia/things/thing.hpp>

typedef Shared<Thing> (*pThingMakerTemplate_t)(Sarg inName);

namespace ThingFactory
{
    template<typename T> requires std::derived_from<T, Thing>
        inline Shared<Thing> ThingMakerTemplate(Sarg inName = "")
        { return Shared<Thing>(new T{inName}); }

    constexpr int cDefaultPriority{0};

    bool Init();
    bool IsInitialized();

    Shared<Thing> MakeThing(FPID inTypeID, Sarg inName = GlobalConstants::str_empty);

    Error AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists = false);
    Error AddThing(pThingMakerTemplate_t, Sarg inType, FPID inBaseType, int inPriority = cDefaultPriority);
    bool SetPriority(FPID inTypeID, int inPriority);
    int  GetPriority(FPID inTypeID);

    std::set<PID> DerivedFrom(FPID inType) noexcept;
    PID BaseOf(FPID inType) noexcept;

    bool IsThing(FPID inTypeID);
    bool IsThinker(FPID inTypeID);
    bool IsResource(FPID inTypeID);
    bool IsDerivedFrom(FPID inDerivedID, FPID inBaseID);
}

#endif // THING_FACTORY_H
