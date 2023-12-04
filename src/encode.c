#include "encode.h"

uint64_t bej_nnint_length(const uint64_t val)
{
    uint64_t length = sizeof(uint64_t);

    while((uint8_t)(val >> 8 * (length - 1)) == 0x00 && length > 1)
    {
        --length;
    }

    return length + 1;
}

uint64_t bej_integer_length(const int64_t val)
{
    uint64_t length = (uint64_t)sizeof(int64_t);
    uint8_t pad_byte = (val < 0) ? 0xFF : 0x00;

    while((uint8_t)(val >> 8 * (length - 1)) == pad_byte && length > 1)
    {
        --length;
    }

    return length;
}

void bej_pack_nnint(const uint64_t val, const uint64_t length, FILE* stream)
{
    uint8_t size = (uint8_t)length - 1;
    fwrite(&size, sizeof(uint8_t), 1, stream);

    uint8_t data = 0;
    for(uint8_t i = 0; i < size; ++i)
    {
        data = (uint8_t)(val >> (8 * i));
        fwrite(&data, sizeof(uint8_t), 1, stream);
    }
}

void bej_pack_integer(const int64_t val, const uint64_t length, FILE* stream)
{
    uint8_t data = 0;
    for(uint8_t i = 0; i < length; ++i)
    {
        data = (uint8_t)(val >> (8 * i));
        fwrite(&data, sizeof(uint8_t), 1, stream);
    }
}

void bej_pack_tuple_s(const bej_tuple_s* tuple_s, FILE* stream)
{
    uint64_t tuple_s_num = *((uint64_t*)tuple_s);
    bej_pack_nnint(tuple_s_num, bej_nnint_length(tuple_s_num), stream);
}

void bej_pack_tuple_f(const bej_tuple_f* tuple_f, FILE* stream)
{
    uint8_t tuple_f_num = *((uint8_t*)tuple_f);
    fwrite(&tuple_f_num, sizeof(bej_tuple_f), 1, stream);
}

void bej_pack_tuple_l(const bej_tuple_l* tuple_l, FILE* stream)
{
    uint64_t tuple_l_num = *((uint64_t*)tuple_l);
    bej_pack_nnint(tuple_l_num, bej_nnint_length(tuple_l_num), stream);
}

void bej_pack_sfl(const bej_tuple_sfl* tuple_sfl, FILE* stream)
{
    bej_pack_tuple_s(&tuple_sfl->sequence, stream);
    bej_pack_tuple_f(&tuple_sfl->format, stream);
    bej_pack_tuple_l(&tuple_sfl->length, stream);
}

void bej_move_stream_data(FILE* src_stream, FILE* dest_stream)
{
    off_t mem_size = get_stream_size(src_stream);
    void* mem_data = malloc(mem_size);

    fseeko(src_stream, 0, SEEK_SET);
    fread(mem_data, mem_size, 1, src_stream);

    fwrite(mem_data, mem_size, 1, dest_stream);
    free(mem_data);
}

char bej_read_delim(FILE* stream)
{
    char next_char = 0;

    do
    {
        fscanf(stream, "%c", &next_char);
    }
    while(isspace(next_char));

    return next_char;
}

char* bej_read_prop_name(FILE* stream)
{
    char* str_buffer = bej_read_string(stream);

    if(bej_read_delim(stream) != BEJ_PROP_VALUE_SEPARATOR)
    {
        free(str_buffer);
        return NULL;
    }

    return str_buffer;
}

char* bej_read_string(FILE* stream)
{
    char* str_buffer = NULL;

    if(bej_read_delim(stream) != BEJ_STRING_QUOTE)
    {
        return NULL;
    }

    fscanf(stream, "%m[^\"]", &str_buffer);

    if(bej_read_delim(stream) != BEJ_STRING_QUOTE)
    {
        free(str_buffer);
        return NULL;
    }

    return str_buffer;
}

bool bej_is_value_null(FILE* stream)
{
    char* str_buffer = NULL;
    off_t curr_pos = ftello(stream);

    fscanf(stream, " ");
    int fields = fscanf(stream, "%m[null]", &str_buffer);

    if(fields != 1 || str_buffer == NULL)
    {
        free(str_buffer);
        fseeko(stream, curr_pos, SEEK_SET);
        return false;
    }

    if(strcmp(str_buffer, "null") != 0)
    {
        free(str_buffer);
        fseeko(stream, curr_pos, SEEK_SET);
        return false;
    }

    return true;
}

