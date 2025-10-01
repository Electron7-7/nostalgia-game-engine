#include "command_line.hpp"
#include "safe_return.hpp"
#include "printing.hpp"

// These commands should always be available
static const std::map<std::string, CommandFunction>
s_cCommands = {};

std::map<std::string, CommandFunction> CommandLine::m_sCommands = {};
std::vector<std::string> CommandLine::m_sActiveQueue  = {};
std::vector<std::string> CommandLine::m_sProcessQueue = {};
bool CommandLine::m_sIsInitialized = false;
bool CommandLine::m_sIsCopyingQueue = false;
bool CommandLine::m_sIsProcessingCommands = false;
std::string CommandLine::m_sNextCommand = "";

bool CommandLine::Init()
{
    if(m_sIsInitialized)
        { return true; }
    m_sIsInitialized = true;
    return true;
}

void CommandLine::ProcessCommands()
{
    if(m_sIsProcessingCommands)
        { return; }

    m_sIsProcessingCommands = true;
    m_sIsCopyingQueue = true;

    m_sProcessQueue = m_sActiveQueue;

    m_sIsCopyingQueue = false;

    while(ExtractNextCommand())
    {
        if(!m_sCommands.contains(m_sNextCommand))
        {
            PRINT_WARNING("CommandLine::ProcessCommands - invalid command '{}'", m_sNextCommand)
            continue;
        }

        SafeStatus command_status = m_sCommands.at(m_sNextCommand)();
        if(command_status.Status() != Status::NO_ERR)
            { PRINT_WARNING("CommandLine::ProcessCommands - command returned with the status: '{}'", command_status.Printout()) }
    }
    m_sIsProcessingCommands = false;
    m_sProcessQueue.clear(); // Just in case
}

void CommandLine::PushFront(const std::string& command)
{ m_sActiveQueue.emplace(m_sActiveQueue.cbegin(), command); }

bool CommandLine::PopFront()
{
    if(m_sActiveQueue.size() == 0)
        { return false; }
    m_sActiveQueue.erase(m_sActiveQueue.cbegin());
    return true;
}

bool CommandLine::Exists(const std::string& command)
{ return (m_sCommands.contains(command) || s_cCommands.contains(command)); }

bool CommandLine::RunCommand(const std::string& command)
{
    if(m_sCommands.contains(command))
    {
        m_sCommands.at(command)();
        return true;
    }
    else if(s_cCommands.contains(command))
    {
        s_cCommands.at(command)();
        return true;
    }
    return false;
}

bool CommandLine::AddCommand(const std::string& command, CommandFunction function)
{
    if(m_sCommands.contains(command))
    {
        PRINT_WARNING("CommandLine::AddCommand - const std::string& '{}' already exists", command)
        return false;
    }
    m_sCommands[command] = function;
    return true;
}

bool CommandLine::RemoveCommand(const std::string& command)
{
    if(s_cCommands.contains(command))
    {
        PRINT_WARNING("CommandLine::RemoveCommand - Command '{}' is an engine command and cannot be removed", command)
        return false;
    }
    return m_sCommands.erase(command);
}

bool CommandLine::ExtractNextCommand()
{
    if(m_sProcessQueue.size() == 0)
    {
        m_sNextCommand = "";
        return false;
    }
    m_sNextCommand = m_sProcessQueue.front();
    m_sProcessQueue.erase(m_sProcessQueue.cbegin());
    return true;
}
