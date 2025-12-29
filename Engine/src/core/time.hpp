#ifndef TIME_H
#define TIME_H

#include <string>

namespace Time
{
    double Start();
    double Current();
    double Elapsed();

    std::string StartFormatted();
    std::string CurrentFormatted();

    // If `OrMsPasses` is '0.0', no maximum wait time will be set. (Well, it'll be set to `DBL_MAX`, so good luck reaching that).
    void Wait(bool& WhileTrue, double OrMsPasses = 0.0);
    void Wait(double TimeInMs = 0.0);
}

#endif // TIME_H
