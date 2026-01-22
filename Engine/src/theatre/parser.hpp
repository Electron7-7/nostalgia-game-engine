#ifndef THEATRE_PARSER_H
#define THEATRE_PARSER_H

#include "fwd/filesystem.hpp"
#include "theatre/variable_registry.hpp"
#include "theatre/number_parser.hpp"
#include "core/id.hpp"
#include "core/farg.hpp"
#include "core/error.hpp"
#include "core/concepts.hpp"
#include "core/enum_prettifier.hpp"
#include "core/smart_pointers.hpp"
#include "math/concepts.hpp"
#include "math/glm_concepts.hpp"
#include <math/glm_format.hpp> // used when converting GLM variables to strings
#include <vector>

#define ASSERT_THING_VARIABLE(ThingVarName, InVarNames, ReturnOnFail...) \
    Farg<ThingVariable> ThingVarName{_get_variable({inNames...})}; \
    if(!ThingVarName) { return ReturnOnFail; }

namespace TheatreFile
{
    enum class ThingVarType
    { String, Bool, Number, Enum, Child, Parent, ID, None };

    // https://en.wikipedia.org/wiki/Lexical_analysis#Lexical_token_and_lexical_tokenization
    enum class TokenName
    {
        Identifier,
        Keyword,
        Separator,
        Operator,
        Literal,
        SinglelineComment,
        MultilineComment,
        Whitespace,
        None
    };

    struct Token
    {
        TokenName category{TokenName::None};
        std::string token{"\0"};
    };

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
        // If `ThingData::set_variable` was used to create a `ThingVariable` with an `ID`
        // value, `thing_uid` is assigned a value. This avoids errors due to `ThingData` not
        // having a `VariableRegistry` pointer assigned.
        //
        // This variable is ignored if `ThingVariable::type` is not set to `ThingVarType::ID`.
        ID thing_uid{};

        void clear()
        { *this = ThingVariable{}; }

        bool invalid() const noexcept
        { return type == ThingVarType::None or name.empty(); }

        bool operator!() const noexcept
        { return invalid(); }

        std::string debug_log() const noexcept
        {
            return std::format("[{}, {}, {}, {}]",
                name,
                value,
                EnumPrettifier::Prettify(type),
                thing_uid[]);
        }
    };

    using TokenArray = std::vector<Token>;
    using ThingVarArray = std::vector<ThingVariable>;

    struct ThingData
    {
    private:
        Farg<ThingVariable> _get_variable(std::initializer_list<std::string>) const;
        Error _get_id_variable(ID&, Farg<ThingVariable>, ThingVarType) const;

    public:
        PID           type{};
        std::string   name{};
        ThingVarArray variables{};
        // Thing uids are generated automatically; `ThingData::uid` should only be used for
        // assigning "reserved" uids (e.g: `UID::a_Player`).
        ID            uid{};
        ThingVariable parent_variable{};
        ThingVarArray children_variables{};
        // When a `Theatre` generates its `ThingData` vector, it will set `theatre_registry` to
        // its `VariableRegistry` member. This is used when attempting to get the value of `ID`,
        // "Child", and "Parent" variables; if it's `nullptr` when either `get_parent`,
        // `get_children`, or `get_variable` with an `ID` variable is called, the respective
        // function will return an error and leave the passed variable reference untouched.
        Shared<VariableRegistry> theatre_registry{nullptr};

        void clear() { *this = ThingData{}; }

        ID get_parent() const;
        IdSet_t get_children() const;

        template<StringType... Names>
            void set_variable(Sarg inValue, Sarg inName)
            { variables.emplace_back(inName, inValue, ThingVarType::String); }

        template<StringType... Names>
            Error set_variable(ID inValue, Sarg inName)
            {
                ThingVariable temp{inName, "", ThingVarType::ID, inValue};
                if(!inName.compare("Parent"))
                    { temp.type = ThingVarType::Parent; }
                else if(!inName.compare("Child"))
                    { temp.type = ThingVarType::Child; }

                if(theatre_registry
                    and !theatre_registry->try_GetIDName(inValue[], temp.value))
                    { return ERR_INVALID_ID; }

                if(temp.type == ThingVarType::Child)
                    { children_variables.push_back(temp); }
                else if(temp.type == ThingVarType::Parent)
                    { parent_variable = temp; }
                else
                    { variables.push_back(temp); }
                return OK;
            }

        template<StringType... Names>
            void set_variable(bool inValue, Sarg inName)
            { variables.emplace_back(inName, std::format("{}", inValue), ThingVarType::Bool); }

        template<NumberOrGLM T, StringType... Names>
            void set_variable(Farg<T> inValue, Sarg inName)
            { variables.emplace_back(inName, NumToString(inValue), ThingVarType::Number); }

        template<IsEnum T, StringType... Names>
            Error set_variable(T inValue, Sarg inName)
            {
                std::string enum_name{};
                if(!VariableRegistry::try_GetEnumName(inValue, enum_name))
                    { return ERR_INVALID; }
                variables.emplace_back(inName, enum_name, ThingVarType::Enum);
                return OK;
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

    struct TheatreData
    {
        std::string name{"Untitled Theatre"};
        uint index{ID::Invalid};
        std::vector<ThingData> data{};

        template<typename... _Args>
            auto emplace_back(_Args&&... __args) noexcept
            { return data.emplace_back(std::forward<_Args>(__args)...); }

        void push_back(Farg<ThingData> inData) noexcept { data.push_back(inData); }
        void clear() noexcept { data.clear(); }

        auto begin()        noexcept { return data.begin();  }
        auto end()          noexcept { return data.end();    }
        auto cbegin() const noexcept { return data.cbegin(); }
        auto cend()   const noexcept { return data.cend();   }
    };

    Error Parser(Farg<TokenArray> inTokens, Shared<TheatreData> outData);
    Error Lexer(Farg<FileData> inFileData, TokenArray& outTokens);
}

#undef ASSERT_THING_VARIABLE
#endif // THEATRE_PARSER_H
