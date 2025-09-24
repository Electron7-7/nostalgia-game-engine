#ifndef TIME_H
#define TIME_H

#include <string>

struct Time
{
    static double Start();
    static double Current();
    static double Elapsed();

    static std::string StartSystem();
    static std::string CurrentSystem();
};

#endif // TIME_H
