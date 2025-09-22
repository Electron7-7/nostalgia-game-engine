#ifndef DEBUG_H
#define DEBUG_H

#include "printing.hpp" // IWYU pragma: keep

#ifdef DEBUGGING
#   define PRINT_DEBUG(FORMAT, ARGS...) __LABELLED_PRINT(__DEBUG, FORMAT, ARGS)
#   define DEBUG(CODE) CODE
#   define NOTDEBUG(CODE)
#else  // !DEBUGGING
//  PRINT_DEBUG is disabled by default; add `-D DEBUGGING` to your compile flags to enable it
#   define PRINT_DEBUG(FORMAT, ARGS...) {} // The curly braces help avoid some syntax errors
#   define DEBUG(CODE)
#   define NOTDEBUG(CODE) CODE
#endif // DEBUGGING

#endif // DEBUG_H
