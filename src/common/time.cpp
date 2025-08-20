#include "time.hpp"

#include <chrono>

using namespace std::chrono;

static const time_point constant_ProgramStartTime = steady_clock::now();
static double wait_start_time = 0.0;

double Time::Start()
{ return duration<double>(constant_ProgramStartTime.time_since_epoch()).count(); }

double Time::Current()
{ return duration<double>(steady_clock::now().time_since_epoch()).count(); }

double Time::Elapsed()
{ return (Current() - Start()); }

void SetWaitStart()
{ wait_start_time = Time::Current(); }

double GetWaitStart()
{ return wait_start_time; }
