#ifndef CONSOLE_H
#define CONSOLE_H

namespace Console
{
    template<typename T>
        concept variable_t = NumberOrBool<T> or std::same_as<T, std::string>;

    class Variable
    {
    public:
        enum VariableType : int
        { INT_TYPE, FLOAT_TYPE, STRING_TYPE, NONE };

        enum VariableHint : int
        {
            HINT_BOOLEAN,
            HINT_RANGE,
            HINT_SELECTION,
            HINT_NONE
        };

        Variable(Sarg inName) noexcept:
            name{inName} {}
        explicit Variable(Sarg inName, bool inValue) noexcept:
            name{inName}, int_value{static_cast<int>(inValue)}, _type{INT_TYPE}, _hint{HINT_BOOLEAN} {}
        explicit Variable(Sarg inName, Sarg inValue) noexcept:
            name{inName}, string_value{inValue}, _type{STRING_TYPE} {}
        explicit Variable(Sarg inName, int inValue) noexcept:
            name{inName}, int_value{inValue}, _type{INT_TYPE} {}
        explicit Variable(Sarg inName, float inValue) noexcept:
            name{inName}, float_value{inValue}, _type{FLOAT_TYPE} {}

        explicit Variable(Sarg inName, int inValue, int inMax, int inMin = 0) noexcept:
            name{inName}, int_value{inValue}, _type{INT_TYPE},
            _hint{(inMin or inMax) ? HINT_RANGE : HINT_NONE},
            _hint_string{(_hint != HINT_RANGE) ? GlobalConstants::str_empty : std::format("{},{}", inMin, inMax)}
            {}
        explicit Variable(Sarg inName, float inValue, float inMax, float inMin = 0.0f) noexcept:
            name{inName}, float_value{inValue}, _type{FLOAT_TYPE},
            _hint{(inMin or inMax) ? HINT_RANGE : HINT_NONE},
            _hint_string{(_hint != HINT_RANGE) ? GlobalConstants::str_empty : std::format("{},{}", inMin, inMax)}
            {}
        template<RestrictToInt... Values> requires (sizeof...(Values) > 1)
            explicit Variable(Sarg inName, int inValue, Values... inValues) noexcept:
                name{inName}, int_value{inValue}, _type{INT_TYPE}, _hint{HINT_SELECTION}
                {
                    for(auto value : {inValues...})
                        { _hint_string += std::format("{},", value); }
                    _hint_string += std::to_string(inValue);
                }
        template<RestrictToFloat... Values> requires (sizeof...(Values) > 1)
            explicit Variable(Sarg inName, float inValue, Values... inValues) noexcept:
                name{inName}, float_value{inValue}, _type{FLOAT_TYPE}, _hint{HINT_SELECTION}
                {
                    for(auto value : {inValues...})
                        { _hint_string += std::format("{},", value); }
                    _hint_string += std::to_string(inValue);
                }

        VariableType type() const noexcept
        { return _type; }

        VariableHint hint() const noexcept
        { return _hint; }

        Sarg hint_string() const noexcept
        { return _hint_string; }

        std::string         name{""};
        int            int_value{0};
        float        float_value{0.0f};
        std::string string_value{""};

        constexpr bool operator==(Farg<Variable> inVar) const noexcept
        { return name == inVar.name; }

    private:
        VariableType _type{NONE};
        VariableHint _hint{HINT_NONE};
        std::string  _hint_string{};
    };
}

template<>
struct std::hash<Console::Variable>
{
    size_t operator()(Farg<Console::Variable> inVar) const noexcept
    { return std::hash<std::string>{}(inVar.name); }
};

namespace Console
{
    typedef void (*ConsoleCommandCallback_f)(Sarg inCommand);

    using Variables_t = std::vector<Variable>;
    using Commands_t  = std::vector<std::string>;
    using Callbacks_t = std::vector<ConsoleCommandCallback_f>;
    using History_t   = std::vector<std::string>;

    void Init();

    void AddCallback(ConsoleCommandCallback_f);
    void RemoveCallback(ConsoleCommandCallback_f);

    Error ProcessLine(Sarg inInput);
    History_t GetHistory(uint inFromLineNumber = 0);
    void ClearHistory(uint inFromLineNumber = 0);
    std::string GetLine(uint inLineNumber);
    void SetHistoryMaxSize(uint inSize);
    uint GetHistoryMaxSize();

    bool HasVariable(Sarg inName);
    Variables_t GetAllVariables();
    Variable GetVariable(Sarg inName);
    Error RemoveVariable(Sarg inName);

    void SetVariable(Sarg inVariableName, int inValue);
    void SetVariable(Sarg inVariableName, bool inValue);
    void SetVariable(Sarg inVariableName, float inValue);
    void SetVariable(Sarg inVariableName, Sarg inValue);

    Error AddCommand(Sarg inCommand);
    Error RemoveCommand(Sarg inCommand);
    Commands_t GetAllCommands();
};

#endif // CONSOLE_H
