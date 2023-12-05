/**
 *
 *  @file
 *  @brief Comman utility functions for decoding and encoding
 *
 **/


#ifndef DICT_FILE_H
#define DICT_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/**
 * @def BEJ_FORMAT_SET
 * @brief BEJ format for a set.
 *
 * @def BEJ_FORMAT_ARRAY
 * @brief BEJ format for an array.
 *
 * @def BEJ_FORMAT_NULL
 * @brief BEJ format for null.
 *
 * @def BEJ_FORMAT_INTEGER
 * @brief BEJ format for an integer.
 *
 * @def BEJ_FORMAT_ENUM
 * @brief BEJ format for an enumeration.
 *
 * @def BEJ_FORMAT_STRING
 * @brief BEJ format for a string.
 *
 * @def BEJ_FORMAT_REAL
 * @brief BEJ format for a real number.
 *
 * @def BEJ_FORMAT_BOOLEAN
 * @brief BEJ format for a boolean.
 *
 * @def BEJ_FORMAT_BYTE_STRING
 * @brief BEJ format for a byte string.
 *
 * @def BEJ_FORMAT_CHOICE
 * @brief BEJ format for a choice.
 *
 * @def BEJ_FORMAT_PROPERTY_ANNOTATION
 * @brief BEJ format for a property annotation.
 *
 * @def BEJ_FORMAT_REGISTRY_ITEM
 * @brief BEJ format for a registry item.
 *
 * @def BEJ_FORMAT_RESOURCE_LINK
 * @brief BEJ format for a resource link.
 *
 * @def BEJ_FORMAT_RESOURCE_LINK_EXPANSION
 * @brief BEJ format for a resource link expansion.
 *
 * @def BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA
 * @brief BEJ dictionary selector for major schema.
 *
 * @def BEJ_DICTIONARY_SELECTOR_ANNOTATION
 * @brief BEJ dictionary selector for annotation.
 *
 * @def BEJ_VERSION_1_0_0
 * @brief BEJ version 1.0.0.
 *
 * @def BEJ_VERSION_1_1_0
 * @brief BEJ version 1.1.0.
 *
 * @def BEJ_SCHEMA_CLASS_MAJOR
 * @brief BEJ major schema class.
 *
 * @def BEJ_SCHEMA_CLASS_EVENT
 * @brief BEJ event schema class.
 *
 * @def BEJ_SCHEMA_CLASS_ERROR
 * @brief BEJ error schema class.
 *
 * @def BEJ_INAVALID_SEQUENCE_NUMBER
 * @brief Invalid sequence number for dictionary entry.
 *
 * @def BEJ_PACKED_STRUCT
 * @brief Macro for packed structure.
 * @note Doxygen treats __attribute__ as a function declaration
 */
#define BEJ_FORMAT_SET                              0x00
#define BEJ_FORMAT_ARRAY                            0x01
#define BEJ_FORMAT_NULL                             0x02
#define BEJ_FORMAT_INTEGER                          0x03
#define BEJ_FORMAT_ENUM                             0x04
#define BEJ_FORMAT_STRING                           0x05
#define BEJ_FORMAT_REAL                             0x06
#define BEJ_FORMAT_BOOLEAN                          0x07
#define BEJ_FORMAT_BYTE_STRING                      0x08
#define BEJ_FORMAT_CHOICE                           0x09
#define BEJ_FORMAT_PROPERTY_ANNOTATION              0x0A
#define BEJ_FORMAT_REGISTRY_ITEM                    0x0B
#define BEJ_FORMAT_RESOURCE_LINK                    0x0E
#define BEJ_FORMAT_RESOURCE_LINK_EXPANSION          0x0F

#define BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA        0b0
#define BEJ_DICTIONARY_SELECTOR_ANNOTATION          0b1

#define BEJ_VERSION_1_0_0                           0xF1F0F000
#define BEJ_VERSION_1_1_0                           0xF1F1F000

