#include "settings_manager.hpp"

std::map<std::string, std::string> SettingsManager::m_StringSettings =
{
    { Settings::Engine::sName_AppName, Settings::Engine::sDefault_AppName },
};

std::map<std::string, int> SettingsManager::m_IntSettings =
{
    { Settings::Engine::sName_TickRate, Settings::Engine::sDefault_TickRate },
};

std::map<std::string, float> SettingsManager::m_FloatSettings =
{
    { Settings::Engine::sName_TickInterval, Settings::Engine::sDefault_TickInterval },
};

void SettingsManager::SetSetting(const std::string& setting_name, const std::string& setting)
{ m_StringSettings[setting_name] = setting; }

void SettingsManager::SetSetting(const std::string& setting_name, int setting)
{ m_IntSettings[setting_name] = setting; }

void SettingsManager::SetSetting(const std::string& setting_name, float setting)
{ m_FloatSettings[setting_name] = setting; }

template<>
SafeReturn<std::string> SettingsManager::try_GetSetting(const std::string& setting_name)
{
    if(!m_StringSettings.contains(setting_name))
    { return SafeReturn<std::string>("", Status::SettingsManagerINVALID_SETTING_NAME); }

    return SafeReturn(m_StringSettings.at(setting_name));
}

template<>
SafeReturn<int> SettingsManager::try_GetSetting(const std::string& setting_name)
{
    if(!m_IntSettings.contains(setting_name))
    { return SafeReturn(-1, Status::SettingsManagerINVALID_SETTING_NAME); }

    return SafeReturn(m_IntSettings.at(setting_name));
}

template<>
SafeReturn<float> SettingsManager::try_GetSetting(const std::string& setting_name)
{
    if(!m_StringSettings.contains(setting_name))
    { return SafeReturn(-1.0f, Status::SettingsManagerINVALID_SETTING_NAME); }

    return SafeReturn(m_FloatSettings.at(setting_name));
}

template<>
std::string SettingsManager::unsafe_GetSetting(const std::string& setting_name)
{ return m_StringSettings.at(setting_name); }

template<>
int SettingsManager::unsafe_GetSetting(const std::string& setting_name)
{ return m_IntSettings.at(setting_name); }

template<>
float SettingsManager::unsafe_GetSetting(const std::string& setting_name)
{ return m_FloatSettings.at(setting_name); }
