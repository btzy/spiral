#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <spiral/detail/int128.hpp>

namespace spiral {

    using byte = std::byte;
    using size_t = std::size_t;
    using ssize_t = std::ptrdiff_t;
    using int8_t = std::int8_t;
    using uint8_t = std::uint8_t;
    using int16_t = std::int16_t;
    using uint16_t = std::uint16_t;
    using int32_t = std::int32_t;
    using uint32_t = std::uint32_t;
    using int64_t = std::int64_t;
    using uint64_t = std::uint64_t;
    using int128_t = spiral::int128_t;
    using uint128_t = spiral::uint128_t;

    template <typename T>
    struct tag {
        using type = T;
    };

    template <typename T>
    struct always_true : std::is_same<T, T> {};

    template <typename T>
    struct always_false : std::negation<always_true<T>> {};

}