#define BEJ_SCHEMA_CLASS_MAJOR                      0x00
#define BEJ_SCHEMA_CLASS_EVENT                      0x01
#define BEJ_SCHEMA_CLASS_ERROR                      0x04

#define BEJ_INAVALID_SEQUENCE_NUMBER                UINT64_MAX

#define BEJ_PACKED_STRUCT __attribute__((packed))

/**
 *  @struct bej_file_header
 *  @brief Structure that represents BEJ file meta data
 *
 *  @var bej_file_header::version
 *  @brief BEJ file version
 *
 *  @var bej_file_header::flags
 *  @brief BEJ file flags (should be 0)
 *
 *  @var bej_file_header::schema_class
 *  @brief BEJ file schema class entry
 *
 *  @typedef bej_file_header
 *  @brief default struct typedef
 **/
typedef struct BEJ_PACKED_STRUCT bej_file_header
{
    uint32_t version;
    uint16_t flags;
    uint8_t schema_class;
}
bej_file_header;

/**
 *  @struct bej_dict_header
 *  @brief Structure that represents Dictionary file meta
 *
 *  @var bej_dict_header::version
 *  @brief Dictionary version
 *
 *  @var bej_dict_header::flags
 *  @brief Dictionary flags
 *
 *  @var bej_dict_header::entry_count
 *  @brief Amount of entries in dictionary
 *
 *  @var bej_dict_header::schema_version
 *  @brief Dictionary schema version

 *  @var bej_dict_header::dict_size
 *  @brief Dictionary size

 *  @typedef bej_dict_header
 *  @brief default struct typedef
 **/
typedef struct BEJ_PACKED_STRUCT bej_dict_header
{
    uint8_t version;
    uint8_t flags;
    uint16_t entry_count;
    uint32_t schema_version;
    uint32_t dict_size;
}
bej_dict_header;

/**
 *  @struct bej_tuple_s
 *  @brief Structure that represents bejTupleS type
 *
 *  @var bej_tuple_s::dict_selector
 *  @brief Dictionary selector (0 bit)
 *
 *  @var bej_tuple_s::seq_num
 *  @brief Sequence number (1-n bits)
 *
 *  @typedef bej_tuple_s
 *  @brief default struct typedef
 **/
typedef struct bej_tuple_s
{
    uint8_t dict_selector   :  1;
    uint64_t seq_num        : 63;
}
bej_tuple_s;


/**
 *  @struct bej_tuple_f
 *  @brief Structure that represents bejTupleF type
 *
 *  @var bej_tuple_f::defer_bind
 *  @brief Deferred binding flag (0 bit)
 *
 *  @var bej_tuple_f::ro_and_tla
 *  @brief Read only property (in Dictionary) and
 *  top level annotation (in Schema) flag (1 bit)
 *
 *  @var bej_tuple_f::nullable
 *  @brief Nullable property flag (2 bit)
 *
 *  @var bej_tuple_f::reserved
 *  @brief Reserved flag (3 bit)
 *
 *  @var bej_tuple_f::bej_type
 *  @brief Principal data type (4-7 bits)
 *
 *  @typedef bej_tuple_f
 *  @brief default struct typedef
 **/
typedef struct bej_tuple_f
{
    uint8_t defer_bind     : 1;
    uint8_t ro_and_tla     : 1;
    uint8_t nullable       : 1;
    uint8_t reserved       : 1;

    uint8_t bej_type       : 4;
}
bej_tuple_f;


/**
 *  @typedef bej_tuple_l
 *  @brief Represents bejTupleL type
 **/
typedef uint64_t bej_tuple_l;


/**
 *  @struct bej_tuple_sfl
 *  @brief Structure that unites F, S and L tuples.
 *  Header of BEJ-encoded values
 *
 *  @var bej_tuple_sfl::sequence
 *  @brief Encoded value dictionary sequence
 *
 *  @var bej_tuple_sfl::format
 *  @brief Encoded value flags and type
 *
 *  @var bej_tuple_sfl::length
 *  @brief Encoded value length in bytes
 *
 *  @typedef bej_tuple_sfl
 *  @brief default struct typedef
 **/
