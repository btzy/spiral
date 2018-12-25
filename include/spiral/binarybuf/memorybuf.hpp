#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

#include <spiral/binarybuf/exceptions.hpp>

namespace spiral {
    using size_t = std::size_t;
    
    // requires: T is PODType, C is a type with begin() and end() that returns ContiguousIterators (e.g. ContiguousContainer or span)
    template <typename T, typename C>
    class basic_imemorybuf {

    public:
        using element_type = T;
        using container_type = C;

        template <bool B = std::is_default_constructible_v<C>, typename = std::enable_if_t<B>>
        basic_imemorybuf() : _container(), _curr(std::end(_container)) {}

        template <bool B = std::is_move_constructible<C>, typename = std::enable_if_t<B>>
        basic_imemorybuf(C&& container) : _container(std::move(container)), _curr(std::begin(_container)) {}

        template <bool B = std::is_move_constructible<C>, typename = std::enable_if_t<B>>
        basic_imemorybuf(basic_imemorybuf<T, C>&& other) : _container(std::move(other._container)), _curr(std::exchange(other._curr, std::end(other._curr))) {}

        template <bool B = std::is_swappable<C>, typename = std::enable_if_t<B>>
        basic_imemorybuf& operator=(basic_imemorybuf<T, C>&& other) {
            swap(*this, other);
        }

        friend inline void swap(basic_imemorybuf<T, C>& x1, basic_imemorybuf<T, C>& x2) {
            swap(x1._container, x2._container);
            swap(x1._curr, x2._curr);
        }

        ~basic_imemorybuf() {}

        /**
         * Reads a single byte from the buffer.
         * Throws eof_exception if there are no more bytes to read.
         */
        inline T read() {
            if (!eof()) {
                return read_unchecked();
            }
            else {
                throw eof_exception("No more available bytes in memorybuf!");
            }
        }
        
        /**
         * Reads the given number of bytes from the buffer, and returns an iterator to the end of the output buffer.
         * Throws eof_exception if there are no more bytes to read.  (When an exception is thrown, zero bytes will be read.)
         */
        template <typename OutIt>
        inline T read(OutIt out_begin, size_t nbytes) {
            if (available(nbytes)) {
                return read_unchecked(out_begin, nbytes);
            }
            else {
                throw eof_exception("Not enough available bytes in memorybuf!");
            }
        }

        /**
         * Reads a single byte from the buffer.
         * Undefined behaviour if there are no more bytes to read.
         */
        inline T read_unchecked() {
            return *_curr++;
        }

        /**
         * Reads the given number of bytes from the buffer, and returns an iterator to the end of the output buffer.
         * Undefined behaviour if there are not enough bytes to read.
         */
        template <typename OutIt>
        inline OutIt read_unchecked(OutIt out_begin, size_t nbytes) {
            return std::move(_curr, _curr + nbytes, out_begin);
        }

        /**
         * Checks whether there are no more bytes to read.
         */
        inline bool eof() {
            return _curr == std::end(_container);
        }

        /**
         * Checks whether there are at least nbytes to read.
         */
        inline bool available(size_t nbytes) {
            return nbytes <= std::distance(_curr, std::end(_container));
        }

    private:
        C _container;
        typename C::iterator _curr;

    };

    template <typename C>
    using memorybuf = basic_imemorybuf<typename C::element_type, C>;

}
