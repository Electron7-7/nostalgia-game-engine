#ifndef TIME_H
#define TIME_H

constexpr double StartTime();
double CurrentTime();
double ElapsedTime();

void SetWaitStart();
double GetWaitStart();

#define WAIT_FOR(CONDITION, PATIENCE) \
SetWaitStart(); while(CONDITION || (CurrentTime() - GetWaitStart()) < PATIENCE) {}

#endif // TIME_H
