#ifndef NOSTALGIA_TYPES_H
#define NOSTALGIA_TYPES_H

#include <sys/types.h>

using uchar = unsigned char;

#ifdef WIN32
    using uint   = unsigned int;
    using ushort = unsigned short;
    using u_char = uchar;
#endif // WIN32

#endif // NOSTALGIA_TYPES_H
