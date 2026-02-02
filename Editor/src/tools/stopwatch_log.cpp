#include "stopwatch_log.hpp"
#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/core/time.hpp>
#include <Nostalgia/core/log.hpp>

StopwatchLog StopwatchLog::Invalid(true);

bool StopwatchLog::Start(const std::string& message)
{
    if(mIsRunning || mIsFinished)
        { return false; }
    mMessage = message;
    mFormattedStartTime = Runtime::CurrentFormatted();
    mHighResStartTime   = Runtime::Current();
    return mIsRunning   = !(mIsFinished = false);
}

bool StopwatchLog::Stop()
{
    if(!mIsRunning || mIsFinished)
        { return false; }
    mHighResStopTime   = Runtime::Current();
    mFormattedStopTime = Runtime::CurrentFormatted();
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
        { return (Runtime::Current() - mHighResStartTime); }
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
