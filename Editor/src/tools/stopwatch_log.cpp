#include "stopwatch_log.hpp"
#include "core/time.hpp"
#include "core/printing.hpp"

StopwatchLog StopwatchLog::Invalid(true);

bool StopwatchLog::Start(const std::string& message)
{
    if(mIsRunning || mIsFinished)
        { return false; }
    mMessage = message;
    mFormattedStartTime = Time::CurrentFormatted();
    mHighResStartTime   = Time::Current();
    return mIsRunning   = !(mIsFinished = false);
}

bool StopwatchLog::Stop()
{
    if(!mIsRunning || mIsFinished)
        { return false; }
    mHighResStopTime   = Time::Current();
    mFormattedStopTime = Time::CurrentFormatted();
    return mIsFinished = !(mIsRunning = false);
}

const std::string& StopwatchLog::StartTime() const
{ return mFormattedStartTime; }

const std::string& StopwatchLog::StopTime() const
{ return mFormattedStopTime; }

double StopwatchLog::HiResStartTime() const
{ return mHighResStartTime; }

double StopwatchLog::HiResStopTime() const
{ return mHighResStopTime; }

double StopwatchLog::Duration() const
{
    if(mIsRunning || !mIsFinished)
        { return (Time::Current() - mHighResStartTime); }
    return mHighResStopTime - mHighResStartTime;
}

const std::string& StopwatchLog::Message() const
{ return mMessage; }

void StopwatchLog::OverrideMessage(const std::string& message)
{ print_debug("Stopwatch message changed from '{}' to '{}'", mMessage, message); mMessage = message; }

bool StopwatchLog::Running() const
{ return mIsRunning; }

bool StopwatchLog::Finished() const
{ return mIsFinished; }
