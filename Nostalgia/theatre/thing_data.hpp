#ifndef THING_DATA_H
#define THING_DATA_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/theatre/variable_registry.hpp>

#define ASSERT_THING_VARIABLE(ThingVarName, InVarNames, ReturnOnFail...) \
    Farg<ThingVariable> ThingVarName{_get_variable({inNames...})}; \
    if(!ThingVarName) { return ReturnOnFail; }

namespace TheatreFile
{
    enum class ThingVarType
    { String, Bool, Number, Enum, Child, Parent, ID, None };

    struct ThingVariable
    {
        // The name of the variable, or, if this is a Child/Parent variable, the name of the
        // Child/Parent Thing.
        std::string  name{};
        // The variable's value, represented as a string, or, if this is a Child/Parent
        // variable, the type-name of the Child/Parent Thing.
        std::string  value{};
        // The variable's type.
        ThingVarType type{ThingVarType::None};

        void clear()
        { *this = ThingVariable{}; }

        bool invalid() const noexcept
        { return type == ThingVarType::None or name.empty(); }

        bool operator!() const noexcept
        { return invalid(); }

        std::string get_log() const noexcept;
        std::string get_parsable_string() const noexcept;

    };

    using ThingVarArray = std::vector<ThingVariable>;

    struct ThingData
    {
    private:
        Farg<ThingVariable> _get_variable(std::initializer_list<std::string>) const;
        Error _get_id_variable(ID&, Farg<ThingVariable>, ThingVarType) const;
        Shared<Resource> _try_get_resource(Sarg inName) const;
        Shared<Thinker> _try_get_thinker(Sarg inName) const;

    public:
        PID           type{};
        std::string   name{};
        ThingVarArray variables{};
        ThingVariable parent_variable{};
        ThingVarArray children_variables{};

        ID _uid{}; // Should never be set manually, as it gets automatically filled in via `UID::Generate`

        std::string get_log() const noexcept;
        std::string get_parsable_string() const noexcept;

        void clear();

        ID get_parent() const;
        IdSet_t get_children() const;

        Error remove_variable(Sarg inName);
        Error remove_child(Sarg inName);

        void  set_variable(Sarg inValue, Sarg inName);
        Error set_variable(ID inValue, Sarg inName);
        void  set_variable(bool inValue, Sarg inName);
        Error set_variable(Shared<FileData> inValue, Sarg inName);

        template<NumberOrGLM T, StringType... Names>
            void set_variable(Farg<T> inValue, Sarg inName)
            { variables.emplace_back(inName, NumToString(inValue), ThingVarType::Number); }

        template<IsEnum T>
            Error set_variable(T inValue, Sarg inName)
            {
                std::string enum_name{};
                if(!VariableRegistry::try_GetEnumName(inValue, enum_name))
                    { return ERR_INVALID; }
                variables.emplace_back(inName, enum_name, ThingVarType::Enum);
                return OK;
            }

        template<typename T> requires std::derived_from<T, Resource>
            Error set_variable(Farg<Shared<T>> inValue, Sarg inName)
            {
                if(not inValue)
                    { return ERR_NULLPTR; }
                return set_variable(inValue->uid(), inName);
            }

        template<typename T> requires std::derived_from<T, Thinker>
            Error set_variable(Farg<Shared<T>> inValue, Sarg inName)
            {
                if(not inValue)
                    { return ERR_NULLPTR; }
                return set_variable(inValue->uid(), inName);
            }

        template<typename T, StringType... Names> requires std::derived_from<T, Resource>
            Error get_variable(Shared<T>& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.value.empty())
                    { return ERR_EMPTY; }
                else if(thing_var.type != ThingVarType::ID)
                    { return ERR_MISMATCHED_TYPES; }
                if(auto resource{DCast<T>(_try_get_resource(thing_var.value))})
                    { outValue = resource; return OK; }
                return ERR_NOT_FOUND;
            }

        template<typename T, StringType... Names> requires std::derived_from<T, Thinker>
            Error get_variable(Shared<T>& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.value.empty())
                    { return ERR_EMPTY; }
                else if(thing_var.type != ThingVarType::ID)
                    { return ERR_MISMATCHED_TYPES; }
                if(auto thinker{DCast<T>(_try_get_thinker(thing_var.value))})
                    { outValue = thinker; return OK; }
                return ERR_NOT_FOUND;
            }

        template<StringType... Names>
            Error get_variable(Shared<FileData>& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.value.empty())
                    { return ERR_EMPTY; }
                else if(thing_var.type == ThingVarType::String)
                {
                    auto output{MakeShared<FileData>()};
                    Error status{print_error_enum(output->LoadFile(thing_var.value))};
                    if(status == OK)
                        { outValue = output; }
                    return status;
                }
                return ERR_MISMATCHED_TYPES;
            }

        template<StringContainer T, StringType... Names>
            Error get_variable(T& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.value.empty())
                    { return ERR_EMPTY; }
                else if(thing_var.type != ThingVarType::String)
                    { return ERR_MISMATCHED_TYPES; }
                outValue = thing_var.value;
                return OK;
            }

        template<StringType... Names>
            Error get_variable(ID& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                return _get_id_variable(outValue, thing_var, ThingVarType::ID);
            }

        template<IsEnum T, StringType... Names>
            Error get_variable(T& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.type != ThingVarType::Enum)
                    { return ERR_MISMATCHED_TYPES; }
                else if(!VariableRegistry::try_GetEnum(thing_var.value, outValue))
                    { return ERR_INVALID; }
                return OK;
            }

        template<StringType... Names>
            Error get_variable(bool& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.type != ThingVarType::Bool)
                    { return ERR_MISMATCHED_TYPES; }
                else if(!thing_var.value.compare("false"))
                    { outValue = false; }
                else if(!thing_var.value.compare("true"))
                    { outValue = true; }
                else
                    { return ERR_INVALID; }
                return OK;
            }

        template<NumberOrGLM T, StringType... Names>
            Error get_variable(T& outValue, Names... inNames) const
            {
                ASSERT_THING_VARIABLE(thing_var, inNames, ERR_NOT_FOUND)
                if(thing_var.type != ThingVarType::Number)
                    { return ERR_MISMATCHED_TYPES; }
                else if(!StringToNum(outValue, thing_var.value))
                    { return ERR_INVALID; }
                return OK;
            }

        template<StringType... Names>
            int erase_variables(Names... inNames)
            {
                int erase_value{0};
                for(FAUTO name : {inNames...})
                {
                    for(auto it{variables.begin()}; it != variables.end();)
                    {
                        if(!it->name.compare(name))
                        {
                            it = variables.erase(it);
                            ++erase_value;
                            continue;
                        }
                        ++it;
                    }
                }
                return erase_value;
            }
    };
}

#undef ASSERT_THING_VARIABLE
#endif // THING_DATA_H
