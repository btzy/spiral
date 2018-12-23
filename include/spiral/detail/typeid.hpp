#pragma once

#include <cassert>

#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/primitives.hpp>

namespace spiral {

    class typeid_t {
    
    public:
        explicit typeid_t() noexcept : curr_typeid(0) {}
        explicit typeid_t(typeid_underlying_t curr_typeid, int32_t array_dimension = 0) noexcept : curr_typeid(curr_typeid), array_dimension(array_dimension) {
            assert(curr_typeid != TypeIDs::Array);
            assert(curr_typeid != 0 || array_dimension == 0);
        }

    private:
        typeid_underlying_t curr_typeid;
        int32_t array_dimension;
    };

}
