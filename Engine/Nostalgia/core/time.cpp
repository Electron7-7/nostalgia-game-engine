static const std::chrono::time_point cProgramStartSteady = std::chrono::steady_clock::now();
static const std::chrono::time_point cProgramStartSystem = std::chrono::system_clock::now();
static constexpr const char* cTimeFormat = "{:%X}";

#define STEADY_NOW std::chrono::steady_clock::now()
#define SYSTEM_NOW std::chrono::system_clock::now()

double Runtime::Start()
{ return std::chrono::duration<double>(cProgramStartSteady.time_since_epoch()).count(); }

double Runtime::Current()
{ return std::chrono::duration<double>(STEADY_NOW.time_since_epoch()).count(); }

double Runtime::Elapsed()
{ return (Current() - Start()); }

std::string Runtime::StartFormatted()
{ return std::format(cTimeFormat, std::chrono::current_zone()->to_local(cProgramStartSystem)); }

std::string Runtime::CurrentFormatted()
{ return std::format(cTimeFormat, std::chrono::current_zone()->to_local(SYSTEM_NOW)); }

#undef STEADY_NOW
#undef SYSTEM_NOW
