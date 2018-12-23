#pragma once

/**
 * Constants for typeids.
 */

#include <spiral/detail/typedefs.hpp>

namespace spiral {

    using typeid_underlying_t = ssize_t;

    namespace TypeIDs {

        // Weak typedef here, because this enum just defines constants for use.
        // Spiral represents varint with ssize_t.
        enum : typeid_underlying_t {
            Array = -0x1,
            I8 = -0x20,
            I16 = -0x21,
            I32 = -0x22,
            I64 = -0x23,
            I128 = -0x24,
            F32 = -0x32,
            F64 = -0x33,
            F128 = -0x34
        };
    }

}
