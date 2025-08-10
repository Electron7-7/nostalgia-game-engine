#ifndef KEYBIND_H
#define KEYBIND_H

#include "key.hpp"
#include "safe_return.hpp"

#include <set>
#include <string>

struct KeyBind
{
public:
    KeyBind();
    KeyBind(const KeyBind& CopyFrom);
    KeyBind(const char* Command, bool OnRelease = false);

    const char* Command() const { return _command; }
    const bool OnRelease() const { return _on_release; }

    constexpr bool operator<(const KeyBind& Other) const { return (_command < Other._command); }
    constexpr bool operator>(const KeyBind& Other) const { return !(*this < Other); }

private:
    const char* _command = "Null";
    bool _on_release = false;
};

typedef std::set<KeyBind> KeyBinds;

SafeReturn<KeyBinds> GetBindings(KeyID Key);
bool try_AddBinding(const std::string& KeyName, const char* CommandName);
bool try_RemoveBinding(const std::string& KeyName, const char* CommandName);
bool try_ClearBindings(const std::string& KeyName);
void ClearAllBindings();

#endif // KEYBIND_H
