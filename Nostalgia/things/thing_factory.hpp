#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/resource.hpp>

typedef Shared<Thing> (*pThingMakerTemplate_t)(Sarg inName);

// This is a class instead of a namespace because fuck typing anything
// other than `friend class ThingFactory` in `ThingType`.
class ThingFactory
{
public:
    template<typename T> requires std::derived_from<T, Thing>
        static Shared<Thing> ThingMakerTemplate(Sarg inName = "")
        { return Shared<Thing>(new T{inName}); }

    static constexpr int cDefaultPriority{0};

    static bool Init();
    static bool IsInitialized();

    static Error AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists = false);
    static Error AddThing(pThingMakerTemplate_t inPtr,
        Sarg inTypeName,
        FPID inBaseType,
        int inPriority = cDefaultPriority);

    static Farg<std::map<PID, PID>> GetThingDeclarations();
    static std::set<ThingType_t> GetAllTypes();
    static IdVec_t GetAllTypeIDs();

    static Error RemoveThing(FPID inType) noexcept;
    static Farg<ThingType_t> GetType(FPID inType) noexcept;
    static PID GetClosestType(FPID inType) noexcept;

    static Shared<Thing> MakeThing(FPID inTypeID, Sarg inName = GlobalConstants::str_empty);

    static bool SetPriority(FPID inTypeID, int inPriority);
    static int  GetPriority(FPID inTypeID);

    static bool IsThing(FPID inTypeID);
    static bool IsThinker(FPID inTypeID);
    static bool IsResource(FPID inTypeID);
    static bool IsDerivedFrom(FPID inDerivedID, FPID inBaseID);

private:
    static bool m_sIsInitialized;
    static std::map<ID, pThingMakerTemplate_t> m_sThingMakers;
    static std::map<ID, int>                   m_sTypePriorities;
    static std::set<ThingType_t>               m_sAllTypes;
};

#endif // THING_FACTORY_H
