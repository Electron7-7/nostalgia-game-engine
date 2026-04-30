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

        Variable(Sarg inName) noexcept:
            name{inName} {}
        explicit Variable(Sarg inName, int inValue) noexcept:
            name{inName}, int_value{inValue}, _type{INT_TYPE} {}
        explicit Variable(Sarg inName, bool inValue) noexcept:
            name{inName}, int_value{static_cast<int>(inValue)}, _type{INT_TYPE} {}
        explicit Variable(Sarg inName, float inValue) noexcept:
            name{inName}, float_value{inValue}, _type{FLOAT_TYPE} {}
        explicit Variable(Sarg inName, Sarg inValue) noexcept:
            name{inName}, string_value{inValue}, _type{STRING_TYPE} {}

        VariableType type() const noexcept
        { return _type; }

        std::string         name{""};
        int            int_value{0};
        float        float_value{0.0f};
        std::string string_value{""};

        constexpr bool operator==(Farg<Variable> inVar) const noexcept
        { return name == inVar.name; }

    private:
        VariableType _type{NONE};
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

    using Variables_t = std::unordered_set<Variable>;
    using Commands_t  = std::unordered_set<std::string>;
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
    Farg<Variables_t> GetAllVariables();
    Farg<Variable>  GetVariable(Sarg inName);
    Error RemoveVariable(Sarg inName);

    void SetVariable(Sarg inVariableName, Farg<int> inValue);
    void SetVariable(Sarg inVariableName, Farg<bool> inValue);
    void SetVariable(Sarg inVariableName, Farg<float> inValue);
    void SetVariable(Sarg inVariableName, Sarg inValue);

    Error AddCommand(Sarg inCommand);
    Error RemoveCommand(Sarg inCommand);
    Farg<Commands_t> GetAllCommands();
};

#endif // CONSOLE_H
