#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "things/fwd.hpp"
#include "theatre/parser/fwd.hpp"

#include "core/safe_return.hpp"
#include "manager.hpp"
#include "input/event.hpp"
#include "things/thing_factory.hpp"

#include <memory>
#include <map>
#include <glm/glm.hpp>

class TheatreManager : public Manager
{
public:
    consteval const char* DebugName() { return "TheatreManager"; }
    bool Init();
    void Update();
    void Tick();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    void ReadyThings();
    bool ThingExists(const ID& UID);
    ID GetType(const ID& ObjectID);

    void LoadTheatreData(const TheatreData&);
    bool LoadTheatreFromMemory(const std::string& Data);
    bool LoadTheatreFromFile(const std::string& Path);

    const TheatreData& GetInitialState();
    TheatreData GetCurrentState();
    std::vector<ID> GetThingIDs();

    void DelegateInputEvent(const InputEvent& InputEvent);
    const ID& CreateThing(const ThingData& ThingData);
    std::shared_ptr<NostalgiaPlayer> GetLocalPlayer();
    bool DestroyThing(const ID&);

    std::shared_ptr<Thing> GetThing(const ID& ObjectID)
    { return (mThings.contains(ObjectID)) ? mThings.at(ObjectID) : std::make_shared<Thing>(); }

    template<ThingDerived T>
    std::shared_ptr<T> GetThing(const ID& ObjectID)
    {
        if(std::shared_ptr<T> thing = dynamic_pointer_cast<T>(GetThing(ObjectID)))
            { return thing; }
        return std::make_shared<T>();
    }

    // More efficient when breaking on an invalid UID is preferred. See `PhysicsManager::CreateBody` for an example.
    template<ThingDerived T>
    SafeStatus GetThing(const ID& ObjectID, std::shared_ptr<T>& Output)
    {
        if(std::shared_ptr<T> thing = dynamic_pointer_cast<T>(GetThing(ObjectID)))
            { Output = thing; return Status::NO_ERR; }
        return (ThingExists(ObjectID)) ? Status::ERROR_INVALID_ID : Status::ERROR_INVALID_TYPE;
    }

private:
    std::map<ID, std::shared_ptr<Thing>> mThings{};
    bool mCreatingThings{false};
    bool mDestroyingThings{false};

    void CreateThings();
    void DestroyThings();
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H
