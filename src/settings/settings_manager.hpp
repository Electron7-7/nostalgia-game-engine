#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "safe_return.hpp"

#include <map>
#include <string>

class SettingsManager
{
public:
    static void SetSetting(const std::string& SettingName, const std::string& Setting);
    static void SetSetting(const std::string& SettingName, float Setting);
    static void SetSetting(const std::string& SettingName, int Setting);

    template<typename T>
    static SafeReturn<T> try_GetSetting(const std::string& SettingName);
    template<> SafeReturn<std::string> try_GetSetting(const std::string& SettingName);
    template<> SafeReturn<float>       try_GetSetting(const std::string& SettingName);
    template<> SafeReturn<int>         try_GetSetting(const std::string& SettingName);

private:
    static std::map<std::string, std::string> m_StringSettings;
    static std::map<std::string, int>         m_IntSettings;
    static std::map<std::string, float>       m_FloatSettings;

    // 'unsafe_GetSetting' is only to be used when the setting is GUARANTEED to exist as it performs no safety checks (for maximum performance)
    // An example use case would be: _Manager using `unsafe_GetSetting` to get the tick interval every frame
    friend struct _Manager;
    template<typename T>
    static T unsafe_GetSetting(const std::string& SettingName);
    template<> std::string unsafe_GetSetting(const std::string& SettingName);
    template<> float       unsafe_GetSetting(const std::string& SettingName);
    template<> int         unsafe_GetSetting(const std::string& SettingName);
};

struct Settings
{
    struct Engine
    {
        // Setting Names
        static constexpr const char* sName_AppName  = "Nostalgia Game Engine";
        static constexpr const char* sName_TickRate = "TickRate";
        static constexpr const char* sName_TickInterval = "TickInterval";
        // Setting Default Values
        static constexpr const char* sDefault_AppName  = "Nostalgia Game Engine";
        static constexpr int         sDefault_TickRate = 70;
        static constexpr int         sDefault_TickInterval = (1.0f / sDefault_TickRate);
    };
};

#endif // SETTINGS_MANAGER_H
