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


/**
 * @def BEJ_CHARACTER_SET_BEGIN
 * @brief Beginning character for a JSON set.
 *
 * @def BEJ_CHARACTER_SET_END
 * @brief Ending character for a JSON set.
 *
 * @def BEJ_CHARACTER_ARRAY_BEGIN
 * @brief Beginning character for a JSON array.
 *
 * @def BEJ_CHARACTER_ARRAY_END
 * @brief Ending character for a JSON array.
 *
 * @def BEJ_CHARACTER_STRING_QUOTE
 * @brief JSON string quote.
 *
 * @def BEJ_CHARACTER_PROP_VALUE_SEPARATOR
 * @brief Separator character between property and value in JSON.
 *
 * @def BEJ_CHARACTER_ELEMENT_SEPARATOR
 * @brief Element separator character in JSON collections (array or set).
 *
 * @def BEJ_CHARACTER_ANOTATION_CHARACTER
 * @brief Character representing an annotation in JSON.
 *
 * @def BEJ_CHARACTER_REAL_PERIOD
 * @brief Period character used in real numbers in JSON.
 *
 * @def BEJ_CHARACTER_REAL_EXPONENT
 * @brief Exponent character used in real numbers in JSON.
 */
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


/**
 * @brief Calculates the length of a non-negative integer value.
 *
 * @param[in]   val     The non-negative integer value to calculate the length of.
 * @returns The length of the non-negative integer value.
 **/
uint64_t bej_nnint_length(const uint64_t val);


/**
 * @brief Calculates the length of an integer value.
 *
 * @param[in]   val     The integer value to calculate the length of.
 * @return The length of the integer value.
 **/
uint64_t bej_integer_length(const int64_t val);


/**
 * @brief Packs a non-negative integer value into a stream.
 *
 * @param[in]   val         The non-negative integer value to pack.
 * @param[in]   length      The length of the value to be packed.
 * @param[out]  stream      The file stream to pack the value into.
 **/
void bej_pack_nnint(const uint64_t val, const uint64_t length, FILE* stream);


/**
 * @brief Packs an integer value into a stream.
 *
 * @param[in]   val         The integer value to pack.
 * @param[in]   length      The length of the value to be packed.
 * @param[out]  stream      The file stream to pack the value into.
 **/
void bej_pack_integer(const int64_t val, const uint64_t length, FILE* stream);


/**
 * @brief Packs a tuple_s structure into a stream.
 *
 * @param[in]   tuple_s     Pointer to the bej_tuple_s structure to be packed.
 * @param[out]  stream      The file stream to pack the structure into.
 **/
void bej_pack_tuple_s(const bej_tuple_s* tuple_s, FILE* stream);


/**
 * @brief Packs a tuple_f structure into a stream.
 *
 * @param[in]   tuple_f     Pointer to the bej_tuple_f structure to be packed.
 * @param[out]  stream      The file stream to pack the structure into.
 **/
void bej_pack_tuple_f(const bej_tuple_f* tuple_f, FILE* stream);


/**
 * @brief Packs a tuple_l structure into a stream.
 *
 * @param[in]   tuple_l     Pointer to the bej_tuple_l structure to be packed.
 * @param[out]  stream      The file stream to pack the structure into.
 **/
void bej_pack_tuple_l(const bej_tuple_l* tuple_l, FILE* stream);


/**
 * @brief Packs a tuple_sfl structure into a stream.
 *
 * @param[in]   tuple_sfl   Pointer to the bej_tuple_sfl structure to be packed.
 * @param[out]  stream      The file stream to pack the structure into.
 **/
void bej_pack_sfl(const bej_tuple_sfl* tuple_sfl, FILE* stream);


/**
 * @brief Moves data from one file stream to another.
 *
 * @param[in]   src_stream      The source file stream.
 * @param[out]  dest_stream     The destination file stream.
 **/
void bej_move_stream_data(FILE* src_stream, FILE* dest_stream);


/**
 * @brief Reads a delimiter character from a file stream.
 *
 * @param[out]  stream      The file stream to read the delimiter from.
 * @return The read delimiter character.
 **/
char bej_read_delim(FILE *stream);


/**
 * @brief Reads a property name from a file stream.
 *
 * @param[out]  stream      The file stream to read the property name from.
 * @return The read property name as a string.
 * @warning Return value should be deallocated by free() function
 **/
char* bej_read_prop_name(FILE* stream);


/**
 * @brief Reads a string from a file stream.
 *
 * @param[out]  stream      The file stream to read the string from.
 * @return The read string.
 * @warning Return value should be deallocated by free() function
 **/
char* bej_read_string(FILE* stream);


