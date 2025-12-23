#include "error.hpp"
#include "message_labels.hpp"

#include <print>

#define Case(ERROR, MESSAGE, OTHER...)    \
    case ERROR:                           \
        err_msg = #ERROR" (" MESSAGE ")"; \
        OTHER                             \
        break;

Error print_error_enum(Error error, bool UNIMPLEMENTED_returns_OK, const std::source_location location)
{
    std::string err_msg{""};
    bool is_warning{error == UNIMPLEMENTED && UNIMPLEMENTED_returns_OK};

    switch(error)
    {
    default: error = FAILED; err_msg = "well, congrats, you've managed to royally fuck up the error-printing function (you probably just passed it a negative integer instead of an Error, or some shit like that)"; break;
    case OK: return OK;
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
    }

    std::println("\x1b[1m{}file {}({}:{}) `{}`: \x1b[22m{}\x1b[0m",
        (is_warning) ? WarningLabel() : ErrorLabel(),
        location.file_name(),
        location.line(),
        location.column(),
        location.function_name(),
        err_msg);
    return (is_warning)
        ? OK
        : error;
}

#undef Case
