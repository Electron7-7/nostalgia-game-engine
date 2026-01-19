#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "fwd/theatre.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"
#include "theatre/thing_type.hpp"
#include <map>
#include <set>

typedef Shared<Thing> (*pThingMakerTemplate_t)();

// This is a class instead of a namespace because fuck typing anything
// other than `friend class ThingFactory` in `ThingType`.
class ThingFactory
{
public:
    template<typename T> requires std::derived_from<T, Thing>
        static Shared<Thing> ThingMakerTemplate()
        { return Shared<Thing>(new T{}); }

    static constexpr int cDefaultPriority{1};

    static bool Init();
    static bool IsInitialized();

    static Error AddThing(pThingMakerTemplate_t inPtr,
        Sarg inTypeName,
        FPID inBaseType,
        int inPriority = cDefaultPriority);

    static Error RemoveThing(FPID inType) noexcept;
    static Farg<ThingType> GetType(FPID inType) noexcept;

    static Shared<Thing> MakeThing(FPID inTypeID);
    static Shared<Thinker> MakeThinker(FPID inTypeID);
    static Shared<Resource> MakeResource(FPID inTypeID);

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
    static std::set<ThingType>                 m_sAllTypes;
};

#endif // THING_FACTORY_H
