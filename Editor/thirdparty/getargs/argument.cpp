#include "getargs/argument.hpp"
#include "getargs/compare_strings.hpp"

//-----
// _Arg
//-----
_Arg::_Arg(const char* LongName, const char* ShortName)
: _long_name(LongName), _short_name(ShortName), _active(false)
{}

_Arg::_Arg(const _Arg& CopyFrom)
: _Arg(CopyFrom._long_name, CopyFrom._short_name)
{}

_Arg::_Arg(const char* Name)
: _Arg(Name, Name)
{}

const bool  _Arg::IsActive()  const { return _active; }
const char* _Arg::LongName()  const { return _long_name;  }
const char* _Arg::ShortName() const { return _short_name; }

const char* _Arg::PrettyName() const
{
    if(CompareStrings(_long_name, "")) { return _short_name; }
    return _long_name;
}

const bool _Arg::operator==(const _Arg& other) const
{
    // FIXME: Replace '.empty()' with 'CompareStrings()' and an empty string
    if(CompareStrings(_short_name, "") || CompareStrings(other._short_name, ""))
    { return CompareStrings(_long_name, other._long_name); }

    if(CompareStrings(_long_name, "") || CompareStrings(other._long_name, ""))
    { return CompareStrings(_short_name, other._short_name); }

    return (CompareStrings(_long_name, other._long_name) || CompareStrings(_short_name, other._short_name));
}

const bool _Arg::operator!=(const _Arg& other) const
{ return !(*this == other); }

const bool _Arg::operator==(const char* string) const
{ return (CompareStrings(_short_name, string) || CompareStrings(_long_name, string)); }

const bool _Arg::operator!=(const char* string) const
{ return !(*this == string); }


//-----
// Flag
//-----
void Flag::Activate()
{ _active = true; }

void Flag::Deactivate()
{ _active = false; }


//-------
// Option
//-------
Option::Option(const char* LongName, const char* ShortName, bool IsOptionMandatory)
:_Arg(LongName, ShortName), _is_option_mandatory(IsOptionMandatory)
{}

Option::Option(const Option& CopyFrom)
: _Arg(CopyFrom._long_name, CopyFrom._short_name), _value(CopyFrom._value), _is_option_mandatory(CopyFrom._is_option_mandatory)
{}

Option::Option(const char* Name, bool IsOptionMandatory)
: Option(Name, Name, IsOptionMandatory)
{}

// TODO: It feels wrong to just copy-paste 'Activate' & 'Deactivate' into 'Option', but it works and it's pretty minor...
// ...still feels wrong tho.
void Option::Activate()
{ _active = true; }

void Option::Deactivate()
{ _active = false; }

void Option::SetValue(const char* Value)
{ _value = Value; }

void Option::EraseValue()
{ _value = ""; }

const char* Option::GetValue() const { return _value; }
bool Option::HasValue() const { return (!CompareStrings(_value, "")); }
bool Option::IsOptionMandatory() const { return _is_option_mandatory; }
