#include <vector>

#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/record.hpp>
#include <spiral/detail/sharedrecord.hpp>
#include <spiral/detail/function.hpp>
#include <spiral/detail/import.hpp>
#include <spiral/detail/export.hpp>

#include <spiral/detail/span-lite/span.hpp>

namespace spiral {

    template <typename T, ssize_t Extent = nonstd::dynamic_extent>
    using span = nonstd::span<T, Extent>;


    /**
    * Represents spiral bytecode (in uncompiled form).
    */
    class Module {
    public:
        Module() = delete;
        Module(Module&&) = default;
        Module(const Module&) = delete;
        Module& operator=(Module&&) = default;
        Module& operator=(const Module&) = delete;

        /**
        * Constructs a module from a raw byte buffer.
        */
        explicit Module(byte* buf_begin, byte* buf_end);

    private:
        std::vector<Record> records;
        std::vector<SharedRecord> sharedrecords;
        std::vector<Function> functions;
        std::vector<Import> imports;
        std::vector<Export> exports;
    };

    
}