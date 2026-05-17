#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include <Nostalgia/things/thing.hpp>

#define USE_NAME_OR_UID(FUNCTION) \
    static FUNCTION(ID inUID); \
    static FUNCTION(Sarg inName);

typedef Shared<Thing> (*pThingMakerTemplate_t)();

struct ThingFactory
{
    inline static constexpr int cDefaultPriority{0};

    struct ThingType_t
    {
        PID name{ThingType::Invalid};
        PID inherits{ThingType::Invalid};
        bool is_virtual{false};
        int init_priority{cDefaultPriority};
    };

    template<Thing_t T>
        inline static Shared<Thing> ThingMakerTemplate()
        { return Shared<Thing>(new T{}); }

    static bool Init();
    static bool IsInitialized();
    static void Shutdown();

    static void DestroyAll(); // The nuclear option (should only really be used when shutting down the entire app)
    static void DestroyAllResources(); // The semi-nuclear option (strictly for `ResourceManager::Shutdown`)

    static LockGuard<RMutex> LockSortedTypeIDs();
    static Farg<std::vector<PID>> GetSortedTypeIDs();
    static Farg<Tree<PID>> GetAllTypes();

    static IdVec_t GetUIDs();
    static IdVec_t GetResourceUIDs();
    static IdVec_t GetThinkerUIDs();
    static Sarg GetName(ID inUID);
    static ID GetUID(Sarg inName);
    static bool DerivedFrom(ID inUID, FPID inType);
    static bool DerivedFrom(Sarg inName, FPID inType);
    static Error SetName(ID inUID, Sarg inNewName);
    static Error SetName(Sarg inOldName, Sarg inNewName);

    USE_NAME_OR_UID(FPID TypeOf)
    USE_NAME_OR_UID(Error Destroy)
    USE_NAME_OR_UID(bool Contains)
    USE_NAME_OR_UID(Shared<Thing> GetThing)

    template<Thing_t T>
        inline static Shared<T> Invalid()
        {
            auto _output{MakeShared<T>()};
            _output->mUID = ID::Invalid;
            return _output;
        }

    template<Thing_t T, NameOrUID_t U>
        inline static Shared<T> GetThing(Farg<U> inNameOrUID)
        {
            if(auto _thing{DCast<T>(GetThing(inNameOrUID))})
                { return _thing; }
            return ThingFactory::Invalid<T>();
        }

    static Shared<Thing> MakeThing(FPID inTypeID, Sarg inName = GlobalConstants::str_empty);

    template<Thing_t T>
        inline static Shared<T> MakeThing(Sarg inName = GlobalConstants::str_empty)
        {
            if(auto _output{DCast<T>(MakeThing(T::sClassType(), inName))})
                { return _output; }
            return ThingFactory::Invalid<T>();
        }

    static Error AddThingDeclaration(Sarg inTypeName, Sarg inSuperName, bool doOverrideIfExists = false);
    static Error AddThingType(pThingMakerTemplate_t,
        FPID inType, FPID inInherits, bool inIsVirtual = false, int inPriority = cDefaultPriority);

    static bool SetPriority(FPID inTypeID, int inPriority);
    static int  GetPriority(FPID inTypeID);
    static PidSet_t DerivedFrom(FPID inType) noexcept;
    static PID BaseOf(FPID inType) noexcept;
    static bool IsVirtual(FPID inType) noexcept;

    static bool IsThing(FPID inTypeID);
    static bool IsThinker(FPID inTypeID);
    static bool IsResource(FPID inTypeID);
    static bool IsDerivedFrom(FPID inDerivedID, FPID inBaseID);

private:
    friend class Thing;

    static ID GetUID();
    static void FreeUID(ID);
    static Shared<Thing> MakeThing(Shared<Thing>, std::string);
    static std::string GetUniqueName(Sarg inName);
};

#undef USE_NAME_OR_UID
#endif // THING_FACTORY_H
