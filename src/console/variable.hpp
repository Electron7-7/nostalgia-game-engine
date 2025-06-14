#ifndef VARIABLE_H
#define VARIABLE_H
#include <string>

// NOTE: I might want to give each 'ConVar_<type>' struct a constructor that takes a std::string as the value, in case I want to pipe string data to a variable constructor

struct ConsoleVariable
{
public:
    static constexpr unsigned int INVALID = 0;
    static constexpr unsigned int STRING  = 1;
    static constexpr unsigned int INT     = 2;
    static constexpr unsigned int FLOAT   = 3;
    static constexpr unsigned int BOOLEAN = 4;

    ConsoleVariable(): ConsoleVariable(INVALID, "Empty_Variable", "EMPTY") {}

    std::string Value() const { return _value; }
    unsigned int Type() const { return _type; }
    std::string Name() const { return _name; }
    void SetValue(const std::string& NewValue) { _value = NewValue; }

protected:
    ConsoleVariable(const unsigned int Type, const std::string& Name, const std::string& Value): _type(Type), _name(Name), _value(Value) {}

    unsigned int _type = INVALID;
    std::string _name = "";
    std::string _value = "";
};

struct ConVar_string : public ConsoleVariable
{
public:
    ConVar_string(): ConVar_string("", "") {}
    ConVar_string(const std::string& Name, const std::string& Value): ConsoleVariable(ConsoleVariable::STRING, Name, Value) {}
};

struct ConVar_int : public ConsoleVariable
{
public:
    ConVar_int(): ConVar_int("", -1) {}
    ConVar_int(const std::string& Name, int Value): ConsoleVariable(ConsoleVariable::INT, Name, std::to_string(Value)) {}
};

struct ConVar_float : public ConsoleVariable
{
public:
    ConVar_float(): ConVar_float("", -1.0f) {}
    ConVar_float(const std::string& Name, int Value): ConsoleVariable(ConsoleVariable::FLOAT, Name, std::to_string(Value)) {}
};

struct ConVar_bool : public ConsoleVariable
{
public:
    ConVar_bool(): ConVar_bool("", false) {}
    ConVar_bool(const std::string& Name, bool Value): ConsoleVariable(ConsoleVariable::BOOLEAN, Name, std::to_string(Value)) {}
};

#endif // VARIABLE_H
