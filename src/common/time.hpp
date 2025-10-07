#ifndef TIME_H
#define TIME_H

#include <string>

struct Time
{
    static double Start();
    static double Current();
    static double Elapsed();

    static std::string StartFormatted();
    static std::string CurrentFormatted();
};

#endif // TIME_H
