#include <nostalgia_pch.hpp>

#define Case(ERROR, MESSAGE) \
    case ERROR: return #ERROR" (" MESSAGE ")";

const char* __get_print_message(Error error)
{
    switch(error)
    {
    default: return "well, congrats, you've managed to royally fuck up the error-printing function (you probably just passed it a negative integer instead of an Error, or some shit like that)";
    case OK: return nullptr;
    Case(UNIMPLEMENTED, "some functionality that hasn't been implemented yet")
    Case(FAILED, "unspecified error")
    Case(ERR_INIT_FAILED, "failure during object initialization. Look for any `::Init` or `::Initialize` class methods")
    Case(ERR_INIT_PARTIALLY_FAILED, "partial failure during object initialization. Look for any `::Init` or `::Initialize` class methods")
    Case(ERR_SWITCH_DEFAULT, "a `switch` statement's `default:` case was met and is considered an error")
    Case(ERR_INDEX_OUT_OF_BOUNDS, "generic 'index out of bounds' error")
    Case(ERR_NOT_FOUND, "generic search error")
    Case(ERR_FILE, "generic file error")
    Case(ERR_FILE_LOAD, "`FileData::LoadFile` error")
    Case(ERR_FILE_READ, "file read error")
    Case(ERR_FILE_WRITE, "file write error")
    Case(ERR_FILE_READ_WRITE, "file read/write error")
    Case(ERR_DATA_LOAD, "generic data loading error (ex: `stbi_load_from_memory`) ")
    Case(ERR_NOT_LOADED, "generic data not yet loaded error")
    Case(ERR_INVALID_ID, "invalid id number (typically `ID` or `JPH::BodyID`)")
    Case(ERR_INVALID, "generic invalid data error")
    Case(ERR_INVALID_TYPE, "invalid type PID")
    Case(ERR_INVALID_ENUM, "you probably casted an invalid number to an enum type; sit in the timeout corner, now")
    Case(ERR_MISMATCHED_TYPES, "two or more types are not equivalent")
    Case(ERR_ALREADY_EXISTS, "typically used when attempting to add an already existing key to a map")
    Case(ERR_NOT_ALLOWED, "generic 'not allowed' error")
    Case(ERR_EMPTY, "typically used to specify that an action failed because a container was empty (ex: erasing an item from an empty container)")
    Case(ERR_FULL, "typically used when trying to add an item to a full container")
    Case(ERR_NULLPTR, "received a nullptr")
    Case(ERR_UNINITIALIZED, "some object or data is uninitialized")
    Case(ERR_THEATRE_LEXER, "Theatre file lexer failure")
    Case(ERR_THEATRE_PARSER, "Theatre file parser failure")
    }
}

Error __print_error_enum(Error error, const std::source_location location)
{
    const char* msg{__get_print_message(error)};
    if(!msg)
        { return OK; }
    __print_verbose(false,
        VERBOSE1 | VERBOSE2 | VERBOSE3,
        "{}",
        location,
        (error == UNIMPLEMENTED)
            ? WarningLabel
            : ErrorLabel,
        msg);
    return (error == UNIMPLEMENTED)
        ? OK
        : error;
}
