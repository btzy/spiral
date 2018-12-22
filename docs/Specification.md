# Specification for Spiral IR

The specification is based on the WebAssembly binary encoding.  A major difference is that WebAssembly works on a single contiguous memory but Spiral works with dynamically-allocated arrays (using an `Array<T>` type).  Every `malloc/free` requires a host call.  Arrays are bounds-checked (unless the index is proved to be valid by other means).

In general, variable-length lists are stored with a `varuint` size followed by the entries.

Signed integers are stored in 2's compliment.

#### `string`

Field | Type | Description
--- | --- | ---
num_characters | `varuint` | Number of bytes in the UTF8 string
characters | `uint8*` | The string content

## Undefined behaviour

Undefined behaviour means that anything can happen (even backward in time).

For example:

```cpp
resize(a, 6);
for (int i = 0; i < 8; ++i) {
    print(a[i]);
}
```

The above code might not print anything at all (not even a[0] to a[5]).

# Module structure

## High-level structure

A module consists of a preamble and some sections.  Each section may be present 0 or 1 times, and they must be in the order of increasing section code.

Field | Type | Description
--- | --- | ---
preamble | `preamble` | Module preamble
num_sections | `varuint` | Number of sections in this module
sections | `section*` | List of sections in this module

### `preamble`

Field | Type | Description
--- | --- | ---
magic | `uint64` | Magic constant "spiralIR" (not null-terminated)
version | `varuint` | Version number, `0x1`

### `section`

Field | Type | Description
--- | --- | ---
section_code | `varuint` | Section code
section_length | `varuint` | Size of the payload of this section in bytes (allows skipping over the whole payload if necessary)
payload | `(see below)` | The content in this section

Meaning of each section code:

Code | Name | Description
--- | --- | ---
1 | Record | Struct definitions
2 | SharedRecord | Sharing of records so that import and export declarations can use them (references records in the record section)
3 | Function | Function declarations of all functions used in this module (each function must be either defined in this module or listed in the imports)
4 | Import | Import declarations (references functions in the function section)
5 | Export | Export declarations (references functions in the function section)
6 | Code | Function definitions (of all unimported functions from the function section)
7 | Global | Global variables

### Record section payload

Field | Type | Description
--- | --- | ---
num_records | `varuint` | Number of records
records | `record*` | List of records (records are assigned `typeid`s starting from 1)

#### `record`

Field | Type | Description
--- | --- | ---
num_fields | `varuint` | Number of fields in this record
field IDs | `typeid*` | List of types of fields in this record

To read `typeid`: Read one `varint`, `typename`.  If `typename` represents an array, then more information follows.  Otherwise, `typeid` consists only of `typename`.

If `typename` is positive, it represents a record type with that ID.

Otherwise:

Value of `typename` | Description
--- | ---
-0x1 | Array.  Read another `typeid`, which is the element type of the array.
-0x20 | i8
-0x21 | i16
-0x22 | i32
-0x23 | i64
-0x24 | i128
-0x32 | f32
-0x33 | f64
-0x34 | f128

Note that the values in the table above are **negative**!

### SharedRecord section payload

Field | Type | Description
--- | --- | ---
num_sharedrecords | `varuint` | Number of shared records
sharedrecords | `sharedrecord*` | List of shared records

#### `sharedrecord`

Field | Type | Description
--- | --- | ---
record_id | `varuint` | Positive integer that refers to the `typeid` of the record
record_name | `string` | Name for host binding

### Function section payload

Field | Type | Description
--- | --- | ---
num_functions | `varuint` | Number of functions
functions | `function*` | List of functions (functions are assigned positive `functionid`s starting from 1)

#### `function`

Field | Type | Description
--- | --- | ---
num_inputs | `varuint` | Number of inputs
input_types | `typeid*` | List of types of inputs
num_outputs | `varuint` | Number of outputs
output_types | `typeid*` | List of types of outputs

### Import section payload

Field | Type | Description
--- | --- | ---
num_imports | `varuint` | Number of imports
imports | `import*` | List of imports

#### `import`

Field | Type | Description
--- | --- | ---
function_id | `varuint` | Positive integer that refers to the `functionid` of the function
function_name | `string` | Name for host binding

### Export section payload

Field | Type | Description
--- | --- | ---
num_exports | `varuint` | Number of exports
exports | `export*` | List of exports

#### `export`

Field | Type | Description
--- | --- | ---
function_id | `varuint` | Positive integer that refers to the `functionid` of the function
function_name | `string` | Name for host binding

### Code section payload

