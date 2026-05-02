#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Nostalgia/things/thing.hpp>

typedef Shared<Thing> (*pThingMakerTemplate_t)(Sarg inName);

// This is a class instead of a namespace because fuck typing anything
// other than `friend class ThingFactory` in `ThingType`.
class ThingFactory
{
public:
    template<typename T> requires std::derived_from<T, Thing>
        inline Shared<Thing> ThingMakerTemplate(Sarg inName = "")
        { return Shared<Thing>(new T{inName}); }

    static constexpr int cDefaultPriority{0};

    static bool Init();
    static bool IsInitialized();

    static Shared<Thing> MakeThing(FPID inTypeID, Sarg inName = GlobalConstants::str_empty);

    static Error AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists = false);
    static Error AddThing(pThingMakerTemplate_t, Sarg inType, FPID inBaseType, int inPriority = cDefaultPriority);
    static bool SetPriority(FPID inTypeID, int inPriority);
    static int  GetPriority(FPID inTypeID);

    static std::set<PID> DerivedFrom(FPID inType) noexcept;
    static PID BaseOf(FPID inType) noexcept;

    static bool IsThing(FPID inTypeID);
    static bool IsThinker(FPID inTypeID);
    static bool IsResource(FPID inTypeID);
    static bool IsDerivedFrom(FPID inDerivedID, FPID inBaseID);
}

#endif // THING_FACTORY_H
