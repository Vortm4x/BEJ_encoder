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
        data = (val >> (size - 1 + i));
        fwrite(&data, sizeof(uint8_t), 1, stream);
    }
}

void bej_pack_integer(const int64_t val, const uint64_t length, FILE* stream)
{
    uint8_t data = 0;
    for(uint8_t i = 0; i < length; ++i)
    {
        data = (val >> 8 * (length - 1 + i));
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
    fwrite(tuple_f, sizeof(bej_tuple_f), 1, stream);
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
    off_t mem_size = ftello(src_stream);
    void* mem_data = malloc(mem_size);

    fread(mem_data, mem_size, 1, src_stream);
    fwrite(mem_data, mem_size, 1, dest_stream);
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

    if(bej_read_delim(stream) != BEJ_PROP_VALUE_SEPARATOR)
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
    int fields = fscanf(stream, "%ms", &str_buffer);

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

bool bej_collection_not_empty(const char end_delim, FILE* stream)
{
    off_t curr_pos = ftello(stream);

    if(bej_read_delim(stream) == end_delim)
    {
        return false;
    }

    fseeko(stream, curr_pos, SEEK_SET);
    return true;
}

uint64_t bej_get_entry_by_name(
    char* prop_name,
    bej_dict_entry* entries,
    uint16_t entries_count,
    FILE* stream
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



bool bej_encode_stream(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector,
    bool named_entry,
    bool is_seq_array_idx
)
{
    uint64_t seq_num = 0;

    if(named_entry)
    {
        char* prop_name = bej_read_prop_name(json_file);

        seq_num = bej_get_entry_by_name(prop_name, entries, entries_count, json_file);
        free(prop_name);

        if(seq_num == BEJ_INAVALID_SEQUENCE_NUMBER)
        {
            return false;
        }
    }


    bej_tuple_sfl tuple_sfl;

    tuple_sfl.format = entries[seq_num].header->format;
    tuple_sfl.sequence.seq_num = seq_num;
    tuple_sfl.sequence.dict_selector = entries_selector;


    if(tuple_sfl.format.nullable)
    {
        if(bej_is_value_null(json_file))
        {
            tuple_sfl.format.bej_type = BEJ_FORMAT_NULL;
        }
    }



    switch (entries[seq_num].header->format.bej_type)
    {
        case BEJ_FORMAT_SET:
        {
            char collection_sep = bej_read_delim(json_file);

            if(collection_sep != BEJ_SET_BEGIN)
            {
                return false;
            }

            FILE* collection_stream = tmpfile();
            bej_tuple_l count = 0;

            if(bej_collection_not_empty(BEJ_SET_END, json_file))
            {
                bej_dict_entry* child_entries = NULL;
                read_dict_entry_children(&child_entries,  entries[seq_num].header, schema_dict_file);

                bool success = true;

                do
                {
                    success = bej_encode_stream(
                        json_file, collection_stream, schema_dict_file, annotation_dict_file,
                        child_entries, entries[seq_num].header->child_count, entries_selector,
                        true, false
                    );

                    ++count;
                    collection_sep = bej_read_delim(json_file);
                }
                while(collection_sep == BEJ_ELEMENT_SEPARATOR && success);

                if(collection_sep != BEJ_SET_END || !success)
                {
                    fclose(collection_stream);
                    free(child_entries);
                    return false;
                }

                free(child_entries);
            }

            bej_pack_nnint(count, bej_nnint_length(count) ,collection_stream);
            tuple_sfl.length = ftello(collection_stream);
            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_move_stream_data(collection_stream, bej_file);

            fclose(collection_stream);

            break;
        }
        case BEJ_FORMAT_STRING:
        {


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


            break;
        }
        case BEJ_FORMAT_BOOLEAN:
        {
            char* val = NULL;
            int fields = fscanf(json_file, "%m[a-z]", &val);
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
        case BEJ_FORMAT_RESOURCE_LINK:
        {


            break;
        }
        case BEJ_FORMAT_ENUM:
        {


            break;
        }
        case BEJ_FORMAT_NULL:
        {
            tuple_sfl.length = 0;
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

            FILE* collection_stream = tmpfile();
            bej_tuple_l count = 0;


            if(bej_collection_not_empty(BEJ_ARRAY_END, json_file))
            {
                bej_dict_entry* child_entries = NULL;
                read_dict_entry_children(&child_entries,  entries[seq_num].header, schema_dict_file);

                bool success = true;

                do
                {
                    success = bej_encode_stream(
                        json_file, collection_stream, schema_dict_file, annotation_dict_file,
                        child_entries, entries[seq_num].header->child_count, entries_selector,
                        false, true
                    );

                    ++count;
                    collection_sep = bej_read_delim(json_file);
                }
                while(collection_sep == BEJ_ELEMENT_SEPARATOR && success);

                if(collection_sep != BEJ_ARRAY_END || !success)
                {
                    fclose(collection_stream);
                    free(child_entries);
                    return false;
                }

                free(child_entries);
            }

            bej_pack_nnint(count, bej_nnint_length(count), collection_stream);
            tuple_sfl.length = ftello(collection_stream);
            bej_pack_sfl(&tuple_sfl, bej_file);
            bej_move_stream_data(collection_stream, bej_file);


            fclose(collection_stream);

            break;
        }
        case BEJ_FORMAT_PROPERTY_ANNOTATION:
        {

            break;
        }
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


    bej_dict_entry entry;
    fseeko(schema_dict_file, sizeof(bej_dict_header), SEEK_SET);
    read_dict_entry(&entry, schema_dict_file);


    //bej_stream_node* stream_stack = bej_append_stream_node(NULL, bej_file);

    bool success = bej_encode_stream(
        json_file, bej_file, schema_dict_file, annotation_dict_file,
        &entry, 1, BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA,
        false, false
    );

    return success;
}
