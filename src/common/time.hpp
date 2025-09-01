#ifndef TIME_H
#define TIME_H

struct Time
{
    static double Start();
    static double Current();
    static double Elapsed();
};

// FIXME: Put these in a namespace/struct
void SetWaitStart();
double GetWaitStart();
// When CONDITION is true, or we've waited for PATIENCE amount of time, exit the while loop
#define WAIT_FOR(CONDITION, PATIENCE) \
SetWaitStart(); \
while(!CONDITION || (Time::Current() - GetWaitStart()) < PATIENCE) {}

#endif // TIME_H
