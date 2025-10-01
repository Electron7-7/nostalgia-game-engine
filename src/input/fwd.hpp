#ifndef INPUT_FWD_H
#define INPUT_FWD_H

#include <climits>

typedef unsigned int id_t;
constexpr id_t INVALID_ID = UINT_MAX;
constexpr const char* INVALID_NAME = "Invalid";

struct InputBindings;
struct binding_t;
struct InputEvent;
class EventQueue;

#endif // INPUT_FWD_H
