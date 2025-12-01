#include "action.hpp"

InputActions::InputActions(const std::string& action):
    actions_{action} {}

InputActions::InputActions(const std::set<std::string>& actions):
    actions_{actions} {}

InputActions::InputActions(const std::vector<std::string>& actions):
    actions_{actions.cbegin(), actions.cend()} {}

std::set<std::string>& InputActions::get()
{ return actions_; }

const std::set<std::string>& InputActions::cget() const
{ return actions_; }

std::string InputActions::formatted() const
{
    if(empty()) { return "N/A"; }
    std::string output{""};
    for(const auto& action : actions_)
        { output += action + ","; }
    output.pop_back();
    return output;
}

bool InputActions::empty() const
{ return actions_.empty(); }

bool InputActions::has(const std::string& Action) const
{ return actions_.contains(Action); }

template<StringType... Args>
bool InputActions::has_any(Args... Actions) const
{
    for(const auto& action : {Actions...})
        { if(has(action)) { return true; } }
    return false;
}

bool InputActions::add(const std::string& Action)
{ return actions_.insert(Action).second; }

template<StringType... Args>
bool InputActions::add(Args... Actions)
{
    bool any_failed{false};
    for(const auto& action : {Actions...})
        { any_failed = (add(action)) ? any_failed : true; }
    return any_failed;
}

template<StringType... Args>
StatusMap<std::string, sizeof...(Args)> InputActions::add_verbose(Args... Actions)
{
    StatusMap<std::string, sizeof...(Args)> status_map{};
    for(const auto& action : {Actions...})
        { status_map.add(action, add(action)); }
    return status_map;
}

bool InputActions::erase(const std::string& Action)
{ return actions_.erase(Action) != 0; }

bool InputActions::erase(const InputActions& Actions)
{
    bool any_succeeded{false};
    for(const auto& action : Actions)
        { any_succeeded = (erase(action)) ? true : any_succeeded; }
    return any_succeeded;
}

template<StringType... Args>
bool InputActions::erase(Args... Actions)
{
    bool any_failed{false};
    for(const auto& action : {Actions...})
        { any_failed = (erase(action)) ? any_failed : true; }
    return any_failed;
}

template<StringType... Args>
StatusMap<std::string, sizeof...(Args)> InputActions::erase_verbose(Args... Actions)
{
    StatusMap<std::string, sizeof...(Args)> status_map{};
    for(const auto& action : {Actions...})
        { status_map.add(action, erase(action)); }
    return status_map;
}

void InputActions::clear() { actions_.clear(); }

InputActions::ActionIterator InputActions::begin()  const { return actions_.begin();  }
InputActions::ActionIterator InputActions::end()    const { return actions_.end();    }
InputActions::ActionIterator InputActions::cbegin() const { return actions_.cbegin(); }
InputActions::ActionIterator InputActions::cend()   const { return actions_.cend();   }
