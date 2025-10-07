#ifndef THING_FACTORY_H
#define THING_FACTORY_H

#include "fwd.hpp"
#include "ids.hpp"

#include <map>
#include <memory>

typedef
std::shared_ptr<Thing>(*pThingMaker_t)();

template<typename T>
std::shared_ptr<Thing> gThingMakerTemplate();

class ThingFactory
{
public:
    static bool AddThing(pThingMaker_t MakerFunction_ptr, const std::string& TypeName, int Priority = 0);
    static pThingMaker_t MakeThing(ID TypeId);
    static bool SetPriority(ID TypeId, int Priority);
    static int  GetPriority(ID TypeId);
    static bool IsThing(ID TypeId);
    static bool IsThing(const std::string& TypeName);
    static const std::string& GetTypeName(ID TypeId);

private:
    static std::map<ID, std::string>   m_sTypeNames;
    static std::map<ID, pThingMaker_t> m_sThingMakers;
    static std::map<ID, int>           m_sTypePriorities;
};

#endif // THING_FACTORY_H
