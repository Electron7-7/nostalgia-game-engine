#ifndef MANAGERS_FWD_H
#define MANAGERS_FWD_H

class IManager;
class Manager;
class PhysicsManager;
class RenderManager;
class TheatreManager;
class UIManager;
class EventManager;
class InputManager;

extern EventManager*   g_pEventManager;
extern InputManager*   g_pInputManager;
extern PhysicsManager* g_pPhysicsManager;
extern RenderManager*  g_pRenderManager;
extern TheatreManager* g_pTheatreManager;
extern UIManager*      g_pUIManager;

extern bool gEnableMsg_ContactValidate,
    gEnableMsg_ContactAdded,
    gEnableMsg_ContactPersisted,
    gEnableMsg_ContactRemoved,
    gEnableMsg_BodyActivated,
    gEnableMsg_BodyDeactivated,
    gDebugPrintFrameNumbers,
    gDebugPrintTickNumbers,
    gPrintInputLogs,
    gPrintLoadedTheatreData,
    gPrintEventLogs;

#endif // MANAGERS_FWD_H
