#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "thing/thing.hpp"
#include "core/id.hpp"
#include "core/smart_pointers.hpp"
#include <map>
#include <cassert>

typedef Shared<Thing> (*pThingMakerTemplate_t)();

class ThingFactory
{
public:
    template<ThingDerived T>
        static Shared<Thing> ThingMakerTemplate() { return Shared<Thing>(new T{}); }

    static constexpr int cDefaultPriority{1};

    bool Init();

    bool AddThing(pThingMakerTemplate_t FunctionPtr, Sarg TypeName, int Priority = cDefaultPriority);
    pThingMakerTemplate_t MakeThing(Farg<TTID> TypeId);
    bool SetPriority(Farg<TTID> TypeId, int Priority);
    int  GetPriority(Farg<TTID> TypeId) const;

    bool IsThing(Farg<TTID> TypeId) const;

    // Slower than `IsThing` due to using `dynamic_pointer_cast` to test derivation
    template<typename T> requires std::derived_from<T,Thing>
        bool IsDerivedFrom(Farg<TTID> TypeId) const
        {
            assert(mIsInitialized);
            if(auto found_it{mThingMakers.find(TypeId)}; found_it != mThingMakers.end())
                { return DCast<T>(found_it->second()) != nullptr; }
            return false;
        }
    bool IsResource(Farg<TTID> TypeId) const; // Uses `IsDerivedFrom`

private:
    bool mIsInitialized{false};
    std::map<ID, pThingMakerTemplate_t> mThingMakers{};
    std::map<ID, int>                   mTypePriorities{};
};

extern ThingFactory* g_pThingFactory;

#endif // THING_FACTORY_H
