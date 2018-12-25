#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/record.hpp>
#include <spiral/detail/sharedrecord.hpp>
#include <spiral/detail/function.hpp>
#include <spiral/detail/import.hpp>
#include <spiral/detail/export.hpp>
#include <spiral/detail/instruction.hpp>

#include <spiral/detail/span-lite/span.hpp>

#include <spiral/binarybuf/memorybuf.hpp>

namespace spiral {

    template <typename T, ssize_t Extent = nonstd::dynamic_extent>
    using span = nonstd::span<T, Extent>;


    /**
     * Represents spiral bytecode (in uncompiled form).
     */
    class Module {
    public:
        Module() = default;
        Module(Module&&) = default;
        Module(const Module&) = default;
        Module& operator=(Module&&) = default;
        Module& operator=(const Module&) = default;

        /**
         * Constructs a module from a raw byte buffer.
         */
        template <typename IBinaryBuf, typename = std::enable_if_t<std::is_same_v<typename IBinaryBuf::element_type, byte>>>
        void read(IBinaryBuf& binary_buf) {
        }

    private:
        std::vector<Record> records;
        std::vector<SharedRecord> sharedrecords;
        std::vector<Function> functions;
        std::vector<Import> imports;
        std::vector<Export> exports;
        std::vector<Instruction> instructions;
    };

    
}
