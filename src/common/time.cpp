#include "time.hpp"
#include <chrono>

std::chrono::time_point constant_ProgramStartTime = std::chrono::steady_clock::now();

constexpr double StartTime()
{ return std::chrono::duration<double>(constant_ProgramStartTime.time_since_epoch()).count(); }

double CurrentTime()
{ return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

double ElapsedTime()
{ return (CurrentTime() - StartTime()); }
