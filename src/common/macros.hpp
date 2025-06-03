#ifndef COMMON_MACROS_H
#define COMMON_MACROS_H

// Forward declaration macro
#ifdef COMPILER_FORWARD_DECLARATIONS
#define FORWARD_DECLARE(forward_declaration) forward_declaration
#else
#define FORWARD_DECLARE(forward_declaration)
#endif // COMPILER_FORWARD_DECLARATIONS

// Debug only macro
#ifdef NOSTALGIA_DEBUGGING
#define DEBUGGING(content) content
#else
#define DEBUGGING(content)
#endif // NOSTALGIA_DEBUGGING

#endif // COMMON_MACROS_H