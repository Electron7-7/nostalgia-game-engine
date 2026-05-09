#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Nostalgia/things/thing.hpp>

typedef Shared<Thing> (*pThingMakerTemplate_t)();

struct ThingFactory
{
    inline static constexpr int cDefaultPriority{0};

    template<Thing_t T>
        inline static Shared<Thing> ThingMakerTemplate()
        { return Shared<Thing>(new T{}); }

    static bool Init();
    static bool IsInitialized();
    static void Shutdown();

    template<Thing_t T>
        inline static Shared<T> MakeThing(Sarg inName = GlobalConstants::str_empty)
        {
            auto _output{MakeShared<T>()};
            _output->mName = inName;
            _output->mUID  = GetUID();
            return _output;
        }

    static Shared<Thing> MakeThing(FPID inTypeID, Sarg inName);

    static Error AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists = false);
    static Error AddThing(pThingMakerTemplate_t, Sarg inType, FPID inBaseType, int inPriority = cDefaultPriority);

    static bool SetPriority(FPID inTypeID, int inPriority);
    static int  GetPriority(FPID inTypeID);

    static IdSet_t DerivedFrom(FPID inType) noexcept;
    static PID BaseOf(FPID inType) noexcept;

    static bool IsThing(FPID inTypeID);
    static bool IsThinker(FPID inTypeID);
    static bool IsResource(FPID inTypeID);
    static bool IsDerivedFrom(FPID inDerivedID, FPID inBaseID);

private:
    friend class Thing;

    static ID GetUID();
    static void FreeUID(ID);
};

#endif // THING_FACTORY_H
