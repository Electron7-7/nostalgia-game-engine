#ifndef THING_DATA_H
#define THING_DATA_H

#include <Nostalgia/things/thing_variable.hpp>
#include <Nostalgia/things/thing_factory.hpp>
#include <Nostalgia/things/thing.hpp>

namespace TheatreFile
{
    struct ThingData
    {
    public:
        PID           type{};
        std::string   name{};
        ThingVarArray variables{};
        ThingVariable parent_variable{ID::Invalid};
        ThingVarArray children_variables{};

        std::string get_log() const noexcept;
        std::string get_parsable_string() const noexcept;

        bool invalid() const;

        void remove_parent();
        void set_parent(Sarg inParentName);
        ID get_parent() const;

        Error remove_child(Sarg inChildName);
        void add_child(Sarg inChildName);
        IdSet_t get_children() const;

        Error remove_variable(Sarg inName);

        template<Variant_t T>
            void set_variable(Farg<T> inValue, Sarg inName,
                VariableHint inHint = VARIABLE_HINT_NONE,
                Sarg inHintString = GlobalConstants::str_empty,
                VariableUsageFlags inUsageFlags = VARIABLE_USAGE_EDITOR)
            {
                if(unlikely(inName == "Child"))
                    { return add_child(Variant{inValue}); }
                else if(unlikely(inName == "Parent"))
                    { return set_parent(Variant{inValue}); }
                else if(auto found_it{_find_variable(inName)}; found_it != variables.end())
                {
                    found_it->value = inValue;
                    found_it->name  = inName;
                    if(inHint != VARIABLE_HINT_MAX)
                    {
                        found_it->hint = inHint;
                        if(not inHintString.empty())
                            { found_it->hint_string = inHintString; }
                    }
                    if(inUsageFlags != VARIABLE_USAGE_INVALID)
                        { found_it->usage_flags = inUsageFlags; }
                    return;
                }
                else if constexpr(IsEnum<T>)
                {
                    variables.emplace_back(inValue,
                        inName,
                        VARIABLE_HINT_ENUM,
                        EnumRegistry::GetEnumsList<T>());
                }
                else if constexpr(std::same_as<T, BitMask>)
                    { variables.emplace_back(inValue, inName, VARIABLE_HINT_FLAGS); }
                else if constexpr(std::same_as<T, ID>)
                    { variables.emplace_back(inValue, inName, VARIABLE_HINT_THING_REFERENCE); }
                else
                {
                    VariableHint _hint{inHint};
                    VariableUsageFlags _usage_flags{(inUsageFlags == VARIABLE_USAGE_INVALID)
                        ? VARIABLE_USAGE_EDITOR : inUsageFlags};
                    std::string _hint_string{inHintString};
                    if(_hint == VARIABLE_HINT_MAX)
                        { _hint = VARIABLE_HINT_NONE; }
                    else if(_hint == VARIABLE_HINT_LINK_VALUES and _hint_string.empty())
                        { _hint_string = "ON"; }
                    variables.emplace_back(inValue, inName, _hint, _hint_string, _usage_flags);
                }
            }

        template<Thing_t T> requires (not std::same_as<T, Thing>)
            void set_variable(Farg<Shared<T>> inValue, Sarg inName)
            { set_variable((inValue) ? inValue->name() : "", inName, VARIABLE_HINT_THING_REFERENCE); }

        template<Variant_t T, StringType... VariableNames>
            Error get_variable(T& outVariable, VariableNames... inNames) const
            {
                ThingVariable _variable{};
                if(not _get_variable(_variable, std::forward<VariableNames>(inNames)...))
                    { return ERR_NOT_FOUND; }
                else if(std::same_as<T, std::string> and _variable.value.type() != Variant::STRING)
                    { return ERR_INVALID_TYPE; }
                outVariable = static_cast<T>(_variable.value);
                return OK;
            }

        template<Thing_t T, StringType... VariableNames>
            Error get_variable(Shared<T>& outVariable, VariableNames... inNames) const
            {
                ThingVariable _variable{};
                if(not _get_variable(_variable, std::forward<VariableNames>(inNames)...))
                    { return ERR_NOT_FOUND; }
                else if(auto _thing{DCast<T>(_variable.value.operator Shared<Thing>())})
                {
                    outVariable = _thing;
                    return OK;
                }
                outVariable = T::Invalid();
                return ERR_INVALID;
            }

        template<StringType... VariableNames>
            void hide_from_editor(VariableNames... inNames)
            {
                for(FAUTO _name : {inNames...})
                {
                    if(auto found_it{_find_variable(_name)}; found_it != variables.end())
                        { found_it->usage_flags &= ~VARIABLE_USAGE_EDITOR; }
                }
            }

        template<StringType... VariableNames>
            void show_in_editor(VariableNames... inNames)
            {
                for(FAUTO _name : {inNames...})
                {
                    if(auto found_it{_find_variable(_name)}; found_it != variables.end())
                        { found_it->usage_flags |= VARIABLE_USAGE_EDITOR; }
                }
            }

    private:
        ThingVarArray::const_iterator _find_variable(Sarg inVariableName) const;
        ThingVarArray::iterator _find_variable(Sarg inVariableName);
        ThingVarArray::const_iterator _find_child(Sarg inChildName) const;
        ThingVarArray::iterator _find_child(Sarg inChildName);

        template<StringType... Names>
            bool _get_variable(ThingVariable& outVariable, Names... inNames) const
            {
                for(FAUTO _name : {inNames...})
                {
                    if(auto found_it{_find_variable(_name)}; found_it != variables.cend())
                    {
                        outVariable = *found_it;
                        return true;
                    }
                }
                return false;
            }
    };
}

#undef ASSERT_THING_VARIABLE
#endif // THING_DATA_H
