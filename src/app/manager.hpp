#ifndef MANAGER_H
#define MANAGER_H
#include <vector>

// Level State
enum LevelState_t
{
    NOT_IN_LEVEL = 0,
    LOADING_LEVEL,
    IN_LEVEL,
    SHUTTING_DOWN_LEVEL,
};

// Level Return Value (i.e: are we working, finished, or fucked?)
enum LevelReturnValue_t
{
    FUCKED = 0, // Valve chose to name this "FAILED", but I'm more crude (and therefore, clearly funnier)
    MORE_WORK,
    FINISHED,
};

// Tick rate and interval. Done this way for convenience and so I can change the tick rate more easily at the expense of negligable compile time(?)
#define TICK_RATE 70 // If DOOM did it at 70hz then so can you
#define TICK_INTERVAL ( 1.0f / TICK_RATE )

// Basic idea taken from Valve's Source Engine, specifically the file -> (src/app/legion/gamemanager.h)
class _Manager
{
public:
    // Managers are expected to implement these methods
    virtual bool Init() = 0;
    virtual LevelReturnValue_t LevelInit(bool IsFirstCall) = 0;
    virtual void Update() = 0;
    virtual LevelReturnValue_t LevelShutdown(bool IsFirstCall) = 0;
    virtual void Shutdown() = 0;

    // Called during game save/restore
    virtual void OnSave() = 0;
    virtual void OnRestore() = 0;

    // Basically only the physics manager will use this; states whether or not this manager should be updated in the fixed update loop
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
    static void Stop();

    // Return the current frame number
    static int FrameNumber();

    // In fixed update: return current/delta time
    static float FixedUpdateCurrentTime();
    static float FixedUpdateDeltaTime();

    // In rendering: return current/delta time
    static float CurrentTime();
    static float DeltaTime();

    // Helpers for loading a level
    static void StartNewLevel();
    static void ShutdownLevel();
    static LevelState_t GetLevelState();

protected:
    static void UpdateLevelStateMachine();

    virtual ~_Manager() {}

    typedef LevelReturnValue_t (_Manager::*ManagerLevelFunction_t)(bool IsFirstCall);
    typedef bool (_Manager::*ManagerInitFunc_t)();
    typedef void (_Manager::*ManagerFunc_t)();

    // Go through every added manager and invoke the supplied method, in specific order (forwards or backwards)
    static void InvokeMethod(ManagerFunc_t Function);
    static void InvokeMethodReverseOrder(ManagerFunc_t Function);
    static bool InvokeMethod(ManagerInitFunc_t Function);
    static LevelReturnValue_t InvokeLevelMethod(ManagerLevelFunction_t Function, bool IsFirstCall);
    static LevelReturnValue_t InvokeLevelMethodReverseOrder(ManagerLevelFunction_t Function, bool IsFirstCall);

    static bool level_shutdown_requested;
    static bool level_start_requested;
    static bool stop_requested;
    static std::vector<_Manager*> game_managers; // Not using smart pointers, because managers are only added during the engine's initialization and are just singleton accessors, meaning they should never result in memory leaks (i.e: made with "new")
    static bool is_running;
    static bool is_initialized;
    static int frame_number;
    static float current_time;
    static float last_time;
    static LevelState_t level_state;
};

template<typename BaseClass = _Manager>
class Manager : public BaseClass
{
public:
    virtual ~Manager();

    // Below are two comments from the Source source code; I'm including them here because what they say seems to be good practice:
    // Managers are expected to implement these methods.
    // NOTE: If Init or LevelInit fail, they are expected to call
    virtual bool Init() { return true; }
    virtual LevelReturnValue_t LevelInit( bool IsFirstCall ) { return FINISHED; }
    virtual void Update() {}
    virtual LevelReturnValue_t LevelShutdown( bool IsFirstCall ) { return FINISHED; }
    virtual void Shutdown() {}
    virtual void OnSave() {}
    virtual void OnRestore() {}
    virtual bool PleaseTickMeInAFixedUpdateLoop() { return false; }
};

// Another comment from the Source source code; including for same reasons:
// Automatically remove the game system if it gets deleted
template<typename BaseClass>
inline Manager<BaseClass>::~Manager()
{ _Manager::Remove(this); }

#endif // MANAGER_H