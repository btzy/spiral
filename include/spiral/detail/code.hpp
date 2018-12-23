#pragma once

#include <vector>

#include <spiral/detail/typeid.hpp>
#include <spiral/detail/function.hpp>

namespace spiral {

    class Code {
        functionid_t functionid;
        std::vector<typeid_t> locals;
        std::vector<Instruction> instructions;
    };

}
