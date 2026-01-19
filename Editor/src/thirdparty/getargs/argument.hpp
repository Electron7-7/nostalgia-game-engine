#ifndef ARGUMENT_H
#define ARGUMENT_H

struct _Arg
{
public:
    _Arg(const char* LongName, const char* ShortName);
    _Arg(const _Arg& CopyFrom);
    _Arg(const char* Name);

    const bool IsActive() const;
    const char* LongName()  const;
    const char* ShortName() const;
    const char* PrettyName() const; // Returns a name, prioritizing '_long_name'

    const bool operator==(const _Arg& other) const;
    const bool operator!=(const _Arg& other) const;
    const bool operator==(const char* string) const;
    const bool operator!=(const char* string) const;

protected:
    friend struct Option;

    const char* _long_name = "";
    const char* _short_name = "";
    bool _active = false;
};

struct Flag : public _Arg
{
public:
    using _Arg::_Arg;

    void Activate();
    void Deactivate();
};

struct Option : public _Arg
{
public:
    Option(const char* LongName, const char* ShortName, bool IsOptionMandatory = false);
    Option(const Option& CopyFrom);
    Option(const char* Name, bool IsOptionMandatory = false);

    void Activate();
    void Deactivate();

    void SetValue(const char* Value);
    void EraseValue();

    const char* GetValue() const;
    bool HasValue() const;
    bool IsOptionMandatory() const;

private:
    const char* _value = "";
    bool _is_option_mandatory = false;
};

#endif // ARGUMENT_H