/**
 * @brief Checks if the JSON value in the file stream is null.
 *
 * @param[out]  stream  The file stream to check for a null value.
 * @return True if the value is null, otherwise false.
 **/
bool bej_is_value_null(FILE* stream);


/**
 * @brief Checks if the JSON collection (set or array) in the file stream is not empty.
 *
 * @param[in]   end_delim   The ending delimiter character of the collection.
 * @param[out]  stream      The file stream to check for a non-empty collection.
 * @return True if the collection is not empty, otherwise false.
 **/
bool bej_collection_is_not_empty(const char end_delim, FILE* stream);


/**
 * @brief Checks if the payload is an annotation based on the property name.
 *
 * @param[in]   prop_name   The property name to check for annotation.
 * @return True if the property is an annotation, otherwise false.
 **/
bool bej_is_payload_anotation(const char* prop_name);


/**
 * @brief Gets an entry by name from the dictionary entries.
 *
 * @param[in]   prop_name The name of the entry to retrieve.
 * @param[in]   entries Pointer to the dictionary entries.
 * @param[in]   entries_count The number of entries in the dictionary.
 * @return The sequence value corresponding to the given name if
 * entry exists, otherwise BEJ_INAVALID_SEQUENCE_NUMBER.
 **/
uint64_t bej_get_entry_by_name(
    const char* prop_name,
    const bej_dict_entry* entries,
    const uint16_t entries_count
);


/**
 * @brief Gets the annotation property name from a given property name.
 *
 * @param[in]   prop_name               The property name containing the annotation.
 * @param[out]  annotation_prop_match   Pointer to the regmatch_t structure to store the annotation property name.
 * @return True if the annotation property name is found, otherwise false.
 **/
bool bej_get_annotation_name(
    const char* prop_name,
    regmatch_t* annotation_prop_match);


/**
 * @brief Gets the parts of an annotation from a given property name.
 *
 * @param[in] prop_name The property name containing the annotation.
 * @param[out] schema_prop_match Pointer to the regmatch_t structure to store the schema part of the annotation.
 * @param[out] annotation_prop_match Pointer to the regmatch_t structure to store the annotation part of the annotation.
 * @return True if the annotation parts are found, otherwise false.
 **/
bool bej_get_annotation_parts(
    const char* prop_name,
    regmatch_t* schema_prop_match,
    regmatch_t* annotation_prop_match
);


/**
 * @brief Gets a string from a property match using indices from a property name.
 *
 * @param[in]   prop_name       The property name.
 * @param[in]   prop_match      Pointer to the regmatch_t structure containing match indices.
 * @return The extracted string based on the indices from the property match.
 * @warning Return value should be deallocated by free() function
 **/
char* bej_get_string_from_prop_match(
    const char* prop_name,
    const regmatch_t* prop_match
);


/**
 * @brief Encodes property value and sfl tuple to the BEJ file
 *
 * @param[in]   json_file               The JSON file stream containing data to encode.
 * @param[out]  bej_file                The BEJ file stream to encode the data into.
 * @param[in]   schema_dict_file        The schema dictionary file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[in]   entry_header            Encoded value entry header.
 * @param[in]   tuple_s                 Encoded value sequence.
 * @param[in]   tuple_f                 Encoded value format.
 * @return True if encoding is successful, otherwise false.
 */
bool bej_encode_sflv(
    FILE *json_file,
    FILE *bej_file,
    FILE *schema_dict_file,
    FILE *annotation_dict_file,
    const bej_dict_entry_header *entry_header,
    const bej_tuple_s *tuple_s,
    const bej_tuple_f *tuple_f
);


/**
 * @brief Reads property name and prepares value info before encoding
 *
 * @param[in]   json_file               The JSON file stream containing data to encode.
 * @param[out]  bej_file                The BEJ file stream to encode the data into.
 * @param[in]   schema_dict_file        The schema dictionary file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[in]   entries                 Entries related to current collection values.
 * @param[in]   entries_count           The number of passed entries.
 * @param[in]   entries_selector        Current dictionary selector
 * @return True if encoding is successful, otherwise false.
 **/
bool bej_encode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    const bej_dict_entry* entries,
    const uint16_t entries_count,
    const uint8_t entries_selector
);


/**
 * @brief Encodes JSON data into a BEJ file.
 *
 * @param[in]   json_file               JSON file stream containing data to encode.
 * @param[out]  bej_file                BEJ file stream to encode the data into.
 * @param[in]   schema_dict_file        The schema dictionary file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[in]   pdr_map_file            The PDR map file stream.
 * @return True if encoding is successful, otherwise false.
 **/
bool bej_encode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
);


#endif // ENCODE_H
