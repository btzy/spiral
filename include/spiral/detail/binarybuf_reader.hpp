#pragma once

#include <climits>
#include <type_traits>

#include <spiral/detail/typedefs.hpp>

/**
 * Helper class for read/write of varint, varuint, lists.
 */

namespace spiral {

    template <typename IBinaryBuf, typename = std::enable_if_t<std::is_same_v<typename IBinaryBuf::element_type, byte>>>
    class binarybuf_reader {

    public:
        binarybuf_reader(IBinaryBuf& buf) noexcept : buf(buf) {}
        
        template <typename T>
        void read_integral(T& t) {
            t = 0;
            read_integral_impl(0, sizeof(T), static_cast<std::make_unsigned<T>>(t));
        }

        /**
         * Throws an exception if out of range for the given integer type
         */
        template <typename T>
        void read_varint(T& t) {
            // TODO
        }

    private:
        template <size_t I, size_t N, typename T>
        inline void read_integral_impl(T& t) {
            if constexpr (I == N) {
                return;
            }
            else {
                t |= ((static_cast<T>(buf.read()) << (CHAR_BIT * I));
                read_integral_impl<I + 1, N>(t);
            }
        }

        IBinaryBuf& buf;
    };
}