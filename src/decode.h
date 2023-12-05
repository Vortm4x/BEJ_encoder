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


/**
 * @brief Unpacks a non-negative integer from a file stream.
 *
 * @param[out]  val         Pointer to store the unpacked non-negative integer value.
 * @param[out]  stream      The file stream to unpack the value from.
 **/
void bej_unpack_nnint(uint64_t* val, FILE* stream);


/**
 * @brief Unpacks a tuple_sfl structure from a file stream.
 *
 * @param[out]  tuple_sfl   Pointer to store the unpacked bej_tuple_sfl structure.
 * @param[out]  stream      The file stream to unpack the structure from.
 **/
void bej_unpack_sfl(bej_tuple_sfl* tuple_sfl, FILE* stream);


/**
 * @brief Unpacks a string from a file stream based on the given length.
 *
 * @param[in]   length      Length of the string to be unpacked.
 * @param[out]  val         Pointer to store the unpacked string.
 * @param[out]  stream      The file stream to unpack the string from.
 */
void bej_unpack_string(const bej_tuple_l length, char* val, FILE* stream);


/**
 * @brief Unpacks a boolean value from a file stream based on the given length.
 *
 * @param[in]   length      Length of the boolean value to be unpacked.
 * @param[out]  val         Pointer to store the unpacked boolean value.
 * @param[out]  stream      The file stream to unpack the boolean value from.
 */
void bej_unpack_boolean(const bej_tuple_l length, bool* val, FILE* stream);


/**
 * @brief Unpacks a real number from a file stream based on the given length.
 *
 * @param[in]   length      Length of the real number to be unpacked.
 * @param[out]  val         Pointer to store the unpacked real number.
 * @param[out]  stream      The file stream to unpack the real number from.
 */
void bej_unpack_real(const bej_tuple_l length, bej_real* val, FILE* stream);


/**
 * @brief Unpacks an integer from a file stream based on the given length.
 *
 * @param[in]   length      Length of the integer to be unpacked.
 * @param[out]  val         Pointer to store the unpacked integer.
 * @param[out]  stream      The file stream to unpack the integer from.
 **/
void bej_unpack_integer(const bej_tuple_l length, int64_t* val, FILE* stream);


/**
 * @brief Retrieves a header for a new dictionary entries subset from dictionary files.
 *
 * @param[in]   schema_dict_file        The schema dictionary file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[in]   tuple_sfl               SFL tuple of decoded value.
 * @param[in]   entries                 Current collection dictionary entries.
 * @param[in]   entries_selector        Dictionary entries sele.
 * @param[out]  entry_header            Pointer to store the retrieved entry header.
 * @return The file stream representing the subset entry header.
 **/
FILE* bej_get_subset_entry_header(
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    const bej_tuple_sfl* tuple_sfl,
    const bej_dict_entry* entries,
    const uint8_t entries_selector,
    bej_dict_entry_header* entry_header
);


/**
 * @brief Decodes an annotation name to JSON file stream.
 *
 * @param[in]   json_file               The JSON file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[out]  sequence                Decoded value sequence.
 **/
void bej_decode_anotation_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    const bej_tuple_s* sequence
);


/**
 * @brief Decodes a property name to JSON streA.
 *
 * @param[out]  json_file               The JSON file stream.
 * @param[in]   annotation_dict_file    The annotation dictionary file stream.
 * @param[in]   tuple_sfl               Decoded value SFL tuple.
 * @param[in]   entries                 Current dictionary entries.
 * @param[in]   entries_count           The number of dictionary entries.
 * @param[in]   entries_selector        Dictionary entries selector.
 **/
void bej_decode_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    const bej_tuple_sfl* tuple_sfl,
    const bej_dict_entry* entries,
    const uint16_t entries_count,
    const uint8_t entries_selector
);


/**
 * @brief Decodes BEJ file to JSON for current dictionary entries.
 *
 * @param[out]  json_file                   The JSON file stream.
 * @param[in]   bej_file                    The BEJ file stream.
 * @param[in]   schema_dict_file            The schema dictionary file stream.
 * @param[in]   annotation_dict_file        The annotation dictionary file stream.
 * @param[in]   entries                     Pointer to the constant dictionary entries.
 * @param[in]   entries_count               The number of dictionary entries.
 * @param[in]   entries_selector            Selector for dictionary entries.
 * @param[in]   prop_count                  The properties count of current subset.
 * @param[in]   add_name                    Defines does current subset props have names.
 * @param[in]   is_seq_array_idx            Defines is sequence of decoded SFL tuple an array index.
 * @return True if decoding is successful, otherwise false.
 */
bool bej_decode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    const bej_dict_entry* entries,
    const uint16_t entries_count,
    const uint8_t entries_selector,
    const uint64_t prop_count,
    const bool add_name,
    const bool is_seq_array_idx
);

/**
 * @brief Decodes BEJ file to JSON.
 *
 * @param[out] json_file            The JSON file stream.
 * @param[in] bej_file              The BEJ file stream.
 * @param[in] schema_dict_file      The schema dictionary file stream.
 * @param[in] annotation_dict_file  The annotation dictionary file stream.
 * @param[in] pdr_map_file          The PDR map file stream.
 * @return True if decoding is successful, otherwise false.
 */

bool bej_decode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
);



#endif // DECODE_H
