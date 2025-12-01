#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include "common/string_concepts.hpp"
#include "common/status_map.hpp"

#include <format>
#include <set>

struct InputActions
{
public:
    InputActions() = default;
    InputActions(const std::string& Action);
    InputActions(const std::set<std::string>& Actions);
    InputActions(const std::vector<std::string>& Actions);

    std::set<std::string>& get();
    const std::set<std::string>& cget() const;
    std::string formatted() const;
    bool empty() const;
    bool has(const std::string& Action) const;
    template<StringType... Args>
        bool has_any(Args... Actions) const;

    // Attempts to add `Action` to the actions set.
    //
    // Returns `false` if `Action` already exists.
    bool add(const std::string& Action);

    // Attempts to add `Actions` to the actions set.
    //
    // Returns `false` if any actions failed to be added.
    template<StringType... Args>
        bool add(Args... Actions);

    // Attempts to add `Actions` to the actions set.
    //
    // Returns a `StatusMap` object which can be used to determine which actions failed to be added.
    //
    // `StatusMap` objects tend to be very large, so this method is really only recommended for debugging.
    template<StringType... Args>
        StatusMap<std::string, sizeof...(Args)> add_verbose(Args... Actions);

    bool erase(const std::string& Action);
    bool erase(const InputActions& Actions);
    template<StringType... Args>
        bool erase(Args... Actions);
    template<StringType... Args>
        StatusMap<std::string, sizeof...(Args)> erase_verbose(Args... Actions);
    void clear();

    typedef
        std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>
        ActionIterator;

    ActionIterator begin()  const;
    ActionIterator end()    const;
    ActionIterator cbegin() const;
    ActionIterator cend()   const;

private:
    std::set<std::string> actions_{};
};

template<>
struct std::formatter<InputActions> : public std::formatter<std::string>
{
    auto format(const InputActions& actions, std::format_context& ctx) const
    { return std::formatter<std::string>::format(actions.formatted(), ctx); }
};

#endif // INPUT_ACTION_H
