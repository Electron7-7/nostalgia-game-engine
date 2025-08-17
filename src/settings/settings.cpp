#include "settings.hpp"

#include <string>
#include <map>

static std::map<std::string, const char*> m_StringSettings = {};
static std::map<std::string, int>         m_IntSettings = {};
static std::map<std::string, float>       m_FloatSettings = {};

void Settings::SetSetting(const char* setting_name, const char* setting)
{ m_StringSettings[setting_name] = setting; }

void Settings::SetSetting(const char* setting_name, int setting)
{ m_IntSettings[setting_name] = setting; }

void Settings::SetSetting(const char* setting_name, float setting)
{ m_FloatSettings[setting_name] = setting; }

SafeReturn<const char*> Settings::try_GetString(const char* setting_name)
{
    if(!m_StringSettings.contains(setting_name))
    { return SafeReturn("", Status::SettingsINVALID_SETTING_NAME); }

    return SafeReturn(m_StringSettings.at(setting_name));
}

SafeReturn<int> Settings::try_GetInt(const char* setting_name)
{
    if(!m_IntSettings.contains(setting_name))
    { return SafeReturn(-1, Status::SettingsINVALID_SETTING_NAME); }

    return SafeReturn(m_IntSettings.at(setting_name));
}

SafeReturn<float> Settings::try_GetFloat(const char* setting_name)
{
    if(!m_StringSettings.contains(setting_name))
    { return SafeReturn(-1.0f, Status::SettingsINVALID_SETTING_NAME); }

    return SafeReturn(m_FloatSettings.at(setting_name));
}

