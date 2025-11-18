#ifndef MANAGER_H
#define MANAGER_H

#include <vector>

namespace ManagerEnums
{
    enum TheatreState_t
    {
        NOT_IN_LEVEL,
        LOADING_LEVEL,
        IN_LEVEL,
        SHUTTING_DOWN_LEVEL,
    };

    enum TheatreReturnValue_t
    {
        FUCKED,
        MORE_WORK,
        FINISHED,
    };
}

// Basic idea taken from Valve's Source Engine, specifically the file -> (src/app/legion/gamemanager.h)
class IManager
{
public:
    virtual consteval const char* DebugName() = 0;
    virtual bool Init() = 0;
    virtual ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) = 0;
    virtual void Update() = 0;
    virtual void Tick()   = 0;
    virtual ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) = 0;
    virtual void Shutdown() = 0;

    // Called during game save/restore
    virtual void OnSave() = 0;
    virtual void OnRestore() = 0;

    // Add/Remove managers
    static void Add(IManager* ManagerToAdd);
    static void Remove(IManager* ManagerToRemove);
    static void RemoveAll();

    // Init/Shutdown managers
    static bool InitAllManagers();
    static void ShutdownAllManagers();

    // Start/Stop running all game managers
    static void Start();
    static void Stop();

    // Return the current frame or tick number
    static long FrameNumber();
    static long TickNumber();

    // In fixed update: return current/delta time
    static float FixedUpdateCurrentTime();
    static float FixedUpdateDeltaTime();

    // In rendering: return current/delta time
    static double CurrentTime();
    static double DeltaTime();

    // Helpers for loading a theatre
    static void StartNewTheatre();
    static void ShutdownTheatre();
    static ManagerEnums::TheatreState_t GetTheatreState();

protected:
    static void UpdateTheatreStateMachine();

    virtual ~IManager() {}

    typedef ManagerEnums::TheatreReturnValue_t (IManager::*ManagerTheatreFunction_t)(bool IsFirstCall);
    typedef bool (IManager::*ManagerInitFunc_t)();
    typedef void (IManager::*ManagerFunc_t)();

    // Go through every added manager and invoke the supplied method, in specific order (forwards or backwards)
    static void InvokeMethod(ManagerFunc_t Function);
    static void InvokeMethodReverseOrder(ManagerFunc_t Function);
    static bool InvokeMethod(ManagerInitFunc_t Function);
    static ManagerEnums::TheatreReturnValue_t InvokeTheatreMethod(ManagerTheatreFunction_t Function, bool IsFirstCall);
    static ManagerEnums::TheatreReturnValue_t InvokeTheatreMethodReverseOrder(ManagerTheatreFunction_t Function, bool IsFirstCall);

    static std::vector<IManager*> m_sGameManagers;
    static bool m_sTheatreShutdownRequested;
    static bool m_sTheatreStartRequested;
    static bool m_sStopRequested;
    static bool m_sIsRunning;
    static bool m_sIsInitialized;
    static long m_sFrameNumber;
    static long m_sTickNumber;
    static ManagerEnums::TheatreState_t theatre_state;

private:
    static void TickLoop();
};

class Manager : public IManager
{
public:
    virtual ~Manager();

    // Managers are expected to implement these methods.
    virtual consteval const char* DebugName() { return "Manager"; };
    virtual bool Init() { return true; }
    virtual ManagerEnums::TheatreReturnValue_t TheatreInit( bool IsFirstCall ) { return ManagerEnums::FINISHED; }
    virtual void Update() {}
    virtual void Tick() {}
    virtual ManagerEnums::TheatreReturnValue_t TheatreShutdown( bool IsFirstCall ) { return ManagerEnums::FINISHED; }
    virtual void Shutdown() {}
    virtual void OnSave() {}
    virtual void OnRestore() {}
};

// Automatically remove the game manager if it gets deleted
inline Manager::~Manager()
{ IManager::Remove(this); }

#ifdef DEBUGGING
    extern bool g_PrintFrameNumbers;
    extern bool g_PrintTickNumbers;
#endif // DEBUGGING

#endif // MANAGER_H
