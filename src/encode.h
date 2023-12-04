#ifndef ENCODE_H
#define ENCODE_H

#include <string.h>
#include <ctype.h>
#include "utils.h"


#define BEJ_SET_BEGIN '{'
#define BEJ_SET_END '}'
#define BEJ_ARRAY_BEGIN '['
#define BEJ_ARRAY_END ']'
#define BEJ_STRING_QUOTE '\"'
#define BEJ_PROP_VALUE_SEPARATOR ':'
#define BEJ_ELEMENT_SEPARATOR ','



//uint64_t bej_pack_nnint(const uint64_t val, FILE* stream);
//uint64_t bej_pack_sfl(bej_tuple_sfl* tuple_sfl, FILE* stream);

//void bej_move_stream_data(FILE* src_stream, FILE* dest_stream);
//char bej_read_delim(FILE* stream);


//bool bej_encode_stream(
//    FILE* json_file,
//    FILE* bej_file,
//    FILE* schema_dict_file,
//    FILE* annotation_dict_file,
//    bej_dict_entry* entries,
//    uint16_t entries_count,
//    uint8_t entries_selector
//);

bool bej_encode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
);


#endif // ENCODE_H
