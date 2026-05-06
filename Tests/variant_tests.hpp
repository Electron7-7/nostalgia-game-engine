#ifndef VARIANT_TESTS_H
#define VARIANT_TESTS_H

#include "../Nostalgia/Nostalgia.hpp" // IWYU pragma: keep
#include "../Nostalgia/theatre/theatre_file.hpp"
#include "../Nostalgia/things/resource_database.hpp"
#include "../Nostalgia/things/thing_factory.hpp"
#include "../Nostalgia/things/thing_data.hpp"
#include "../Nostalgia/core/variant.hpp"
#include "../Nostalgia/console/console.hpp"
#include "../Nostalgia/thirdparty/frozen/map.h"
#include "../Nostalgia/thirdparty/frozen/string.h"

namespace TheatreFile
{
    enum VariableHint : short
    {
        VARIABLE_HINT_NONE,
        VARIABLE_HINT_FLAGS,
        VARIABLE_HINT_LAYERS_2D_RENDER,
        VARIABLE_HINT_LAYERS_3D_RENDER,
        VARIABLE_HINT_FILE,
        VARIABLE_HINT_RESOURCE_TYPE,
        VARIABLE_HINT_PLACEHOLDER_TEXT,
        VARIABLE_HINT_OBJECT_ID,
    };

    enum VariableUsageFlags : int
    {
        VARIABLE_USAGE_NONE     = 0,
        VARIABLE_USAGE_EDITOR   = 1 << 1,
        VARIABLE_USAGE_GROUP    = 1 << 2,
        VARIABLE_USAGE_CATEGORY = 1 << 3,
        VARIABLE_USAGE_SUBGROUP = 1 << 4,
        VARIABLE_USAGE_BITMASK  = 1 << 5,
        VARIABLE_USAGE_ENUM     = 1 << 6,
    };

    struct ThingVariable2
    {
        std::string name{""};
        Variant value{0};
        VariableHint hint{VARIABLE_HINT_NONE};
        std::string hint_string{""};
        int usage_flags{VARIABLE_USAGE_EDITOR};

        bool invalid() const noexcept
        { return name.empty(); }
    };

    using ThingVar2Array = std::vector<ThingVariable2>;

    struct ThingData2
    {
    public:
        PID            type{ThingType::Invalid};
        std::string    name{};
        ThingVar2Array variables{};
        ThingVariable2 parent_variable{};
        ThingVar2Array children_variables{};

        std::string get_log() const noexcept
        {
            std::string output{std::format("<ThingData>\n\ttype: {}\n\tname: {}\n\tvariables:",
                type.name(), name)};
            for(FAUTO var : variables)
                { output += std::format("\n\t\t{}: {}", var.name, var.value.operator std::string()); }
            if(not children_variables.empty())
            {
                output += "\n\tchildren:";
                for(FAUTO var : children_variables)
                    { output += std::format("\n\t\t{}: {}", var.name, var.value.operator std::string()); }
            }
            if(not parent_variable.invalid())
            {
                output += std::format("\n\t\t{}: {}",
                    parent_variable.name,
                    parent_variable.value.operator std::string());
            }
            return output + "\n";
        }

        std::string get_parsable_string() const noexcept
        {
            std::string output{std::format("\n{} {}\n{{", type.name(), name)};
            for(FAUTO var : variables)
            {
                if(var.invalid())
                    { continue; }
                output += std::format("\n{} = {}", var.name, var.value.get_theatre_file_string());
            }
            for(FAUTO var : children_variables)
            {
                if(var.invalid())
                    { continue; }
                output += std::format("\nChild = {}", var.value.get_theatre_file_string());
            }
            if(not parent_variable.invalid())
                { output += std::format("\nParent = {}", parent_variable.value.get_theatre_file_string()); }
            return std::format("{}\n}}\n", output);
        }

        bool invalid() const
        { return type == ThingType::Invalid; }

        ID get_parent() const
        { return parent_variable.value.operator ID(); }

        IdSet_t get_children() const
        {
            IdSet_t _output{};
            for(FAUTO _child : children_variables)
                { _output.insert(_child.value.operator ID()); }
            return _output;
        }

        void  remove_parent()
        { parent_variable = {}; }

        Error remove_variable(Sarg inName)
        {
            if(auto found_it{_find(inName, variables)}; found_it != variables.end())
            {
                variables.erase(found_it);
                return OK;
            }
            return ERR_NOT_FOUND;
        }

        Error remove_child(Sarg inName)
        {
            if(auto found_it{_find(inName, children_variables)}; found_it != children_variables.end())
            {
                children_variables.erase(found_it);
                return OK;
            }
            return ERR_NOT_FOUND;
        }

        void set_variable(Farg<ThingVariable2> inVariable)
        {
            if(auto found_it{_find(inVariable.name, variables)}; found_it != variables.end())
            {
                *found_it = inVariable;
                return;
            }
            variables.push_back(inVariable);
        }

        template<Variant_t T, StringType... VariableNames>
            Error get_variable(T& outVariable, VariableNames... inNames)
            {
                ThingVariable2 _variable{};
                if(not _get_variable(_variable, std::forward<VariableNames>(inNames)...))
                    { return ERR_NOT_FOUND; }
                outVariable = static_cast<T>(_variable.value);
                return OK;
            }

    private:
        ThingVar2Array::iterator _find(Sarg inVariableName, ThingVar2Array& inArray)
        {
            for(auto iter{inArray.begin()}; iter != inArray.end(); ++iter)
            {
                if(unlikely(iter->name == inVariableName))
                    { return iter; }
            }
            return inArray.end();
        }

        template<StringType... Names>
            bool _get_variable(ThingVariable2& outVariable, Names... inNames)
            {
                for(FAUTO _name : {inNames...})
                {
                    if(auto found_it{_find(_name, variables)}; found_it != variables.end())
                    {
                        outVariable = *found_it;
                        return true;
                    }
                }
                return false;
            }
    };

    struct TheatreData2
    {
        uint index{ID::Invalid};
        std::string name{"Untitled Theatre"}, file_path{""};
        std::vector<ThingData2> data{};
        std::map<std::string, std::string> type_declarations{};

        template<typename... _Args>
            auto emplace_back(_Args&&... __args) noexcept
            { return data.emplace_back(std::forward<_Args>(__args)...); }

        void sort_by_priority();
        void push_back(Farg<ThingData2> inData) noexcept { data.push_back(inData); }
        void clear() noexcept { data.clear(); }

        std::string get_log() const
        {
            std::string _out{std::format("TheatreData [{}, {}]\n", name, index)};
            for(FAUTO _data : data)
                { _out += std::format("{}\n", _data.get_log()); }
            return _out;
        }

        std::string get_parsable_string() const
        {
            std::string _out{std::format("@{}#{}\n\n", name, index)};
            for(FAUTO [type, super] : type_declarations)
                { _out += std::format("declare {} inherits {}\n", type, super); }
            for(FAUTO _data : data)
                { _out += std::format("{}\n", _data.get_parsable_string()); }
            return _out;
        }


        auto begin()        noexcept { return data.begin();  }
        auto end()          noexcept { return data.end();    }
        auto cbegin() const noexcept { return data.cbegin(); }
        auto cend()   const noexcept { return data.cend();   }
        auto erase(FAUTO inIterator) noexcept { return data.erase(inIterator); }
    };

    Error Parse(Farg<TokenArray> inTokens, Shared<TheatreData2> outData);
}

#endif // VARIANT_TESTS_H
