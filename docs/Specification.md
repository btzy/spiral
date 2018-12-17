# Specification for Spiral IR

#### `string`

Field | Type | Description
--- | --- | ---
num_characters | `varuint` | Number of bytes in the UTF8 string
characters | `uint8*` | The string content

# Module structure

## High-level structure

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
section_length | `varuint` | Size of the payload of this section in bytes
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
--- | --- | ---
1 | Array.  Read another `typeid`, which is the element type of the array.
32 | i8
33 | i16
34 | i32
35 | i64
42 | f32
43 | f64

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
