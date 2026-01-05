#ifndef ERROR_H
#define ERROR_H

#include "core/printing.hpp"
#include <source_location>

// Modelled after GDScript's `Error` enum:
// https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#enum-globalscope-error
enum Error : int {
    OK = 0, // Successful
    UNIMPLEMENTED = -1, // Unimplemented code; `TestError` can take an argument which decides if this is an error or not
    FAILED = 1, // Generic/default error
    ERR_INIT_FAILED, // Total failure during initialization
    ERR_INIT_PARTIALLY_FAILED, // Partial failure during initialization
    ERR_SWITCH_DEFAULT, // A switch statement's `default` case was met (and is considered an error)
    ERR_INDEX_OUT_OF_BOUNDS,
    ERR_NOT_FOUND, // Generic search failure
    ERR_FILE, // Generic file error
    ERR_FILE_LOAD, // Specifically, a `FileData::LoadFile` error
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_FILE_READ_WRITE,
    ERR_DATA_LOAD, // Example use: OpenGLTextureBuffer's constructor sets `mStatus` to this if `stbi_load_from_memory` fails
    ERR_NOT_LOADED,
    ERR_INVALID_ID, // Invalid `ID` error
    ERR_INVALID_TYPE, // Invalid type error
    ERR_ALREADY_EXISTS, // Example use: InputManager::AddAction returns this if the given "action name" already exists in the unordered map
    ERR_NOT_ALLOWED, // Example use: OpenGLRendererAPI::RemoveViewport returns this if the user tries to remove the default "main window" viewport
    ERR_EMPTY, // Some container is empty
    ERR_FULL, // Some container is full
    ERR_NULLPTR, // A pointer is `nullptr`
    ERR_UNINITIALIZED, // Uninitialized data/object error
    ERR_INVALID_ENUM, // An invalid value was passed when an enum was expected (e.g: casting an out of bounds number to an enum type)
};

constexpr bool operator!(const Error& inError) noexcept
{ return inError != OK; }

const char* __get_print_message(Error error);

inline Error __print_error_enum(Error error, const std::source_location location)
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

#define print_error_enum(ENUM) __print_error_enum(ENUM, std::source_location::current())

#endif // ERROR_H
