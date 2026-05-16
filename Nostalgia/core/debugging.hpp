#ifndef DEBUGGING_H
#define DEBUGGING_H

// https://stackoverflow.com/a/58240517
#ifdef NOSTALGIA_DEBUGGING
#   ifdef linux
#      include <signal.h>
#      define BREAK_HERE raise(SIGTRAP);
#   elifdef _WIN32
#      include <intrin.h>
#      define BREAK_HERE __debugbreak
#   endif
#else
#   define BREAK_HERE
#endif // NOSTALGIA_DEBUGGING

#endif // DEBUGGING_H
