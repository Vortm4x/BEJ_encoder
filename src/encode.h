/**
 *
 *  @file
 *  @brief Functions for encoding process
 *  (converting JSON file to BEJ file)
 *
 **/


#ifndef ENCODE_H
#define ENCODE_H

#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "utils.h"


#define BEJ_CHARACTER_SET_BEGIN                   '{'
#define BEJ_CHARACTER_SET_END                     '}'
#define BEJ_CHARACTER_ARRAY_BEGIN                 '['
#define BEJ_CHARACTER_ARRAY_END                   ']'
#define BEJ_CHARACTER_STRING_QUOTE                '\"'
#define BEJ_CHARACTER_PROP_VALUE_SEPARATOR        ':'
#define BEJ_CHARACTER_ELEMENT_SEPARATOR           ','
#define BEJ_CHARACTER_ANOTATION_CHARACTER         '@'
#define BEJ_CHARACTER_REAL_PERIOD                 '.'
#define BEJ_CHARACTER_REAL_EXPONENT               'e'


/************************************************
 *
 *  @param val - unsigned 64bit number, that
 *  be packed
 *
 ***********************************************/
uint64_t bej_nnint_length(const uint64_t val);




uint64_t bej_integer_length(const int64_t val);

void bej_pack_nnint(const uint64_t val, const uint64_t length, FILE* stream);
void bej_pack_integer(const int64_t val, const uint64_t length, FILE* stream);
void bej_pack_tuple_s(const bej_tuple_s* tuple_s, FILE* stream);
void bej_pack_tuple_f(const bej_tuple_f* tuple_f, FILE* stream);
void bej_pack_tuple_l(const bej_tuple_l* tuple_l, FILE* stream);
void bej_pack_sfl(const bej_tuple_sfl* tuple_sfl, FILE* stream);
void bej_move_stream_data(FILE* src_stream, FILE* dest_stream);

char bej_read_delim(FILE *stream);
char* bej_read_prop_name(FILE* stream);
char* bej_read_string(FILE* stream);

bool bej_is_value_null(FILE* stream);
bool bej_collection_is_not_empty(const char end_delim, FILE* stream);
bool bej_is_payload_anotation(char* prop_name);
bool bej_is_payload_anotation(char* prop_name);

uint64_t bej_get_entry_by_name(
    const char* prop_name,
    bej_dict_entry* entries,
    uint16_t entries_count);

bool bej_get_annotation_name(
    const char* prop_name,
    regmatch_t* annotation_prop_match);

bool bej_get_annotation_parts(
    const char* prop_name,
    regmatch_t* schema_prop_match,
    regmatch_t* annotation_prop_match
);

char* bej_get_string_from_prop_match(
    const char* prop_name,
    const regmatch_t* prop_match
);


bool bej_encode_sflv(
    FILE *json_file,
    FILE *bej_file,
    FILE *schema_dict_file,
    FILE *annotation_dict_file,
    bej_dict_entry_header *entry_header,
    bej_tuple_s *tuple_s,
    bej_tuple_f *tuple_f
);

bool bej_encode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector
);

bool bej_encode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
);


#endif // ENCODE_H
