//
// Created by cosmin on 4/4/26.
//

#pragma once

#include <cstdlib>
#include <source_location>
#include <print>

namespace core
{
    [[noreturn]]
    inline void panic()
    {
        std::abort();
    }

    [[noreturn]]
    inline void panic(std::string_view message)
    {
        std::println(stderr, "{}", message);
        panic();
    }

    template<typename... Args>
    [[noreturn]]
    void panic(std::format_string<Args...> format, Args&&... args)
    {
        auto message = std::format(format, std::forward<Args>(args)...);
        std::println(stderr, "[PANIC] {}", message);
        panic();
    }

    template<typename... Args>
    [[noreturn]]
    void panic(std::source_location location, std::format_string<Args...> format, Args&&... args)
    {
        auto message = std::format(format, std::forward<Args>(args)...);
        std::println(stderr, "[PANIC] {}:{} in {}:\n{}\n", location.file_name(), location.line(), location.function_name(), message);
        panic();
    }
}

#if defined(NDEBUG)
    #define PANIC(...) core::panic(__VA_ARGS__)
#else
    #define PANIC(...) core::panic(std::source_location::current(), __VA_ARGS__)
#endif
