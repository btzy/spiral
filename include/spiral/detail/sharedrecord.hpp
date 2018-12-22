#pragma once

#include <string>

#include <spiral/detail/record.hpp>

namespace spiral {

    class SharedRecord {
        recordid_t recordid;
        std::string name;
    };

}
