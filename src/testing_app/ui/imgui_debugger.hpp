#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#include "ui/imgui_object.hpp"

#include <string>

struct TheatreData;  // Forward Declaration
struct StopwatchLog; // Forward Declaration

class imgui_Debugger : public ImGui_Object
{
public:
    bool Init();
    void Shutdown();
    void Update();

    StopwatchLog& StartStopwatch(const std::string& Message = "No Message");
    bool StopStopwatch(StopwatchLog& Stopwatch);

    void StartTheatreTiming(bool IsTheatreLoading);
    void StopTheatreTiming(bool IsTheatreLoading);

private:
    StopwatchLog& m_StartStopwatch(const std::string& Message);
    bool m_StopStopwatch(StopwatchLog& Stopwatch);
};

struct StopwatchLog
{
public:
    static StopwatchLog Invalid;

    StopwatchLog() = default;

    bool Start(const std::string& Message = "");
    bool Stop();

    const std::string& GetStartTime() const;
    const std::string& GetStopTime() const;
    double GetHiResStartTime() const;
    double GetHiResStopTime() const;
    double GetDuration() const;

    const std::string& GetMessage() const;

    bool IsRunning() const;
    bool IsFinished() const;

    constexpr bool operator==(const StopwatchLog& Other) const
    {
        return
        (
            m_Invalid          == Other.m_Invalid          &&
            m_HighResStartTime == Other.m_HighResStartTime &&
            m_IsRunning        == Other.m_IsRunning        &&
            m_IsFinished       == Other.m_IsFinished
        );
    }

private:
    friend void s_HandleAutomaticStopwatchToggle();
    StopwatchLog(bool Invalid): m_Invalid(Invalid) {}

    double m_HighResStartTime = -1.0;
    double m_HighResStopTime  = -1.0;
    bool   m_IsRunning   = false;
    bool   m_IsFinished  = false;
    bool   m_Invalid     = false;

    std::string m_FormattedStartTime = "";
    std::string m_FormattedStopTime  = "";

    std::string m_Message = "";
};

struct TheatreLog
{
public:
    std::string m_TheatreFilePath = "";

    StopwatchLog m_LoadTime;
    StopwatchLog m_UnloadTime;
};

extern imgui_Debugger* g_pDebugger;

#endif // IMGUI_DEBUGGER_H
