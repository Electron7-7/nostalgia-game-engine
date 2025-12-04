#ifndef ERROR_H
#define ERROR_H

// Modelled after GDScript's `Error` enum:
// https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#enum-globalscope-error
enum Error {
    OK = 0, // Successful
    UNIMPLEMENTED = -1, // Unimplemented code; `TestError` can take an argument which decides if this is an error or not
    FAILED = 1, // Generic/default error
    ERR_INIT_FAILED, // Total failure during initialization
    ERR_INIT_PARTIALLY_FAILED, // Partial failure during initialization
    ERR_SWITCH_DEFAULT, // A switch statement's `default` case was met (and is considered an error)
    ERR_INDEX_OUT_OF_BOUNDS,
    ERR_NOT_FOUND, // Generic search failure
    ERR_ALREADY_EXISTS, // Example use: EventManager::AddAction returns this if the given "action name" already exists in the unordered map
    ERR_FILE, // Generic file error
    ERR_FILE_LOAD, // Specifically, a `FileData::LoadFile` error
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_FILE_READ_WRITE,
};

constexpr bool operator!(const Error& inError) noexcept
{ return inError != OK; }

#endif // ERROR_H
