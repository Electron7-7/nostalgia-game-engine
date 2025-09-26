#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "safe_return.hpp"

#include <map>
#include <vector>
#include <string>

typedef SafeStatus (*CommandFunction)();

struct CommandLine
{
public:
    static bool Init();

    static void ProcessCommands();
    static void PushFront(const std::string& Command);
    static bool PopFront();

    static bool Exists(const std::string& Command);
    static bool RunCommand(const std::string& Command);
    static bool AddCommand(const std::string& Command, CommandFunction);
    static bool RemoveCommand(const std::string& Command);

    static constexpr const char* cmd_ExitProgram = "please don't do this unless you know what you're doing";
    static constexpr const char* cmd_PrototypeFullscreen = "PrototypeFullscreen";

private:
    static std::map<std::string, CommandFunction> m_sCommands;
    static std::vector<std::string> m_sActiveQueue;
    static std::vector<std::string> m_sProcessQueue;

    static bool m_sIsInitialized;
    static bool m_sIsCopyingQueue;
    static bool m_sIsProcessingCommands;
    static std::string m_sNextCommand;

    static bool ExtractNextCommand();
};

#endif // COMMAND_LINE_H
