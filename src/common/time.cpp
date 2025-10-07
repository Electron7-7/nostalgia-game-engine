#include "time.hpp"

#include <format>
#include <chrono>

static const std::chrono::time_point cProgramStartSteady = std::chrono::steady_clock::now();
static const std::chrono::time_point cProgramStartSystem = std::chrono::system_clock::now();
static constexpr const char* cTimeFormat = "{:%X}";
static double sWaitStart = 0.0;

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

#undef STEADY_NOW
#undef SYSTEM_NOW
