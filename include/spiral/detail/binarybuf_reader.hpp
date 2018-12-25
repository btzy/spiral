#pragma once

#include <climits>
#include <stdexcept>
#include <type_traits>

#include <spiral/detail/typedefs.hpp>

/**
 * Helper class for read/write of varint, varuint, lists.
 */

namespace spiral {

    template <typename IBinaryBuf, typename = std::enable_if_t<std::is_same_v<typename IBinaryBuf::element_type, byte>>>
    class binarybuf_reader {

        static_assert(CHAR_BIT == 8, "CHAR_BIT must be equal to 8!");
        static_assert(-1 == ~0, "2's complement integers must be used!");

    public:
        binarybuf_reader(IBinaryBuf& buf) noexcept : buf(buf) {}
        
        template <typename T>
        void read_integral(T& t) {
            t = 0;
            read_integral_impl<0, sizeof(T) * CHAR_BIT>(reinterpret_cast<std::make_unsigned<T>&>(t));
        }

        /**
         * Throws an exception if out of range for the given integer type, or if the underlying binarybuf throws exception.
         * Satisfies the basic exception guarantee: If an exception is thrown, there is no guarantee as to how much of the varint has been read (but the underlying binary buffer will be in a valid state).
         */
        template <typename T>
        void read_varint(T& t) {
            t = 0;
            read_varint_impl<0, sizeof(T) * CHAR_BIT>(t);
        }

    private:
        template <size_t I, size_t N, typename T>
        inline void read_integral_impl(T& t) {
            static_assert(I <= N, "Asserting (I <= N) failed!");
            if constexpr (I != N) {
                t |= ((static_cast<T>(buf.read()) << I);
                read_integral_impl<I + CHAR_BIT, N>(t);
            }
        }

        inline typename IBinaryBuf::element_type varint_byte_low_bits(typename IBinaryBuf::element_type x) {
            return x & ~(static_cast<typename IBinaryBuf::element_type>(1) << (CHAR_BIT - 1));
        }

        inline bool varint_byte_high_bit(typename IBinaryBuf::element_type x) {
            return (x & (static_cast<typename IBinaryBuf::element_type>(1) << (CHAR_BIT - 1))) != 0;
        }

        template <size_t I, size_t N, typename T>
        inline void read_varint_impl(T& t) {
            static_assert(I < N, "Asserting (I < N) failed!");
            using unsigned_T = std::make_unsigned<T>;

            const typename IBinaryBuf::element_type nextbyte = buf.read();

            if constexpr (I + (CHAR_BIT - 1) <= N) { // can read the whole block of (CHAR_BIT - 1) bits
                const typename IBinaryBuf::element_type lowbits = varint_byte_low_bits(nextbyte);
                const bool highbit = varint_byte_high_bit(nextbyte);

                t |= (static_cast<unsigned_T>(lowbits) << I);
                if (!highbit) {
                    // no more bytes
                    if constexpr (std::numeric_limits<T>::is_signed && I + (CHAR_BIT - 1) < N) {
                        // the integer is signed and there is at least one unwritten character
                        if (lowbits & (static_cast<typename IBinaryBuf::element_type>(1) << (CHAR_BIT - 2))) {
                            // the number is negative
                            // do sign extension:
                            t |= ~((static_cast<unsigned_T>(1) << (I + (CHAR_BIT - 1))) - static_cast<unsigned_T>(1));
                        }
                    }
                }
                else {
                    if constexpr (I + (CHAR_BIT - 1) < N) {
                        read_varint_impl<I + (CHAR_BIT - 1), N>(t);
                    }
                    else {
                        throw std::out_of_range("varint too large!");
                    }
                }
            }
            else {
                if constexpr (std::numeric_limits<T>::is_signed) {
                    constexpr typename IBinaryBuf::element_type signbit_bitmask = static_cast<typename IBinaryBuf::element_type>(1) << (N - I - 1);
                    if (nextbyte & signbit_bitmask) { // is negative number
                        // expecting 01..1X..X
                        constexpr typename IBinaryBuf::element_type excessbits_bitmask = ~((static_cast<typename IBinaryBuf::element_type>(1) << (N - I)) - static_cast<typename IBinaryBuf::element_type>(1));
                        constexpr typename IBinaryBuf::element_type testbits_bitmask = ~((~excessbits_bitmask) | static_cast<typename IBinaryBuf::element_type>(1) << (CHAR_BIT - 1));
                        if ((nextbyte & excessbits_bitmask) != testbits_bitmask) {
                            throw std::out_of_range("varint too large!");
                        }
                    }
                    else {
                        // same as unsigned case
                        // expecting 00..0X..X
                        constexpr typename IBinaryBuf::element_type excessbits_bitmask = ~((static_cast<typename IBinaryBuf::element_type>(1) << (N - I)) - static_cast<typename IBinaryBuf::element_type>(1));
                        if (nextbyte & excessbits_bitmask) {
                            throw std::out_of_range("varint too large!");
                        }
                    }
                    
                }
                else {
                    // expecting 00..0X..X
                    constexpr typename IBinaryBuf::element_type excessbits_bitmask = ~((static_cast<typename IBinaryBuf::element_type>(1) << (N - I)) - static_cast<typename IBinaryBuf::element_type>(1));
                    if (nextbyte & excessbits_bitmask) {
                        throw std::out_of_range("varint too large!");
                    }
                }
                t |= (static_cast<unsigned_T>(nextbyte) << I); // excess bits just get shifted out of the integer
            }

        }

        IBinaryBuf& buf;
    };
}