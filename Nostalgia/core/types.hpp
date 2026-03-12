#ifndef NOSTALGIA_TYPES_H
#define NOSTALGIA_TYPES_H

#include <sys/types.h>

using uchar = unsigned char;
using uint64 = uint64_t;

#ifdef WIN32
    using uint   = unsigned int;
    using ushort = unsigned short;
    using u_char = uchar;
    using uint32 = uint32_t;
#endif // WIN32

#endif // NOSTALGIA_TYPES_H
