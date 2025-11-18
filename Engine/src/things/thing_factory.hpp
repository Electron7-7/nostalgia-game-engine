#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "fwd.hpp"
#include "core/id.hpp"
#include "thing.hpp"

#include <map>
#include <memory>
#include <cassert>

typedef
    std::shared_ptr<Thing>(*pThingMaker_t)();

template<typename T>
    concept ThingDerived = requires{std::derived_from<T,Thing>;};

template<typename T>
    std::shared_ptr<Thing> gThingMakerTemplate();

constexpr int cDefaultPriority{1};

class ThingFactory
{
public:
    bool Init();

    bool AddThing(pThingMaker_t MakerFunction_ptr, const std::string& TypeName, int Priority = cDefaultPriority);
    pThingMaker_t MakeThing(ID TypeId);
    bool SetPriority(ID TypeId, int Priority);
    int  GetPriority(ID TypeId) const;
    const std::string& GetTypeName(ID TypeId) const;

    bool IsThing(ID TypeId) const;
    template<ThingDerived T>
    bool IsDerivedFrom(ID TypeId) const
        { assert(mIsInitialized); return (std::dynamic_pointer_cast<T>(mThingMakers.at(TypeId)()) != nullptr); } // Slower than `IsThing` as this function uses dynamic casting to test derivation
    bool IsResource(ID TypeId) const; // Uses `IsDerived`
    bool IsDevice(ID TypeId) const;   // Uses `IsDerived`

private:
    bool mIsInitialized{false};
    std::map<ID, std::string>   mTypeNames{};
    std::map<ID, pThingMaker_t> mThingMakers{};
    std::map<ID, int>           mTypePriorities{};
};

extern ThingFactory* g_pThingFactory;

#endif // THING_FACTORY_H