bool bej_collection_is_not_empty(const char end_delim, FILE* stream)
{
    off_t curr_pos = ftello(stream);

    if(bej_read_delim(stream) == end_delim)
    {
        return false;
    }

    fseeko(stream, curr_pos, SEEK_SET);
    return true;
}

bool bej_is_payload_anotation(char* prop_name)
{
    if(prop_name == NULL)
    {
        return false;
    }

    if(strchr(prop_name, BEJ_ANOTATION_CHARACTER) == NULL)
    {
        return false;
    }

    return true;
}

uint64_t bej_get_entry_by_name(
    const char* prop_name,
    bej_dict_entry* entries,
    uint16_t entries_count
)
{
    if(prop_name == NULL)
    {
        return BEJ_INAVALID_SEQUENCE_NUMBER;
    }

    uint64_t seq_num = BEJ_INAVALID_SEQUENCE_NUMBER;

    for (uint16_t i = 0; i < entries_count; ++i)
    {
        if(strcmp(entries[i].name, prop_name) == 0)
        {
            seq_num = i;
            break;
        }
    }

    return seq_num;
}

bool bej_get_annotation_name(
    const char* prop_name,
    regmatch_t* annotation_prop_match
)
{
    regex_t anot_parts_regex;
    const char* anot_parts_pattern = ".*@.*\\.(.*)";

    if(regcomp(&anot_parts_regex, anot_parts_pattern, 0) != REG_NOERROR)
    {
        return false;
    }

    regmatch_t mathches[2];
    if(regexec(&anot_parts_regex, prop_name, 2, mathches, 0) != REG_NOERROR)
    {
        regfree(&anot_parts_regex);
        return false;
    }

    *annotation_prop_match = mathches[1];

    regfree(&anot_parts_regex);
    return true;
}

bool bej_get_annotation_parts(
    const char* prop_name,
    regmatch_t* schema_prop_match,
    regmatch_t* annotation_prop_match
)
{
    regex_t anot_parts_regex;
    const char* anot_parts_pattern = "(.*)(@.*\\..*)";

    if(regcomp(&anot_parts_regex, anot_parts_pattern, 0) != REG_NOERROR)
    {
        return false;
    }

    regmatch_t mathches[3];
    if(regexec(&anot_parts_regex, prop_name, 3, mathches, 0) != REG_NOERROR)
    {
        regfree(&anot_parts_regex);
        return false;
    }

    *schema_prop_match = mathches[1];
    *annotation_prop_match = mathches[2];

    regfree(&anot_parts_regex);
    return true;
}

char* bej_get_string_from_prop_match(
    const char* prop_name,
    const regmatch_t* prop_match
)
{
    regoff_t len = prop_match->rm_eo - prop_match->rm_so;

    char* prop_substr = calloc(len + 1, sizeof(char));
    strncpy(prop_substr, prop_name + prop_match->rm_so, len);

    return prop_substr;
}

