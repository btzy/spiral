#pragma once

#include <vector>

#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/typeid.hpp>

namespace spiral {

    using functionid_t = size_t;

    class Function {
        std::vector<typeid_t> inputs;
        std::vector<typeid_t> outputs;
    };

}
