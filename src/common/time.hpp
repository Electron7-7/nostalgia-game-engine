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
#define WAIT_FOR(CONDITION, PATIENCE) SetWaitStart(); while(CONDITION || (Time::Current() - GetWaitStart()) < PATIENCE) {}

#endif // TIME_H
