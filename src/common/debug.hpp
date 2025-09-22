#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUGGING
#   define DEBUG(CODE) CODE
#   define NOTDEBUG(CODE)
#else  // !DEBUGGING
#   define DEBUG(CODE)
#   define NOTDEBUG(CODE) CODE
#endif // DEBUGGING

#endif // DEBUG_H
