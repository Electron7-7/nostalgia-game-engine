#ifndef INPUT_FWD_H
#define INPUT_FWD_H

struct InputBindingID;
struct KeyID;

class InputEvent;
class InputAction;
class InputEventQueue;
class TriggerEvent;
class AppEvent;
class EngineEvent;
class AppEventQueue;
class EngineEventQueue;

typedef const KeyID& KeyArg;
typedef void (*InputEventCallback)(InputEvent*);

#endif // INPUT_FWD_H
