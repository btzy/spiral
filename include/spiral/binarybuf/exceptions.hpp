#pragma once

#include <stdexcept>
#include <memory>
#include <string>

namespace spiral {
    
    /**
     * Base class for exceptions thrown by binarybuf library.
     */
    class io_exception : public std::runtime_error {
    public:
        explicit io_exception(const char* description) : std::runtime_error(description) {}
        explicit io_exception(const std::string& description) : std::runtime_error(description) {}
        virtual ~io_exception() noexcept {}
    };

    /**
     * Stream EOF exception.
     */
    class eof_exception : public io_exception {
    public:
        explicit eof_exception(const char* description) : std::runtime_error(description) {}
        explicit eof_exception(const std::string& description) : std::runtime_error(description) {}
        virtual ~eof_exception() noexcept {}
    };
}