bool bej_encode_sflv(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_dict_entry_header* entry_header,
    bej_tuple_s* tuple_s,
    bej_tuple_f* tuple_f
)
{
    bej_tuple_sfl tuple_sfl;
    tuple_sfl.sequence = *tuple_s;
    tuple_sfl.format = *tuple_f;
    tuple_sfl.length = 0;

    if(tuple_sfl.format.nullable)
    {
        if(bej_is_value_null(json_file))
        {
            tuple_sfl.format.bej_type = BEJ_FORMAT_NULL;
        }
    }

    switch (tuple_sfl.format.bej_type)
    {
        case BEJ_FORMAT_SET:
        {
            char collection_sep = bej_read_delim(json_file);

            if(collection_sep != BEJ_SET_BEGIN)
            {
                return false;
            }

            FILE* nested_stream = tmpfile();
            bej_tuple_l count = 0;

            if(bej_collection_is_not_empty(BEJ_SET_END, json_file))
            {
                bej_dict_entry* child_entries = NULL;
                FILE* current_dict;

                if(tuple_s->dict_selector == BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA)
                {
                    current_dict = schema_dict_file;
                }
                else
                {
                    current_dict = annotation_dict_file;
                }

                fseeko(current_dict, entry_header->offset, SEEK_SET);
                read_dict_entry_children(&child_entries, entry_header, current_dict);

                bool success = true;

                do
                {
                    success = bej_encode_stream(
                        json_file, nested_stream, schema_dict_file, annotation_dict_file,
                        child_entries, entry_header->child_count, tuple_s->dict_selector
                    );

                    ++count;
                    collection_sep = bej_read_delim(json_file);
                }
                while(collection_sep == BEJ_ELEMENT_SEPARATOR && success);

                if(collection_sep != BEJ_SET_END || !success)
                {
                    fclose(nested_stream);
                    free(child_entries);
                    return false;
                }

                free(child_entries);
            }
            uint64_t count_len = bej_nnint_length(count);
            tuple_sfl.length = count_len + ftello(nested_stream);

            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_pack_nnint(count, count_len, bej_file);
            bej_move_stream_data(nested_stream, bej_file);

            fclose(nested_stream);

            break;
        }
        case BEJ_FORMAT_STRING:
        {
            char* val = bej_read_string(json_file);

            if(val == NULL)
            {
                return false;
            }

            tuple_sfl.length = strlen(val) + 1;

            bej_pack_sfl(&tuple_sfl, bej_file);
            fwrite(val, sizeof(char), tuple_sfl.length, bej_file);

            free(val);

            break;
        }
        case BEJ_FORMAT_INTEGER:
        {
            int64_t val = 0;
            int fields = fscanf(json_file, "%ld", &val);

            if(fields != 1)
            {
                return false;
            }

            tuple_sfl.length = bej_integer_length(val);

            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_pack_integer(val, tuple_sfl.length, bej_file);

            break;
        }
        case BEJ_FORMAT_REAL:
        {
            bej_real val;
            memset(&val, 0, sizeof(val));

            if(fscanf(json_file, "%ld", &val.whole) != 1)
            {
                return false;
            }

            if(fgetc(json_file) != BEJ_REAL_PERIOD)
            {
                return false;
            }

            char* lead_zeros_str = NULL;
            if(fscanf(json_file, "%m[0]", &lead_zeros_str) < 0)
            {
                return false;
            }

            if(lead_zeros_str != NULL)
            {
                val.lead_zeros = strlen(lead_zeros_str);
                free(lead_zeros_str);
            }

            if(fscanf(json_file, "%ld", &val.fract) < 0)
            {
                return false;
            }

            if(val.lead_zeros > 0 && val.fract == 0)
            {
                val.lead_zeros = 0;
            }

            off_t curr_pos = ftello(json_file);

            if(tolower(fgetc(json_file)) == BEJ_REAL_EXPONENT)
            {
                if(fscanf(json_file, "%ld", &val.exponent) != 1)
                {
                    return false;
                }
            }
            else
            {
                fseeko(json_file, curr_pos, SEEK_SET);
            }

            uint64_t whole_len = bej_integer_length(val.whole);
            uint64_t exp_len = bej_integer_length(val.exponent);

            uint64_t whole_len_size = bej_nnint_length(whole_len);
            uint64_t lead_zeros_size = bej_nnint_length(val.lead_zeros);
            uint64_t fract_size = bej_nnint_length(val.fract);
            uint64_t exp_len_size = bej_nnint_length(val.fract);

            tuple_sfl.length += whole_len_size;
            tuple_sfl.length += whole_len;
            tuple_sfl.length += fract_size;
            tuple_sfl.length += lead_zeros_size;
            tuple_sfl.length += exp_len_size;
            tuple_sfl.length += exp_len;

            bej_pack_sfl(&tuple_sfl, bej_file);

            bej_pack_nnint(whole_len, whole_len_size, bej_file);
            bej_pack_integer(val.whole, whole_len, bej_file);
            bej_pack_nnint(val.lead_zeros, lead_zeros_size, bej_file);
            bej_pack_nnint(val.fract, fract_size, bej_file);
            bej_pack_nnint(exp_len, exp_len_size, bej_file);
            bej_pack_integer(val.exponent, exp_len, bej_file);

            break;
        }
        case BEJ_FORMAT_BOOLEAN:
        {
            char* val = NULL;

            fscanf(json_file, " ");
            int fields = fscanf(json_file, "%m[truefalse]", &val);
            uint8_t encoded_val = 0x00;

            if(fields != 1)
            {
                free(val);
                return false;
            }

            if(val == NULL)
            {
                return false;
            }
            else if(strcmp(val, "true") == 0)
            {
                encoded_val = 0x01;
            }
            else if(strcmp(val, "false") == 0)
            {
                encoded_val = 0x00;
            }
            else
            {
                free(val);
                return false;
            }

            free(val);

            tuple_sfl.length = 1;
            bej_pack_sfl(&tuple_sfl, bej_file);
            fwrite(&encoded_val, sizeof(uint8_t), 1, bej_file);

            break;
        }
        case BEJ_FORMAT_ENUM:
        {
            char* val = bej_read_string(json_file);

            if(val == NULL)
            {
                return false;
            }

            FILE* current_dict;

            if(tuple_sfl.sequence.dict_selector == BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA)
            {
                current_dict = schema_dict_file;
            }
            else
            {
                current_dict = annotation_dict_file;
            }

            bej_dict_entry* child_entries;
            fseeko(current_dict, entry_header->offset, SEEK_SET);
            read_dict_entry_children(&child_entries, entry_header, current_dict);

            uint64_t enum_val = bej_get_entry_by_name(val, child_entries, entry_header->child_count);
            free_dict_entry_children(&child_entries, entry_header->child_count);
            free(val);

            if(enum_val == BEJ_INAVALID_SEQUENCE_NUMBER)
            {
                return false;
            }

            tuple_sfl.length = bej_nnint_length(enum_val);

            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_pack_nnint(enum_val, tuple_sfl.length, bej_file);

            break;
        }
        case BEJ_FORMAT_NULL:
        {
            bej_pack_sfl(&tuple_sfl, bej_file);

            break;
        }
        case BEJ_FORMAT_ARRAY:
        {
            char collection_sep = bej_read_delim(json_file);

            if(collection_sep != BEJ_ARRAY_BEGIN)
            {
                return false;
            }

            FILE* nested_stream = tmpfile();
            bej_tuple_l count = 0;

            if(bej_collection_is_not_empty(BEJ_ARRAY_END, json_file))
            {
                bej_dict_entry_header child_entry_header;
                FILE* current_dict;
                bej_tuple_s sequence;
                bej_tuple_f format;

                if(tuple_s->dict_selector == BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA)
                {
                    current_dict = schema_dict_file;
                }
                else
                {
                    current_dict = annotation_dict_file;
                }

                fseeko(current_dict, entry_header->offset, SEEK_SET);
                fread(&child_entry_header, sizeof(bej_dict_entry_header), 1, current_dict);

                memset(&sequence, 0, sizeof(bej_tuple_s));
                memset(&format, 0, sizeof(bej_tuple_f));

                sequence.dict_selector = tuple_s->dict_selector;
                format.bej_type = child_entry_header.format.bej_type;

                bool success = true;

                do
                {
                    success = bej_encode_sflv(
                        json_file, nested_stream, schema_dict_file, annotation_dict_file,
                        &child_entry_header, &sequence, &format
                    );

                    ++count;
                    ++sequence.seq_num;
                    collection_sep = bej_read_delim(json_file);
                }
                while(collection_sep == BEJ_ELEMENT_SEPARATOR && success);

                if(collection_sep != BEJ_ARRAY_END || !success)
                {
                    fclose(nested_stream);
                    return false;
                }
            }

            uint64_t count_len = bej_nnint_length(count);
            tuple_sfl.length = count_len + ftello(nested_stream);

            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_pack_nnint(count, count_len, bej_file);
            bej_move_stream_data(nested_stream, bej_file);

            fclose(nested_stream);

            break;
        }
    }

    return true;
}

