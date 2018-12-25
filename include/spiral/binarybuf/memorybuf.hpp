#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

#include <spiral/binarybuf/exceptions.hpp>

namespace spiral {
    using size_t = std::size_t;
    
    template <typename T>
    class basic_memorybuf {

    public:
        basic_memorybuf() noexcept : _data(), _size(nullptr), _curr(nullptr) {}
        basic_memorybuf(std::unique_ptr<byte>&& buffer, size_t size) noexcept : _data(std::move(buffer)), _size(buffer.get() + size), _curr(buffer.get()) {
            assert(buffer != nullptr || size == 0);
        }
        ~basic_memorybuf() {}

        /**
         * Reads a single byte from the buffer.
         * Throws eof_exception if there are no more bytes to read.
         */
        inline T read() {
            if (_curr < _size) {
                return *_curr++;
            }
            else {
                throw eof_exception("No more available bytes in memorybuf!");
            }
        }

    private:
        std::unique_ptr<T> _data;
        T* _size, _curr;

    };

    using memorybuf = basic_memorybuf<std::byte>;

}
