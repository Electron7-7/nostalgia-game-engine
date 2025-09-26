#include "keybind.hpp"
#include "key.hpp"
#include "printing.hpp" // IWYU pragma: keep // used in macro

#include <map>
#include <set>

std::map<KeyID, std::set<KeyBind>> KeyBind::m_sKeybinds = {};

KeyBind::KeyBind(const std::string& command, bool on_release)
: m_Command(command), m_OnRelease(on_release)
{}

KeyBind::KeyBind(const KeyBind& copy_from)
: KeyBind(copy_from.m_Command, copy_from.m_OnRelease)
{}

KeyBind::KeyBind()
: KeyBind("")
{}

SafeReturn<std::set<KeyBind>> KeyBind::GetBindings(KeyID key)
{
    if(!m_sKeybinds.contains(key) || m_sKeybinds.at(key).empty())
        { return SafeReturn<std::set<KeyBind>>({KeyBind()}, Status::KeyBindsKEY_HAS_NO_BINDS); }
    return m_sKeybinds.at(key);
}

bool KeyBind::AddBinding(const std::string& key_name, const std::string& command_name)
{
    if(!sAssertKeyname(key_name))
        { return false; }
    KeyID id = key_strings_map.at(key_name);
    m_sKeybinds[id].insert(m_sKeybinds[id].end(), command_name);
    return true;
}

bool KeyBind::RemoveBinding(const std::string& key_name, const std::string& command_name)
{
    if(!sAssertKeyname(key_name))
        { return false; }
    KeyID id = key_strings_map.at(key_name);

    if(!m_sKeybinds.contains(id) || !m_sKeybinds.at(id).contains(command_name))
        { return true; }

    auto& key_binds = m_sKeybinds.at(id);

    key_binds.erase(command_name);

    if(key_binds.size() == 0)
        { m_sKeybinds.erase(id); }

    return true;
}

bool KeyBind::ClearBindings(const std::string& key_name)
{
    if(!sAssertKeyname(key_name))
        { return false; }
    KeyID id = key_strings_map.at(key_name);

    if(!m_sKeybinds.contains(id))
        { return true; }

    m_sKeybinds.erase(id);
    return true;
}

void KeyBind::ClearAllBindings()
{ m_sKeybinds.clear(); }

bool KeyBind::sAssertKeyname(const std::string& key_name)
{
    if(!key_strings_map.contains(key_name))
    {
        PRINT_WARNING("KeyBind::sAssertKeyname - Invalid key name '{}'", key_name)
        return false;
    }
    return true;
}