bool bej_encode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector
)
{
    char* prop_name = bej_read_prop_name(json_file);
    uint64_t seq_num = bej_get_entry_by_name(prop_name, entries, entries_count);
    bool is_payload_annotation = bej_is_payload_anotation(entries[seq_num].name);

    if(seq_num == BEJ_INAVALID_SEQUENCE_NUMBER && !is_payload_annotation)
    {
        free(prop_name);
        return false;
    }

    bej_dict_entry_header entry_header;
    bej_tuple_s sequence;
    bej_tuple_f format;

    memset(&sequence, 0, sizeof(bej_tuple_s));
    memset(&format, 0, sizeof(bej_tuple_f));

    if(is_payload_annotation)
    {
        regmatch_t schema_match, annotation_match;
        if(!bej_get_annotation_parts(prop_name, &schema_match, &annotation_match))
        {
            free(prop_name);
            return false;
        }

        bej_dict_entry_header annotation_entry_header;
        fseeko(annotation_dict_file, sizeof(bej_dict_header), SEEK_SET);
        fread(&annotation_entry_header, sizeof(bej_dict_entry_header), 1, annotation_dict_file);

        bej_dict_entry* annotation_entries;
        fseeko(annotation_dict_file, annotation_entry_header.offset, SEEK_SET);
        read_dict_entry_children(&annotation_entries, &annotation_entry_header, annotation_dict_file);

        char* annotation_prop = bej_get_string_from_prop_match(prop_name, &annotation_match);
        uint64_t annot_seq = bej_get_entry_by_name(annotation_prop, annotation_entries, annotation_entry_header.child_count);
        if(annot_seq == BEJ_INAVALID_SEQUENCE_NUMBER)
        {
            free(prop_name);
            free(annotation_prop);
            free_dict_entry_children(&annotation_entries, annotation_entry_header.child_count);
            return false;
        }

        entry_header = *annotation_entries[annot_seq].header;
        sequence.dict_selector = BEJ_DICTIONARY_SELECTOR_ANNOTATION;

        if(entries_selector == BEJ_DICTIONARY_SELECTOR_ANNOTATION)
        {
            format.ro_and_tla = 0b1;
        }

        char* schema_prop = bej_get_string_from_prop_match(prop_name, &schema_match);
        if(strlen(schema_prop) > 0)
        {
            format.bej_type = BEJ_FORMAT_PROPERTY_ANNOTATION;
        }
        else
        {
            format.bej_type = entry_header.format.bej_type;
        }

        free(schema_prop);
        free(annotation_prop);
        free_dict_entry_children(&entries, entry_header.child_count);
    }
    else
    {
        entry_header = *entries[seq_num].header;
        sequence.dict_selector = entries_selector;
        format.bej_type = entry_header.format.bej_type;
    }
    sequence.seq_num = entry_header.sequence;
    format.nullable = entry_header.format.nullable;
    format.reserved = entry_header.format.reserved;


    if(format.bej_type == BEJ_FORMAT_PROPERTY_ANNOTATION)
    {
        regmatch_t schema_match, annotation_match;
        if(!bej_get_annotation_parts(prop_name, &schema_match, &annotation_match))
        {
            free(prop_name);
            return false;
        }

        char* schema_prop = bej_get_string_from_prop_match(prop_name, &schema_match);
        uint64_t prop_seq = bej_get_entry_by_name(schema_prop, entries, entries_count);

        free(schema_prop);
        free(prop_name);

        if(prop_seq == BEJ_INAVALID_SEQUENCE_NUMBER)
        {
            return false;
        }

        format.bej_type = entry_header.format.bej_type;

        FILE* nested_stream = tmpfile();

        bool success = bej_encode_sflv(
            json_file, nested_stream, schema_dict_file, annotation_dict_file,
            &entry_header, &sequence, &format
        );

        if(!success)
        {
            fclose(nested_stream);
            return false;
        }

        bej_tuple_sfl tuple_sfl;
        memset(&tuple_sfl, 0, sizeof(bej_tuple_sfl));

        tuple_sfl.sequence.seq_num = prop_seq;
        tuple_sfl.sequence.dict_selector = BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA;
        tuple_sfl.format.bej_type = BEJ_FORMAT_PROPERTY_ANNOTATION;
        tuple_sfl.length = ftello(nested_stream);

        bej_pack_sfl(&tuple_sfl, bej_file);
        bej_move_stream_data(nested_stream, bej_file);

        fclose(nested_stream);
    }
    else
    {
        bool success = bej_encode_sflv(
            json_file, bej_file, schema_dict_file, annotation_dict_file,
            &entry_header, &sequence, &format
        );

        return success;
    }

    return true;
}

bool bej_encode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
)
{
    fseeko(json_file, 0, SEEK_SET);

    bej_file_header bej_header;
    bej_header.version = BEJ_VERSION_1_1_0;
    bej_header.flags = 0;
    bej_header.schema_class = BEJ_SCHEMA_CLASS_MAJOR;

    fwrite(&bej_header, sizeof(bej_file_header), 1, bej_file);

    bej_dict_entry_header entry_header;
    fseeko(schema_dict_file, sizeof(bej_dict_header), SEEK_SET);
    fread(&entry_header, sizeof(entry_header), 1, schema_dict_file);

    bej_tuple_s sequence;
    bej_tuple_f format;
    memset(&sequence, 0, sizeof(bej_tuple_s));
    memset(&format, 0, sizeof(bej_tuple_f));

    sequence.dict_selector = BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA;
    format.bej_type = BEJ_FORMAT_SET;

    bool success = bej_encode_sflv(
        json_file, bej_file, schema_dict_file, annotation_dict_file,
        &entry_header, &sequence, &format
    );

    return success;
}
