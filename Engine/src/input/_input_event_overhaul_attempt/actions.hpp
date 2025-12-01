#ifndef INPUT_ACTIONS_H
#define INPUT_ACTIONS_H

#include "common/string_concepts.hpp"
#include "common/status_map.hpp"
#include "core/type_helpers.hpp"

#include <format>
#include <set>

struct InputActions
{
public:
    typedef std::set<std::string>
        Actions;
    typedef std::vector<std::string>
        ActionsList;
    typedef std::string
        Action;
    typedef std::_Rb_tree_const_iterator<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>
        ActionIterator;

    InputActions() = default;
    InputActions(FARG(Action));
    InputActions(FARG(Actions));
    InputActions(FARG(ActionsList));

    Actions& get();
    FARG(Actions) cget() const;
    bool empty() const;
    bool has(FARG(Action)) const;
    void clear();
    bool erase(FARG(Action));
    bool erase(FARG(InputActions));
    // Attempts to add `Action` to the actions set. Returns `false` if `Action` already exists.
    bool add(FARG(Action));
    // Returns a formatted string displaying all the argument names in `arguments_`.
    std::string formatted() const;

    template<StringType... Args>
        bool has_any(Args... Actions) const;
    // Attempts to add `Actions` to the actions set. Returns `false` if any actions failed to be added.
    template<StringType... Args>
        bool add(Args... Actions);
    // Attempts to add `Actions` to the actions set. Returns a `StatusMap` object which can be used to determine
    // which actions failed to be added. `StatusMap` objects tend to be very large, so this method is really only
    // recommended for debugging.
    template<StringType... Args>
        StatusMap<std::string, sizeof...(Args)> add_verbose(Args... Actions);
    template<StringType... Args>
        bool erase(Args... Actions);
    template<StringType... Args>
        StatusMap<std::string, sizeof...(Args)> erase_verbose(Args... Actions);

    ActionIterator begin()       { return actions_.begin();  }
    ActionIterator end()         { return actions_.end();    }
    ActionIterator begin() const { return actions_.cbegin(); }
    ActionIterator end()   const { return actions_.cend();   }

    constexpr bool operator==(FARG(InputActions) other) const
    { return actions_ == other.actions_; }
    constexpr bool operator< (FARG(InputActions) other) const
    { return actions_ < other.actions_; }

private:
    Actions actions_{};
};

template<>
struct std::formatter<InputActions> : public std::formatter<std::string>
{
    auto format(const InputActions& actions, std::format_context& ctx) const
    { return std::formatter<std::string>::format(actions.formatted(), ctx); }
};

#endif // INPUT_ACTIONS_H
