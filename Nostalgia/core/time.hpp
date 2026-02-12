#ifndef TIME_H
#define TIME_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

namespace Runtime
{
    double Start();
    double Current();
    double Elapsed();

    std::string StartFormatted();
    std::string CurrentFormatted();
}

#endif // TIME_H
