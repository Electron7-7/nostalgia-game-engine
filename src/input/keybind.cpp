#include "keybind.hpp"

#include "key.hpp"

#include <map>
#include <set>

#define ASSERT_KEYNAME(KEY_NAME)            \
if(!key_strings_map.contains(KEY_NAME))     \
{ return false; }                           \
KeyID KEYID = key_strings_map.at(KEY_NAME);

static std::map<KeyID, std::set<KeyBind>> keybinds_map = {};

KeyBind::KeyBind(const char* command, bool on_release)
: _command(command), _on_release(on_release)
{}

KeyBind::KeyBind(const KeyBind& copy_from)
: KeyBind(copy_from._command, copy_from._on_release)
{}

KeyBind::KeyBind()
: KeyBind("Null")
{}

SafeReturn<KeyBinds> GetBindings(KeyID key)
{
    if(!keybinds_map.contains(key) || keybinds_map.at(key).empty())
    { return SafeReturn<KeyBinds>({KeyBind()}, Status::KeyBindsKEY_HAS_NO_BINDS); }

    return keybinds_map.at(key);
}

bool try_AddBinding(const std::string& key_name, const char* command_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID))
    { keybinds_map[KEYID] = {}; }

    keybinds_map.at(KEYID).insert(keybinds_map.at(KEYID).end(), command_name);
    return true;
}

bool try_RemoveBinding(const std::string& key_name, const char* command_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID) || !keybinds_map.at(KEYID).contains(command_name))
    { return true; }

    std::set<KeyBind>& key_binds = keybinds_map.at(KEYID);

    key_binds.erase(command_name);

    if(key_binds.size() == 0) keybinds_map.erase(KEYID);

    return true;
}

bool try_ClearBindings(const std::string& key_name)
{
    ASSERT_KEYNAME(key_name)

    if(!keybinds_map.contains(KEYID))
    { return true; }

    keybinds_map.erase(KEYID);
    return true;
}

void ClearAllBindings()
{
    keybinds_map.clear();
}
