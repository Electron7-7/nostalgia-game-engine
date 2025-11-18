#ifndef STOPWATCH_LOG_H
#define STOPWATCH_LOG_H

#include "fwd.hpp"

#include <string>

class StopwatchLog
{
public:
    static StopwatchLog Invalid;

    StopwatchLog() = default;

    bool Start(const std::string& Message = "");
    bool Stop();
    const std::string& StartTime() const;
    const std::string& StopTime() const;
    double HiResStartTime() const;
    double HiResStopTime() const;
    double Duration() const;
    bool Running() const;
    bool Finished() const;
    const std::string& Message() const;
    void OverrideMessage(const std::string&);

    constexpr bool operator==(const StopwatchLog& Other) const
    {
        return mInvalid       == Other.mInvalid          &&
            mHighResStartTime == Other.mHighResStartTime &&
            mIsRunning        == Other.mIsRunning        &&
            mIsFinished       == Other.mIsFinished;
    }

private:
    StopwatchLog(bool Invalid): mInvalid{Invalid} {}

    double mHighResStartTime{-1.0};
    double mHighResStopTime{-1.0};
    bool   mIsRunning{false};
    bool   mIsFinished{false};
    bool   mInvalid{false};

    std::string mFormattedStartTime{""};
    std::string mFormattedStopTime{""};
    std::string mMessage{""};
};

struct TheatreLog
{
public:
    std::string name_or_file_path{""};
    StopwatchLog load_time{};
    StopwatchLog unload_time{};
};

#endif // STOPWATCH_LOG_H
