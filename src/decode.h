/**
 *
 *  @file
 *  @brief Functions for decocing process
 *  (converting BEJ file to JSON file)
 *
 **/


#ifndef DECODE_H
#define DECODE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "utils.h"


void bej_unpack_nnint(uint64_t* val, FILE* stream);
void bej_unpack_sfl(bej_tuple_sfl* tuple_sfl, FILE* stream);

void bej_unpack_string(const bej_tuple_l length, char* val, FILE* stream);
void bej_unpack_boolean(const bej_tuple_l length, bool* val, FILE* stream);
void bej_unpack_real(const bej_tuple_l length, bej_real* val, FILE* stream);
void bej_unpack_integer(const bej_tuple_l length, int64_t* val, FILE* stream);

FILE* bej_get_subset_entry_header(
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint8_t entries_selector,
    bej_dict_entry_header* entry_header
);

void bej_decode_enum(
    FILE* json_file,
    FILE* dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entry,
    uint64_t value
);

void bej_decode_anotation_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    bej_tuple_s* sequence
);

void bej_decode_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector
);

bool bej_decode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector,
    uint64_t prop_count,
    bool add_name,
    bool is_seq_array_idx
);

bool bej_decode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
);





#endif // DECODE_H
