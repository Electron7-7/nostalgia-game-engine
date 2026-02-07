#ifndef CONSOLE_H
#define CONSOLE_H

#include <Nostalgia/fwd/core.hpp>

namespace Console
{
    struct Variable
    {
        enum Type { STRING_TYPE, FLOAT_TYPE, INTEGER_TYPE };
        Type        type{STRING_TYPE};
        std::string name{""};
        std::string string_value{""};
        int         int_value{0};
        float       float_value{0.0f};

        explicit Variable() noexcept = default;
        explicit Variable(Sarg inName, Sarg inValue) noexcept:
            type{STRING_TYPE},
            name{inName},
            string_value{inValue} {}
        explicit Variable(Sarg inName, int inValue) noexcept:
            type{INTEGER_TYPE},
            name{inName},
            int_value{inValue} {}
        explicit Variable(Sarg inName, bool inValue) noexcept:
            type{INTEGER_TYPE},
            name{inName},
            int_value{inValue} {}
        explicit Variable(Sarg inName, float inValue) noexcept:
            type{FLOAT_TYPE},
            name{inName},
            float_value{inValue} {}
    };

    using Variables = std::vector<Shared<Variable>>;
    using Commands  = std::set<std::string>;

    typedef void (*ConsoleCommandCallback_f)(Sarg inCommand);

    void SetCommandCallback(ConsoleCommandCallback_f);

    Error ProcessLine(Sarg inInput);

    Shared<Variable> try_GetVariable(Sarg inName);
    Error GetVariable(Sarg inName, Shared<Variable>& outVariable);
    Error SetVariable(Sarg inName, Sarg  inValue);
    Error SetVariable(Sarg inName, int   inValue);
    Error SetVariable(Sarg inName, float inValue);
    Error AddVariable(Sarg inName, Sarg  inInitialValue);
    Error AddVariable(Sarg inName, int   inInitialValue);
    Error AddVariable(Sarg inName, float inInitialValue);
    Error RemoveVariable(Sarg inName);
    Farg<Variables> GetVariables();

    Error AddCommand(Sarg inCommand);
    Error RemoveCommand(Sarg inCommand);
    Farg<Commands> GetCommands();
};

#endif // CONSOLE_H
