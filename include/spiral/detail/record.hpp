#pragma once

#include <vector>

#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/typeid.hpp>

namespace spiral {

    using recordid_t = size_t;

    class Record {
        std::vector<typeid_t> fields;
    };

}
