#ifndef MANAGER_H
#define MANAGER_H
#include <vector>
#include <chrono>

// Theatre State
enum TheatreState_t
{
    NOT_IN_LEVEL,
    LOADING_LEVEL,
    IN_LEVEL,
    SHUTTING_DOWN_LEVEL,
};

// Theatre Return Value (i.e: are we working, finished, or fucked?)
enum TheatreReturnValue_t
{
    FUCKED,
    MORE_WORK,
    FINISHED,
};

// Basic idea taken from Valve's Source Engine, specifically the file -> (src/app/legion/gamemanager.h)
class _Manager
{
public:
    virtual bool Init() = 0;
    virtual TheatreReturnValue_t TheatreInit(bool IsFirstCall) = 0;
    virtual void Update() = 0;
    virtual TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) = 0;
    virtual void Shutdown() = 0;

    // Called during game save/restore
    virtual void OnSave() = 0;
    virtual void OnRestore() = 0;

    // States whether or not this manager should be updated in the fixed update loop
    virtual bool PleaseTickMeInAFixedUpdateLoop() = 0;

    // Add/Remove managers
    static void Add(_Manager* ManagerToAdd);
    static void Remove(_Manager* ManagerToRemove);
    static void RemoveAll();

    // Init/Shutdown managers
    static bool InitAllManagers();
    static void ShutdownAllManagers();

    // Start/Stop running all game managers
    static void Start();
    static void Tick();
    static void Stop();

    // Return the current frame number
    static int FrameNumber();

    // In fixed update: return current/delta time
    static float FixedUpdateCurrentTime();
    static float FixedUpdateDeltaTime();

    // In rendering: return current/delta time
    static float CurrentTime();
    static float DeltaTime();

    // Helpers for loading a theatre
    static void StartNewTheatre();
    static void ShutdownTheatre();
    static TheatreState_t GetTheatreState();

protected:
    static void UpdateTheatreStateMachine();

    virtual ~_Manager() {}

    typedef TheatreReturnValue_t (_Manager::*ManagerTheatreFunction_t)(bool IsFirstCall);
    typedef bool (_Manager::*ManagerInitFunc_t)();
    typedef void (_Manager::*ManagerFunc_t)();

    // Go through every added manager and invoke the supplied method, in specific order (forwards or backwards)
    static void InvokeMethod(ManagerFunc_t Function);
    static void InvokeMethodReverseOrder(ManagerFunc_t Function);
    static bool InvokeMethod(ManagerInitFunc_t Function);
    static TheatreReturnValue_t InvokeTheatreMethod(ManagerTheatreFunction_t Function, bool IsFirstCall);
    static TheatreReturnValue_t InvokeTheatreMethodReverseOrder(ManagerTheatreFunction_t Function, bool IsFirstCall);

    static bool theatre_shutdown_requested;
    static bool theatre_start_requested;
    static bool stop_requested;
    static std::vector<_Manager*> game_managers; // Not using smart pointers, because managers are only added during the engine's initialization and are just singleton accessors, meaning they should never result in memory leaks (i.e: made with "new")
    static bool is_running;
    static bool is_initialized;
    static int frame_number;
    static int tick_number;
    static TheatreState_t theatre_state;
    inline static std::chrono::time_point start_time = std::chrono::steady_clock::now();
};

class Manager : public _Manager
{
public:
    virtual ~Manager();

    // Managers are expected to implement these methods.
    virtual bool Init() { return true; }
    virtual TheatreReturnValue_t TheatreInit( bool IsFirstCall ) { return FINISHED; }
    virtual void Update() {}
    virtual TheatreReturnValue_t TheatreShutdown( bool IsFirstCall ) { return FINISHED; }
    virtual void Shutdown() {}
    virtual void OnSave() {}
    virtual void OnRestore() {}
    virtual bool PleaseTickMeInAFixedUpdateLoop() { return false; }
};

// Automatically remove the game manager if it gets deleted
inline Manager::~Manager()
{ _Manager::Remove(this); }

#endif // MANAGER_H
