#pragma once

#include <cstdint>

namespace spiral {

    class int128 {
        std::int64_t lower, upper;
    };

    class uint128 {
        std::uint64_t lower, upper;
    };

    using int128_t = int128;
    using uint128_t = uint128;

}
