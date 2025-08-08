#include "time.hpp"

#include <chrono>

using namespace std::chrono;

const time_point constant_ProgramStartTime = steady_clock::now();

constexpr double StartTime()
{ return duration<double>(constant_ProgramStartTime.time_since_epoch()).count(); }

double CurrentTime()
{ return duration<double>(steady_clock::now().time_since_epoch()).count(); }

double ElapsedTime()
{ return (CurrentTime() - StartTime()); }
