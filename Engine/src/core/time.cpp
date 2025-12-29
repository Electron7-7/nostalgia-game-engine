#include "core/time.hpp"
#include <float.h>
#include <format>
#include <chrono>

static const std::chrono::time_point cProgramStartSteady = std::chrono::steady_clock::now();
static const std::chrono::time_point cProgramStartSystem = std::chrono::system_clock::now();
static constexpr const char* cTimeFormat = "{:%X}";

#define STEADY_NOW std::chrono::steady_clock::now()
#define SYSTEM_NOW std::chrono::system_clock::now()

double Time::Start()
{ return std::chrono::duration<double>(cProgramStartSteady.time_since_epoch()).count(); }

double Time::Current()
{ return std::chrono::duration<double>(STEADY_NOW.time_since_epoch()).count(); }

double Time::Elapsed()
{ return (Current() - Start()); }

std::string Time::StartFormatted()
{ return std::format(cTimeFormat, std::chrono::current_zone()->to_local(cProgramStartSystem)); }

std::string Time::CurrentFormatted()
{ return std::format(cTimeFormat, std::chrono::current_zone()->to_local(SYSTEM_NOW)); }

void Time::Wait(bool& WhileTrue, double OrMsPasses)
{
    double start_time{Current()};
    OrMsPasses = (!OrMsPasses)
        ? DBL_MAX
        : OrMsPasses * 0.001;
    while(WhileTrue && (Current() - start_time) < OrMsPasses) {}
}
void Time::Wait(double TimeInMs)
{
    bool throwaway{true};
    Wait(throwaway, TimeInMs);
}

#undef STEADY_NOW
#undef SYSTEM_NOW
