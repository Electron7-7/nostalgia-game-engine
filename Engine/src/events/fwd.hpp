#ifndef INPUT_FWD_H
#define INPUT_FWD_H

struct InputBindingID;

class  InputAction;
class  InputEvent;
class  InputEventQueue;
struct KeyID;

typedef const KeyID& KeyArg;
typedef void (*InputEventCallback)(InputEvent*);

#endif // INPUT_FWD_H
