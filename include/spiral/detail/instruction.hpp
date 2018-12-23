#pragma once

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <spiral/detail/primitives.hpp>
#include <spiral/detail/typedefs.hpp>
#include <spiral/detail/typeid.hpp>
#include <spiral/detail/function.hpp>
#include <spiral/detail/untagged_union.hpp>

namespace spiral {

    using variableid_t = ssize_t;
    using instructionid_t = size_t;

    enum class typeid_primitive_t : typeid_underlying_t {
        I8 = TypeIDs::I8,
        I16 = TypeIDs::I16,
        I32 = TypeIDs::I32,
        I64 = TypeIDs::I64,
        I128 = TypeIDs::I128,
        F32 = TypeIDs::F32,
        F64 = TypeIDs::F64,
        F128 = TypeIDs::F128
    };

    enum class typeid_integral_t : std::underlying_type_t<typeid_primitive_t> {
        I8 = TypeIDs::I8,
        I16 = TypeIDs::I16,
        I32 = TypeIDs::I32,
        I64 = TypeIDs::I64,
        I128 = TypeIDs::I128
    };

    enum class typeid_float_t : std::underlying_type_t<typeid_primitive_t> {
        F32 = TypeIDs::F32,
        F64 = TypeIDs::F64,
        F128 = TypeIDs::F128
    };

    class referenceid_t {
        variableid_t variableid;
        union {
            variableid_t array_index_variableid; // used for arrays only
            ssize_t record_fieldindex; // used for records only
        };
    };

    namespace InstructionParamTypes {
        struct Empty {};
        struct Jump {
            instructionid_t target;
        };
        struct JumpConditional {
            typeid_integral_t type;
            instructionid_t target;
            referenceid_t condition;
        };
        struct Call {
            functionid_t target;
            std::unique_ptr<referenceid_t[]> params;
        };
        union any_signed_integral {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            int128_t i128;
        };
        struct Immediate {
            typeid_primitive_t type;
            referenceid_t variable;
            any_signed_integral value;
        };
        struct Transfer {
            typeid_underlying_t type;
            referenceid_t source;
            referenceid_t destination;
        };
        struct OneOperandInt {
            typeid_integral_t type;
            referenceid_t operand;
            referenceid_t result;
        };
        struct TwoOperandInt {
            typeid_integral_t type;
            referenceid_t operand1;
            referenceid_t operand2;
            referenceid_t result;
        };
        struct BitCount {
            typeid_integral_t operand_type;
            typeid_integral_t result_type;
            referenceid_t operand;
            referenceid_t result;
        };
        struct Shift {
            typeid_integral_t type;
            typeid_integral_t shamt_type;
            referenceid_t operand;
            referenceid_t shamt;
            referenceid_t result;
        };
        struct MulEx {
            typeid_integral_t operand_type; // cannot be the largest available type
            referenceid_t operand1;
            referenceid_t operand2;
            referenceid_t result;
        };
        struct DivEx {
            typeid_integral_t result_type; // cannot be the largest available type
            referenceid_t dividend;
            referenceid_t divisor;
            referenceid_t quotient;
            referenceid_t remainder;
        };
        struct ArraySized {
            typeid_underlying_t element_type;
            referenceid_t array;
            referenceid_t size;
        };
        struct ArrayClear {
            typeid_underlying_t element_type;
            referenceid_t array;
        };
        struct Convert {
            typeid_primitive_t operand_type;
            typeid_primitive_t result_type;
            referenceid_t operand;
            referenceid_t result;
        };
        struct Reinterpret {
            typeid_primitive_t operand_type;
            typeid_primitive_t result_type; // must have same width as operand_type
            referenceid_t operand;
            referenceid_t result;
        };
    }

    enum class opcode_t : uint32_t {
        UNREACHABLE = 0x00,
        NOP = 0x01,
        JMP = 0x02,
        JZ = 0x03,
        JNZ = 0x04,
        CALL = 0x0c,

        IMM = 0x0f,

        COPY = 0x10,
        MOVE = 0x11,
        SWAP = 0x12,

