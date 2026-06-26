#ifndef TIME_H
#define TIME_H

namespace Runtime
{
    double Start();
    double Current();
    double Elapsed();

    std::string StartFormatted();
    std::string CurrentFormatted();
}

#endif // TIME_H
