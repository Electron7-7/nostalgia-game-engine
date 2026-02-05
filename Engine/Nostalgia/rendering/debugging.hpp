#ifndef RENDERING_DEBUGGING_H
#define RENDERING_DEBUGGING_H

enum class DebugMessageSeverityFilter
{
    None = 0,
    Low,
    Medium,
    High,
};

extern bool gPrintDrawLogs;
extern bool gOpenGLEnableNotificationMesssages;
extern DebugMessageSeverityFilter gOpenGLMessageFilter;

#endif // RENDERING_DEBUGGING_H
