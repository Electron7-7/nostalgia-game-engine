#ifndef KEYBIND_H
#define KEYBIND_H

#include "key.hpp"
#include "hash.hpp"
#include "safe_return.hpp"

#include <set>
#include <string>

struct KeyBind
{
public:
    KeyBind();
    KeyBind(const KeyBind& CopyFrom);
    KeyBind(const std::string& Command, bool OnRelease = false);

    const std::string& Command() const  { return m_Command; }
    const bool OnRelease() const { return m_OnRelease; }

    constexpr bool operator<(const KeyBind& Other) const { return (ConstexprHash(m_Command) < ConstexprHash(Other.m_Command)); }
    constexpr bool operator>(const KeyBind& Other) const { return !(*this < Other); }

    static SafeReturn<std::set<KeyBind>> GetBindings(KeyID Key);
    static bool AddBinding(const std::string& KeyName, const std::string& CommandName);
    static bool RemoveBinding(const std::string& KeyName, const std::string& CommandName);
    static bool ClearBindings(const std::string& KeyName);
    static void ClearAllBindings();

private:
    std::string m_Command = "";
    bool m_OnRelease = false;

    static std::map<KeyID, std::set<KeyBind>> m_sKeybinds;

    static bool sAssertKeyname(const std::string&);
};

#endif // KEYBIND_H