Field | Type | Description
--- | --- | ---
num_codes | `varuint` | Number of code functions (this should be equal to num_functions minus num_imports)
codes | `code*` | List of codes

#### `code`

Field | Type | Description
--- | --- | ---
function_id | `varuint` | Positive integer that refers to the `functionid` of the function
local_count | `varuint` | Number of local variables
locals | `typeid*` | List of local variable types (local variables are assigned `variableid`s starting from 1)
num_bytes | `varuint` | Length of instructions in bytes (allows easy skipping)
num_instructions | `varuint` | Number of instructions
instructions | `instruction*` | List of instructions

#### `instruction`

Variables are referred to by their `variableid`. Positive `variableid` refers to local variables.  Negative `variableid` refers to function parameters (-1=first input, -2=second input, ..., -n=last input, -(n+1)=first output, ..., -(n+m)=last output; where n=number of inputs, m=number of outputs)

Each `instruction` is an opcode, followed by zero or more arguments as specified in the table below.  The argument kind (e.g. whether it is an immediate or variable) depends on the opcode.  Except otherwise stated, arguments that work on integers will accept integers of any width, as long as the width of all integral arguments are the same (the actual behaviour of the opcode depends on the width used, selected at compile time).  This allows it to work with typedefs.

`instructionid` (`varuint`) - a zero-based index into an instruction of this function (compile error if it is out of bounds).  This function returns when control reaches the end of the list of instructions; jumping to `num_instructions` means an early return from this function.
`functionid` (`varuint`) - a reference to a function (positive integer)
`immediate` - an immediate value: if an integer is required, then `varint`; if a float is required, then the exact floating point representation (in IEEE float format) of the required width

`referenceid` - refers to a variable or part of a variable; any one of the following:

* `variableid:varint` `appendage` - `variableid` with some appendage depending on the value of `variableid`

Value of `appendage`:

* If `variableid` is zero, then there is no appendage.  Zero is only valid when used as an output-only reference; it denotes an unused output (e.g. `call` operator or `result` variable; not valid for inputs)
* If `variableid` is a reference to an integral or float variable, then there is no appendage
* If `variableid` is a reference to an array, then the appendage is either (1) `0:varint`, to denote a reference to that array itself, or (2) `indexref:referenceid` `appendage`, where `indexref` is a reference to an integral type (this means it refers to the element at (zero-based) `indexref` in the array).  `appendage` follows the same rules, taking `arr[index]` to be the variable.
* If `variableid` is a reference to a record, then the appendage is either (1) `-1:varint`, to denote a reference to that record itself, or (2) `index:varint` `appendage`, where `index` is a reference to an integral type (this means it refers to the `index`-th member field in the record).  `appendage` follows the same rules, taking that member field to be the variable.

##### Control flow operators

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`unreachable` | `0x00` |  | Undefined behaviour if execution reaches this instruction
`nop` | `0x01` |  | No operation
`jmp` | `0x02` | target: `instructionid` | Jump to instruction
`jz` | `0x03` | target: `instructionid`, condition: `referenceid`  | Conditional jump to instruction (jump if `value == 0`)
`jnz` | `0x04` | target: `instructionid`, condition: `referenceid`  | Conditional jump to instruction (jump if `value != 0`)
`call` | `0x0c` | target: `functionid`, inputs...: `referenceid*`, outputs...: `referenceid*` | Call function

##### Constant operators

Works with integral and floats only.

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`imm` | `0x0f` | var: `referenceid`, value: `immediate` | Copy immediate into variable

##### Move/Copy operators

Works with all variable types.

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`copy` | `0x10` | source: `referenceid`, destination: `referenceid` | Copy immediate into variable (source is preserved)
`move` | `0x11` | source: `referenceid`, destination: `referenceid` | Move immediate into variable (source may or may not be preserved)

##### Arithmetic and comparison operators

