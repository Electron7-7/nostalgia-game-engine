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
    inline void Wait(bool* WhileTrue, double OrMsPasses = 0.0)
    {
        double start_time{Current()};
        if(OrMsPasses == 0.0)
            { OrMsPasses = DBL_MAX; }
        do {} while(WhileTrue || (Current() - start_time) >= OrMsPasses);
    }
}

#endif // TIME_H