        SLT = 0x20,
        SLTU = 0x21,
        SEQ = 0x22,
        ADD = 0x24,
        ADDU = 0x25,
        SUB = 0x26,
        SUBU = 0x27,
        MUL = 0x28,
        MULU = 0x29,
        DIV = 0x2a,
        DIVU = 0x2b,
        MULEX = 0x2c,
        MULUEX = 0x2d,
        DIVEX = 0x2e,
        DIVUEX = 0x2f,

        AND = 0x30,
        OR = 0x31,
        XOR = 0x32,
        NOT = 0x34,
        NOTL = 0x35,
        POPCNT = 0x36,
        CLZ = 0x38,
        CTZ = 0x39,
        SLL = 0x3a,
        SRL = 0x3b,
        SRA = 0x3c,
        ROTL = 0x3e,
        ROTR = 0x3f,

        RESIZE = 0x70,
        CREATE = 0x71,
        CLEAR = 0x72,

        CONV = 0x80,
        CONVU = 0x81,
        REINTERPRET = 0x82
    };

    inline bool is_valid(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::UNREACHABLE:
        case opcode_t::NOP:
        case opcode_t::JMP:
        case opcode_t::JZ:
        case opcode_t::JNZ:
        case opcode_t::CALL:

        case opcode_t::IMM:

        case opcode_t::COPY:
        case opcode_t::MOVE:
        case opcode_t::SWAP:

        case opcode_t::SLT:
        case opcode_t::SLTU:
        case opcode_t::SEQ:
        case opcode_t::ADD:
        case opcode_t::ADDU:
        case opcode_t::SUB:
        case opcode_t::SUBU:
        case opcode_t::MUL:
        case opcode_t::MULU:
        case opcode_t::DIV:
        case opcode_t::DIVU:
        case opcode_t::MULEX:
        case opcode_t::MULUEX:
        case opcode_t::DIVEX:
        case opcode_t::DIVUEX:

        case opcode_t::AND:
        case opcode_t::OR:
        case opcode_t::XOR:
        case opcode_t::NOT:
        case opcode_t::NOTL:
        case opcode_t::POPCNT:
        case opcode_t::CLZ:
        case opcode_t::CTZ:
        case opcode_t::SLL:
        case opcode_t::SRL:
        case opcode_t::SRA:
        case opcode_t::ROTL:
        case opcode_t::ROTR:

        case opcode_t::RESIZE:
        case opcode_t::CREATE:
        case opcode_t::CLEAR:

        case opcode_t::CONV:
        case opcode_t::CONVU:
        case opcode_t::REINTERPRET:
            return true;

        default:
            return false;
        }
    }

    inline bool is_empty_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::UNREACHABLE:
        case opcode_t::NOP:
            return true;
        default:
            return false;
        }
    }

    inline bool is_jump_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::JMP:
            return true;
        default:
            return false;
        }
    }

    inline bool is_jump_conditional_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::JZ:
        case opcode_t::JNZ:
            return true;
        default:
            return false;
        }
    }

    inline bool is_call_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::CALL:
            return true;
        default:
            return false;
        }
    }

    inline bool is_immediate_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::IMM:
            return true;
        default:
            return false;
        }
    }

    inline bool is_transfer_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::COPY:
        case opcode_t::MOVE:
        case opcode_t::SWAP:
            return true;
        default:
            return false;
        }
    }

    inline bool is_one_operand_int_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::NOT:
        case opcode_t::NOTL:
            return true;
        default:
            return false;
        }
    }

    inline bool is_two_operand_int_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::SLT:
        case opcode_t::SLTU:
        case opcode_t::SEQ:
        case opcode_t::ADD:
        case opcode_t::ADDU:
        case opcode_t::SUB:
        case opcode_t::SUBU:
        case opcode_t::MUL:
        case opcode_t::MULU:
        case opcode_t::DIV:
        case opcode_t::DIVU:
        case opcode_t::AND:
        case opcode_t::OR:
        case opcode_t::XOR:
            return true;
        default:
            return false;
        }
    }

    inline bool is_bit_count_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::POPCNT:
        case opcode_t::CLZ:
        case opcode_t::CTZ:
            return true;
        default:
            return false;
        }
    }

    inline bool is_shift_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::SLL:
        case opcode_t::SRL:
        case opcode_t::SRA:
        case opcode_t::ROTL:
        case opcode_t::ROTR:
            return true;
        default:
            return false;
        }
    }

    inline bool is_mul_ex_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::MULEX:
        case opcode_t::MULUEX:
            return true;
        default:
            return false;
        }
    }

    inline bool is_div_ex_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::DIVEX:
        case opcode_t::DIVUEX:
            return true;
        default:
            return false;
        }
    }

    inline bool is_array_sized_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::RESIZE:
        case opcode_t::CREATE:
            return true;
        default:
            return false;
        }
    }

    inline bool is_array_clear_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::CLEAR:
            return true;
        default:
            return false;
        }
    }

    inline bool is_convert_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::CONV:
        case opcode_t::CONVU:
            return true;
        default:
            return false;
        }
    }

    inline bool is_reinterpret_instruction(opcode_t opcode) {
        switch (opcode) {
        case opcode_t::REINTERPRET:
            return true;
        default:
            return false;
        }
    }

    template <typename Callback>
    inline void switch_by_opcode_param_type(opcode_t opcode, Callback&& callback) {
        assert(is_valid(opcode));
        if (is_empty_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Empty>{});
        }
        else if (is_jump_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Jump>{});
        }
        else if (is_jump_conditional_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::JumpConditional>{});
        }
        else if (is_call_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Call>{});
        }
        else if (is_immediate_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Immediate>{});
        }
        else if (is_transfer_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Transfer>{});
        }
        else if (is_one_operand_int_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::OneOperandInt>{});
        }
        else if (is_two_operand_int_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::TwoOperandInt>{});
        }
        else if (is_bit_count_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::BitCount>{});
        }
        else if (is_shift_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Shift>{});
        }
        else if (is_mul_ex_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::MulEx>{});
        }
        else if (is_div_ex_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::DivEx>{});
        }
        else if (is_array_sized_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::ArraySized>{});
        }
        else if (is_array_clear_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::ArrayClear>{});
        }
        else if (is_convert_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Convert>{});
        }
        else if (is_reinterpret_instruction(opcode)) {
            std::forward<Callback>(callback)(tag<InstructionParamTypes::Reinterpret>{});
        }
        else {
            assert(false);
        }
    }

    class Instruction {
    public:
        template <typename Callback>
        inline void get_params(Callback&& callback) & {
            assert(is_valid(opcode));
            switch_by_opcode_param_type(opcode, [&, this](const auto params_tag) {
                std::forward<Callback>(callback)(paramdata.get<typename decltype(params_tag)::type>());
            });
        }
        template <typename Callback>
        inline void get_params(Callback&& callback) && {
            assert(is_valid(opcode));
            switch_by_opcode_param_type(opcode, [&, this](const auto params_tag) {
                std::forward<Callback>(callback)(std::move(paramdata).get<typename decltype(params_tag)::type>());
            });
        }
        template <typename Callback>
        inline void get_params(Callback&& callback) const & {
            assert(is_valid(opcode));
            switch_by_opcode_param_type(opcode, [&, this](const auto params_tag) {
                std::forward<Callback>(callback)(paramdata.get<typename decltype(params_tag)::type>());
            });
        }
    private:
        opcode_t opcode;
        untagged_union<
            InstructionParamTypes::Empty,
            InstructionParamTypes::Jump,
            InstructionParamTypes::JumpConditional,
            InstructionParamTypes::Call,
            InstructionParamTypes::Immediate,
            InstructionParamTypes::Transfer,
            InstructionParamTypes::OneOperandInt,
            InstructionParamTypes::TwoOperandInt,
            InstructionParamTypes::BitCount,
            InstructionParamTypes::Shift,
            InstructionParamTypes::MulEx,
            InstructionParamTypes::DivEx,
            InstructionParamTypes::ArraySized,
            InstructionParamTypes::ArrayClear,
            InstructionParamTypes::Convert,
            InstructionParamTypes::Reinterpret> paramdata;
        template <typename U, typename... Args, typename = std::void_t<decltype(paramdata.emplace<U>(std::declval<Args>()...))>>
        Instruction(opcode_t opcode, Args&&... args) : opcode(opcode) {
            paramdata.emplace<U>(std::forward<Args>(args)...);
        }
        ~Instruction() {
            switch_by_opcode_param_type(opcode, [this](const auto param_tag) {
                paramdata.destroy<typename decltype(param_tag)::type>();
            });
        }
    };

}