Variables must be integral types.

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`slt` | `0x20` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Set less than (signed) `result = operand1 < operand2 ? 1 : 0`
`sltu` | `0x21` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Set less than (unsigned) `result = operand1 < operand2 ? 1 : 0`
`seq` | `0x22` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | `result == operand1 < operand2 ? 1 : 0`
`add` | `0x24` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Add (undefined behaviour on overflow/underflow) `result = operand1 + operand2`
`addu` | `0x25` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Add (wrap around on overflow/underflow) `result = operand1 + operand2`
`sub` | `0x26` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Subtract (undefined behaviour on overflow/underflow) `result = operand1 - operand2`
`subu` | `0x27` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Subtract (wrap around on overflow/underflow) `result = operand1 - operand2`
`mul` | `0x28` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Multiply (undefined behaviour on overflow/underflow) `result = operand1 * operand2`
`mulu` | `0x29` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Multiply (wrap around on overflow/underflow) `result = operand1 * operand2`
`div` | `0x2a` | dividend: `referenceid`, divisor: `referenceid`, quotient: `referenceid`, remainder: `referenceid` | Divide (undefined behaviour on overflow/underflow) `result = operand1 / operand2`
`divu` | `0x2b` | dividend: `referenceid`, divisor: `referenceid`, quotient: `referenceid`, remainder: `referenceid` | Divide (wrap around on overflow/underflow) `result = operand1 / operand2`
`mulex` | `0x2c` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Multiply with larger result (undefined behaviour on overflow/underflow) `result = operand1 * operand2`
`muluex` | `0x2d` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | Multiply with larger result (wrap around on overflow/underflow) `result = operand1 * operand2`
`divex` | `0x2e` | dividend: `referenceid`, divisor: `referenceid`, quotient: `referenceid`, remainder: `referenceid` | Divide with larger dividend (undefined behaviour on overflow/underflow, or if quotient does not fit the integer width) `result = operand1 / operand2`
`divuex` | `0x2f` | dividend: `referenceid`, divisor: `referenceid`, quotient: `referenceid`, remainder: `referenceid` | Divide with larger dividend (wrap around on overflow/underflow; undefined behaviour if quotient does not fit the integer width) `result = operand1 / operand2`

Note: `mulex`/`muluex` requires `result` to be twice the width of the other variables; `divex`/`divuex` requires `dividend` to be twice the width of the other variables.  (They map to a single instruction on x86 architecture.)

##### Logical and bitwise operators

Variables must be integral types.

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`and` | `0x30` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | `result = operand1 & operand2`
`or` | `0x31` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | `result = operand1 & operand2`
`xor` | `0x32` | operand1: `referenceid`, operand2: `referenceid`, result: `referenceid` | `result == operand1 ^ operand2`
`not` | `0x34` | operand: `referenceid`, result: `referenceid` | `result = ~operand`
`notl` | `0x35` | operand: `referenceid`, result: `referenceid` | `result = !operand`
`popcnt` | `0x36` | operand: `referenceid`, result: `referenceid` | `result` = number of "1" bits in `operand`
`clz` | `0x38` | operand: `referenceid`, result: `referenceid` | `result` = number of leading "0"s in `operand`
`ctz` | `0x39` | operand: `referenceid`, result: `referenceid` | `result` = number of trailing "0"s in `operand`
`sll` | `0x3a` | operand: `referenceid`, shamt: `referenceid`, result: `referenceid` | Shift left logical (`shamt` width may differ from the other two parameters)
`srl` | `0x3b` | operand: `referenceid`, shamt: `referenceid`, result: `referenceid` | Shift right logical (zeroes are used for padding on the left) (`shamt` width may differ from the other two parameters)
`sra` | `0x3c` | operand: `referenceid`, shamt: `referenceid`, result: `referenceid` | Shift right arithmetic (sign bits are used for padding on the left) (`shamt` width may differ from the other two parameters)
`rotl` | `0x3e` | operand: `referenceid`, shamt: `referenceid`, result: `referenceid` | Rotate left (`shamt` width may differ from the other two parameters)
`rotr` | `0x3f` | operand: `referenceid`, shamt: `referenceid`, result: `referenceid` | Rotate right (`shamt` width may differ from the other two parameters)

##### Floating point operators

TODO.

##### Array operators

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`resize` | `0x70` | array: `referenceid`, size: `referenceid` | Resize `array` to the given size (existing elements are preserved if they fit within the new size)
`create` | `0x71` | array: `referenceid`, size: `referenceid` | Resize `array` to the given size (existing elements may or may not be preserved)
`clear` | `0x72` | array: `referenceid` | Resize `array` to zero length

##### Conversion operators

In this section, the operand and result can have different width.

Name | Opcode | Arguments | Description
--- | --- | --- | ---
`conv` | `0x80` | operand: `referenceid`, result: `referenceid` | Integer or float conversion (signed integer)
`convu` | `0x81` | operand: `referenceid`, result: `referenceid` | Integer or float conversion (unsigned integer)
`reinterpret` | `0x82` | operand: `referenceid`, result: `referenceid` | Reinterpretation between integer and float types (operand and result must have the same width)

Note: when converting between floats, `conv` and `convu` are equivalent
