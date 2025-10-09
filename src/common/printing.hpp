#ifndef PRINTING_H
#define PRINTING_H

#include <print>

template<class... Args>
    void print_error(std::format_string<Args...> fmt, Args&&... args)
    { std::println("\x1b[1;31m[ERROR] {}\x1b[0m",
        std::format(fmt, std::forward<Args>(args)...)); }

template<class... Args>
    void print_warning(std::format_string<Args...> fmt, Args&&... args)
    { std::println("\x1b[1;33m[WARNING] {}\x1b[0m",
            std::format(fmt, std::forward<Args>(args)...)); }

// Debug printouts are only enabled in the Debug build
#ifdef DEBUGGING
template<class... Args>
    void print_debug(std::format_string<Args...> fmt, Args&&... args)
    { std::println("\x1b[1;36m[DEBUG] {}\x1b[0m",
            std::format(fmt, std::forward<Args>(args)...)); }

template<class... Args>
    void print_debugging(std::format_string<Args...> fmt, Args&&... args)
    { std::println(fmt, std::forward<Args>(args)...); }

template<class... Args>
    void print_app_debug(std::format_string<Args...> fmt, Args&&... args)
    { std::println("\x1b[1;35m[APP DEBUG] {}\x1b[0m",
            std::format(fmt, std::forward<Args>(args)...)); }
#else  // !DEBUGGING
// The curly brackets help avoid some syntax errors
#   define print_debug(FMT,ARGS...)     {}
#   define print_debugging(FMT,ARGS...) {}
#   define print_app_debug(FMT,ARGS...) {}
#endif // DEBUGGING

#endif // PRINTING_H
