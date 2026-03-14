#ifndef ERROR_H
#define ERROR_H

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
    ERR_FILE_EXISTS, // A file already exists at a given path
    ERR_FILE_LOAD, // Specifically, a `FileData::LoadFile` error
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_FILE_READ_WRITE,
    ERR_DATA_LOAD, // Example use: OpenGLTextureBuffer's constructor sets `mStatus` to this if `stbi_load_from_memory` fails
    ERR_NOT_LOADED, // Data hasn't been loaded in yet
    ERR_INVALID, // Generic "invalid <data>" error
    ERR_INVALID_PATH, // Invalid file path error
    ERR_INVALID_ID, // Invalid `ID` error
    ERR_INVALID_TYPE, // Invalid type error
    ERR_MISMATCHED_TYPES, // Types are not equivalent
    ERR_ALREADY_EXISTS, // Example use: InputManager::AddAction returns this if the given "action name" already exists in the unordered map
    ERR_NOT_ALLOWED, // Example use: OpenGLRendererAPI::RemoveViewport returns this if the user tries to remove the default "main window" viewport
    ERR_EMPTY, // Some container is empty
    ERR_FULL, // Some container is full
    ERR_MAX, // Some number is already as large as it's allowed to be
    ERR_MIN, // Some number is already as small as it's allowed to be
    ERR_NULLPTR, // A pointer is `nullptr`
    ERR_UNINITIALIZED, // Uninitialized data/object error
    ERR_INVALID_ENUM, // An invalid value was passed when an enum was expected (e.g: casting an out of bounds number to an enum type)
    ERR_THEATRE_LEXER, // Used by `Theatre` if the lexer failed (the parser will always fail if the lexer fails, so no need to combine the two)
    ERR_THEATRE_PARSER, // Used by `Theatre` if the parser failed
};

constexpr bool operator!(const Error& inError) noexcept { return inError != OK; }

#endif // ERROR_H
