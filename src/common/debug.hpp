#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUGGING
#   include "testing_app/ui/imgui_debugger.hpp" // IWYU pragma: keep
#   define DEBUG(CODE) CODE
#else  // !DEBUGGING
#   define DEBUG(CODE)
#endif // DEBUGGING

#endif // DEBUG_H
