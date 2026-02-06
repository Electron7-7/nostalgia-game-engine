#ifndef NOSTALGIA_TYPES_H
#define NOSTALGIA_TYPES_H

#include <sys/types.h>

// Fixing issues with MinGW types
#if defined(__MINGW32__) or defined(__MINGW64__)
    using uint   = unsigned int;
    using ushort = unsigned short;
    using uchar  = unsigned char;
    using u_char = uchar;
#endif // MINGW

#endif // NOSTALGIA_TYPES_H
