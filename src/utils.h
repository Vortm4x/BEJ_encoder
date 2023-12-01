#ifndef DICT_FILE_H
#define DICT_FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


// BEJ FORMAT definitions
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

#define BEJ_FLAG_DEFERRED                           (1 << 0)
#define BEJ_FLAG_READONLY                           (1 << 1)
#define BEJ_FLAG_NULLABLE                           (1 << 2)
#define BEJ_FLAG_NESTED_TOP_LEVEL_ANNOTATION        (1 << 1)

#define BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA        0b0
#define BEJ_DICTIONARY_SELECTOR_ANNOTATION          0b1

#define BEJ_VERSION_1_0_0                           0xF1F0F000
#define BEJ_VERSION_1_1_0                           0xF1F1F000

#define BEJ_SCHEMA_CLASS_MAJOR                      0x00
#define BEJ_SCHEMA_CLASS_EVENT                      0x01
#define BEJ_SCHEMA_CLASS_ERROR                      0x04


typedef struct bej_tuple_s
{
    uint64_t seq_num        : 63;
    uint8_t dict_selector   :  1;
}
bej_tuple_s;


typedef struct bej_tuple_f
{
    uint8_t bej_type       : 4;
    uint8_t reserved       : 1;
    uint8_t nullable       : 1;
    uint8_t ro_and_tla     : 1;
    uint8_t defer_bind     : 1;
}
bej_tuple_f;


typedef uint64_t bej_tuple_l;


typedef struct bej_tuple_sfl
{
    bej_tuple_s sequence;
    bej_tuple_f format;
    bej_tuple_l length;
}
bej_tuple_sfl;


typedef struct bej_real
{
    int64_t whole;
    uint64_t lead_zeros;
    uint64_t fract;
    int64_t exponent;
}
bej_real;


typedef struct __attribute__((packed)) bej_file_header
{
    uint32_t version;
    uint16_t flags;
    uint8_t schema_class;
}
bej_file_header;


typedef struct __attribute__((packed)) bej_dict_entry_header
{
    bej_tuple_f format;
    uint16_t sequence;
    uint16_t offset;
    uint16_t child_count;
    uint8_t name_size;
    uint16_t name_ofset;
}
bej_dict_entry_header;


typedef struct bej_dict_entry
{
    bej_dict_entry_header* header;
    char* name;
}
bej_dict_entry;


typedef struct __attribute__((packed)) bej_dict_header
{
    uint8_t version;
    uint8_t flags;
    uint16_t entry_count;
    uint32_t schema_version;
    uint32_t dict_size;
}
bej_dict_header;

off_t get_stream_size(FILE* stream);

void get_dict_entry_by_sequence(
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector
);

FILE* get_collection_entry_header(
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint8_t entries_selector,
    bej_dict_entry_header* entry_header
);

void read_dict_entry(bej_dict_entry* entry, FILE* stream);
void free_dict_entry(bej_dict_entry* entry);

void read_dict_entry_children(bej_dict_entry** children, bej_dict_entry_header* entry_header, FILE* stream);
void free_dict_entry_children(bej_dict_entry** children, uint16_t child_count);


#endif // DICT_FILE_H
