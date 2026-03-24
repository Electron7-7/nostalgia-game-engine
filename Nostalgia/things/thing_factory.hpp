#ifndef THING_FACTORY_H
#define THING_FACTORY_H

typedef Shared<Thing> (*pThingMakerTemplate_t)(FPID inType, Sarg inName, ID inUID);

// This is a class instead of a namespace because fuck typing anything
// other than `friend class ThingFactory` in `ThingType`.
class ThingFactory
{
public:
    template<typename T> requires std::derived_from<T, Thing>
        static Shared<Thing> ThingMakerTemplate(FPID inType = ThingType::Thing, Sarg inName = "", ID inUID = {})
        { return Shared<Thing>(new T{inType, inName, inUID}); }

    static constexpr int cDefaultPriority{1};

    static bool Init();
    static bool IsInitialized();

    static Error AddThing(pThingMakerTemplate_t inPtr,
        Sarg inTypeName,
        FPID inBaseType,
        int inPriority = cDefaultPriority);

    static Error RemoveThing(FPID inType) noexcept;
    static Farg<ThingType> GetType(FPID inType) noexcept;

    static Shared<Thing>    MakeThing(FPID inTypeID, Sarg inName, ID inUID = {});
    static Shared<Thinker>  MakeThinker(FPID inTypeID, Sarg inName, ID inUID = {});
    static Shared<Resource> MakeResource(FPID inTypeID, Sarg inName, ID inUID = {});

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
