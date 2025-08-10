#include "time.hpp"

#include <chrono>

using namespace std::chrono;

const time_point constant_ProgramStartTime = steady_clock::now();
double wait_start_time = 0.0;

constexpr double StartTime()
{ return duration<double>(constant_ProgramStartTime.time_since_epoch()).count(); }

double CurrentTime()
{ return duration<double>(steady_clock::now().time_since_epoch()).count(); }

double ElapsedTime()
{ return (CurrentTime() - StartTime()); }

void SetWaitStart()
{ wait_start_time = CurrentTime(); }

double GetWaitStart()
{ return wait_start_time; }
