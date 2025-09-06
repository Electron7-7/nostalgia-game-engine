#include "time.hpp"

#include <format>
#include <chrono>

static const std::chrono::time_point s_cProgramStartSteady = std::chrono::steady_clock::now();
static const std::chrono::time_point s_cProgramStartSystem = std::chrono::system_clock::now();
static double wait_start_time = 0.0;
static constexpr const char* s_cTimeFormat = "{:%X}";

#define STEADY_NOW std::chrono::steady_clock::now()
#define SYSTEM_NOW std::chrono::system_clock::now()

double Time::Start()
{ return std::chrono::duration<double>(s_cProgramStartSteady.time_since_epoch()).count(); }

double Time::Current()
{ return std::chrono::duration<double>(STEADY_NOW.time_since_epoch()).count(); }

double Time::Elapsed()
{ return (Current() - Start()); }

std::string Time::StartSystem()
{ return std::format(s_cTimeFormat, std::chrono::current_zone()->to_local(s_cProgramStartSystem)); }

std::string Time::CurrentSystem()
{ return std::format(s_cTimeFormat, std::chrono::current_zone()->to_local(SYSTEM_NOW)); }

void SetWaitStart()
{ wait_start_time = Time::Current(); }

double GetWaitStart()
{ return wait_start_time; }
