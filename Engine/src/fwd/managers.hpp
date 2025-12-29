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

#endif // MANAGERS_FWD_H
