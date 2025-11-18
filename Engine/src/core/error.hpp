#ifndef ERROR_H
#define ERROR_H

// Modelled after GDScript's `Error` enum:
// https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#enum-globalscope-error
enum Error
{
    NOT_IMPLEMENTED = -1, // Unimplemented code; not techincally an error but not successful, either
    OK = 1, // Successfull
    FAILED = 0, // Generic Error
    ERR_INIT_FAILED, // Failure during initialization of some object
    ERR_SWITCH_DEFAULT, // A switch statement's `default` case was met (and is considered an error)
    ERR_INDEX_OUT_OF_BOUNDS,
    ERR_NOT_FOUND, // Search failure
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_FILE_READ_WRITE,
    ERR_FILE_OTHER, // Generic file-related error
};

#endif // ERROR_H
