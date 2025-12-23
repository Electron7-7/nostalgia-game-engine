#ifndef TIME_H
#define TIME_H

#include <string>
#include <float.h>

namespace Time
{
    double Start();
    double Current();
    double Elapsed();

    std::string StartFormatted();
    std::string CurrentFormatted();

    // If `OrMsPasses` is '0.0', no maximum wait time will be set. (Well, it'll be set to `DBL_MAX`, so good luck reaching that).
    inline void Wait(bool& WhileTrue, double OrMsPasses = 0.0)
    {
        double start_time{Current()};
        OrMsPasses = (!OrMsPasses)
            ? DBL_MAX
            : OrMsPasses * 0.001;
        while(WhileTrue && (Current() - start_time) < OrMsPasses) {}
    }
    inline void Wait(double TimeInMs = 0.0)
    {
        bool throwaway{true};
        Wait(throwaway, TimeInMs);
    }
}

#endif // TIME_H