typedef struct bej_tuple_sfl
{
    bej_tuple_s sequence;
    bej_tuple_f format;
    bej_tuple_l length;
}
bej_tuple_sfl;


/**
 *  @struct bej_real
 *  @brief Structure that simplifies bejReal type processing. Stores floating point number data
 *
 *  @var bej_real::whole
 *  @brief Whole part value
 *
 *  @var bej_real::lead_zeros
 *  @brief Lead zeros count
 *
 *  @var bej_real::fract
 *  @brief Fraction part value
 *
 *  @var bej_real::exponent
 *  @brief Exponent part value
 *
 *  @typedef bej_real
 *  @brief default struct typedef
 **/
typedef struct bej_real
{
    int64_t whole;
    uint64_t lead_zeros;
    uint64_t fract;
    int64_t exponent;
}
bej_real;

/**
 *  @struct bej_dict_entry_header
 *  @brief Structure that represents dictionary entry (with name).
 *
 *  @var bej_dict_entry_header::format
 *  @brief Entry format
 *
 *  @var bej_dict_entry_header::sequence
 *  @brief Entry sequence (position in child entry array of its parent)
 *
 *  @var bej_dict_entry_header::offset
 *  @brief Child entries offset
 *
 *  @var bej_dict_entry_header::child_count
 *  @brief Child entries count
 *
 *  @var bej_dict_entry_header::name_size
 *  @brief Entry name length
 *
 *  @var bej_dict_entry_header::name_ofset
 *  @brief Entry name dictionary offset
 *
 *  @typedef bej_dict_entry_header
 *  @brief default struct typedef
 **/
typedef struct BEJ_PACKED_STRUCT bej_dict_entry_header
{
    bej_tuple_f format;
    uint16_t sequence;
    uint16_t offset;
    uint16_t child_count;
    uint8_t name_size;
    uint16_t name_ofset;
}
bej_dict_entry_header;

/**
 *  @struct bej_dict_entry
 *  @brief Structure that represents dictionary entry (with name).
 *
 *  @var bej_dict_entry::header
 *  @brief Entry info
 *
 *  @var bej_dict_entry::name
 *  @brief Entry name
 *
 *  @typedef bej_dict_entry
 *  @brief default struct typedef
 **/
typedef struct bej_dict_entry
{
    bej_dict_entry_header* header;
    char* name;
}
bej_dict_entry;


/**
 *  Gets size of stream in bytes
 *
 *  @param[in]  stream          File stream
 *  @returns size of stream
 **/
off_t get_stream_size(FILE* stream);


/**
 *  Reads dictionary entry from file stream at current position
 *
 *  @param[out] entry          Dictionary entry address
 *  @param[in]  stream         Dictionary entry stream
 **/
void read_dict_entry(
    bej_dict_entry* entry,
    FILE* stream
);


/**
 *  Frees dictionary entry
 *
 *  @param[inout] entry     Dictionary entry
 **/
void free_dict_entry(bej_dict_entry* entry);


/**
 *  Reads all children of entry by its header
 *
 *  @param[out] children        Dictionary entries array pointer
 *  @param[in]  entry_header    Header of parent entry
 *  @param[in]  stream          Dictionary entry stream
 **/
void read_dict_entry_children(
    bej_dict_entry** children,
    const bej_dict_entry_header* entry_header,
    FILE* stream
);


/**
 *  Frees dictionary entry array
 *
 *  @param[inout] children     Dictionary entries array pointer
 *  @param[in] child_count     Entries count
 **/
void free_dict_entry_children(
    bej_dict_entry** children,
    const uint16_t child_count
);


#endif // DICT_FILE_H
