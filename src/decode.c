#include "decode.h"
#include <string.h>

void bej_unpack_string(const bej_tuple_l length, char* val, FILE* stream)
{
    fread(val, sizeof(char), length, stream);
}

void bej_unpack_boolean(const bej_tuple_l length, bool* val, FILE* stream)
{
    fread(val, sizeof(bool), 1, stream);
    fseeko(stream, length - sizeof(bool), SEEK_CUR);
}

void bej_unpack_real(const bej_tuple_l length, bej_real* val, FILE* stream)
{
    uint64_t whole_len = 0;
    uint64_t exponent_len = 0;

    bej_unpack_nnint(&whole_len, stream);
    bej_unpack_integer((bej_tuple_l)whole_len, &val->whole, stream);
    bej_unpack_nnint(&val->lead_zeros, stream);
    bej_unpack_nnint(&val->fract, stream);
    bej_unpack_nnint(&exponent_len, stream);
    bej_unpack_integer((bej_tuple_l)exponent_len, &val->exponent, stream);
}

void bej_unpack_integer(const bej_tuple_l length, int64_t* val, FILE* stream)
{
    uint8_t data = 0;;
    *val = 0;

    for(uint8_t i = 0; i < length; ++i)
    {
        fread(&data, sizeof(uint8_t), 1, stream);
        *val |= (uint64_t)data << (i * 8);
    }

    if(data >> 7 == 0b1)
    {
        for(uint8_t i = length; i < sizeof(int64_t); ++i)
        {
            *val |= (uint64_t)0xFF << (i * 8);
        }
    }
}

void bej_unpack_nnint(uint64_t* val, FILE* stream)
{
    uint8_t length = 0;
    fread(&length, sizeof(uint8_t), 1, stream);

    uint8_t data = 0;
    *val = 0;

    for(uint8_t i = 0; i < length; ++i)
    {
        fread(&data, sizeof(uint8_t), 1, stream);
        *val |= (uint64_t)data << (i * 8);
    }
}

void bej_unpack_tuple_s(bej_tuple_s* tuple_s, FILE* stream)
{
    uint64_t tuple_s_num;
    bej_unpack_nnint(&tuple_s_num, stream);

    tuple_s->dict_selector = (tuple_s_num & 0b1);
    tuple_s->seq_num = (tuple_s_num >> 1);
}

void bej_unpack_tuple_f(bej_tuple_f* tuple_f, FILE* stream)
{
    uint8_t tuple_f_num;
    fread(&tuple_f_num, sizeof(uint8_t), 1, stream);

    tuple_f->bej_type = tuple_f_num >> 4;

    tuple_f->reserved   = (tuple_f_num & 0xF) >> 3;
    tuple_f->nullable   = (tuple_f_num & 0xF) >> 2;
    tuple_f->ro_and_tla = (tuple_f_num & 0xF) >> 1;
    tuple_f->defer_bind = (tuple_f_num & 0xF) >> 0;
}

void bej_unpack_sfl(bej_tuple_sfl* tuple_sfl, FILE* stream)
{
    bej_unpack_tuple_s(&tuple_sfl->sequence, stream);
    bej_unpack_tuple_f(&tuple_sfl->format, stream);
    bej_unpack_nnint((uint64_t*)&tuple_sfl->length, stream);
}

void bej_decode_enum(
    FILE* json_file,
    FILE* dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entry,
    uint64_t value
)
{
    fseeko(
        dict_file,
        entry->header->offset + value * sizeof(bej_dict_entry),
        SEEK_SET
    );

    bej_dict_entry annotation_entry;
    read_dict_entry(&annotation_entry, dict_file);

    fprintf(json_file, "\"%s\"", annotation_entry.name);

    free_dict_entry(&annotation_entry);
}

void bej_decode_anotation_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    bej_tuple_s* sequence
)
{
    bej_dict_entry_header base_entry_header;
    bej_dict_entry annotation_entry;

    fseeko(annotation_dict_file, sizeof(bej_dict_header), SEEK_SET);
    fread(&base_entry_header, sizeof(bej_dict_entry_header), 1, annotation_dict_file);

    fseeko(annotation_dict_file, base_entry_header.offset, SEEK_SET);
    fseeko(annotation_dict_file, sequence->seq_num * sizeof(bej_dict_entry_header), SEEK_CUR);
    read_dict_entry(&annotation_entry, annotation_dict_file);

    if(annotation_entry.header->name_size > 0)
    {
        fprintf(json_file, "%s", annotation_entry.name);
    }

    free_dict_entry(&annotation_entry);
}

void bej_decode_name(
    FILE* json_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint16_t entries_count,
    uint8_t entries_selector
)
{
    if (tuple_sfl->sequence.dict_selector == entries_selector && tuple_sfl->format.ro_and_tla == 0)
    {
        if(entries[tuple_sfl->sequence.seq_num].header->name_size > 0)
        {
            fprintf(json_file, "\"%s\":", entries[tuple_sfl->sequence.seq_num].name);
        }
    }
    else if(entries_selector == BEJ_DICTIONARY_SELECTOR_ANNOTATION)
    {
        fprintf(json_file, "\"");
        bej_decode_anotation_name(json_file, annotation_dict_file, &tuple_sfl->sequence);
        fprintf(json_file, "\":");
    }
    else
    {
        if(entries[tuple_sfl->sequence.seq_num].header->name_size > 0)
        {
            fprintf(json_file, "\"%s\":", entries[tuple_sfl->sequence.seq_num].name);
        }
    }
}

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
)
{
    off_t stream_size = get_stream_size(bej_file);
    off_t stream_pos = ftello(bej_file);
    uint64_t curr_prop = 0;
    bool success = true;

    bej_tuple_sfl tuple_sfl;

    while (success && ftello(bej_file) < stream_size && curr_prop < prop_count)
    {
        bej_unpack_sfl(&tuple_sfl, bej_file);

        switch (tuple_sfl.format.bej_type)
        {
            case BEJ_FORMAT_SET:
            {
                off_t value_offset = ftello(bej_file);

                uint64_t count;
                bej_unpack_nnint(&count, bej_file);

                if(is_seq_array_idx)
                {
                    tuple_sfl.sequence.seq_num = 0;
                }

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }


                bej_dict_entry_header entry_header;
                bej_dict_entry* children;

                FILE* current_dict = get_collection_entry_header(
                    schema_dict_file, annotation_dict_file,
                    &tuple_sfl, entries, entries_selector,
                    &entry_header
                );

                fseeko(current_dict, entry_header.offset, SEEK_SET);
                read_dict_entry_children(&children, &entry_header, current_dict);

                fprintf(json_file, "{");

                success = bej_decode_stream(
                    json_file, bej_file, schema_dict_file, annotation_dict_file,
                    children, entry_header.child_count,
                    tuple_sfl.sequence.dict_selector, count, true, false
                );

                fprintf(json_file, "}");

                free_dict_entry_children(&children, entry_header.child_count);


                off_t current_pos = ftello(bej_file);

                if(current_pos - value_offset != tuple_sfl.length)
                {
                    return false;
                }

                break;
            }
            case BEJ_FORMAT_STRING:
            {
                char* val = calloc(tuple_sfl.length, sizeof(char));

                bej_unpack_string(tuple_sfl.length, val, bej_file);

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                fprintf(json_file, "\"%s\"", val);

                free(val);

                break;
            }
            case BEJ_FORMAT_INTEGER:
            {
                int64_t value;
                bej_unpack_integer(tuple_sfl.length, &value, bej_file);

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                fprintf(json_file, "%ld", value);

                break;
            }
            case BEJ_FORMAT_REAL:
            {
                bej_real value;
                bej_unpack_real(tuple_sfl.length, &value, bej_file);

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                fprintf(
                    json_file,
                    "%ld.%0*d%lue%+ld",
                    value.whole,
                    (int)value.lead_zeros, 0,
                    value.fract,
                    value.exponent
                );

                break;
            }
            case BEJ_FORMAT_BOOLEAN:
            {
                bool value;
                bej_unpack_boolean(tuple_sfl.length, &value, bej_file);

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                if(value)
                {
                    fprintf(json_file, "true");
                }
                else
                {
                    fprintf(json_file, "false");
                }

                break;
            }
            case BEJ_FORMAT_RESOURCE_LINK:
            {
                uint64_t value;
                bej_unpack_nnint(&value, bej_file);

                if(is_seq_array_idx)
                {
                    tuple_sfl.sequence.seq_num = 0;
                }

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                fprintf(json_file, "\"\"");

                break;
            }
            case BEJ_FORMAT_ENUM:
            {
                uint64_t value;
                bej_unpack_nnint(&value, bej_file);

                if(is_seq_array_idx)
                {
                    tuple_sfl.sequence.seq_num = 0;
                }

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                bej_dict_entry_header entry_header;
                bej_dict_entry entry;

                FILE* current_dict = get_collection_entry_header(
                    schema_dict_file, annotation_dict_file,
                    &tuple_sfl, entries, entries_selector,
                    &entry_header
                );

                fseeko(current_dict, entry_header.offset, SEEK_SET);
                fseeko(current_dict, value * sizeof(bej_dict_entry_header), SEEK_CUR);

                read_dict_entry(&entry, current_dict);

                fprintf(json_file, "\"%s\"", entry.name);

                free_dict_entry(&entry);

                break;
            }
            case BEJ_FORMAT_NULL:
            {
                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }

                fprintf(json_file, "null");

                break;
            }
            case BEJ_FORMAT_ARRAY:
            {
                off_t value_offset = ftello(bej_file);

                uint64_t count;
                bej_unpack_nnint(&count, bej_file);

                if(is_seq_array_idx)
                {
                    tuple_sfl.sequence.seq_num = 0;
                }

                if(add_name)
                {
                    bej_decode_name(
                        json_file, annotation_dict_file,
                        &tuple_sfl, entries, entries_count, entries_selector
                    );
                }


                bej_dict_entry_header entry_header;
                bej_dict_entry* children;

                FILE* current_dict = get_collection_entry_header(
                    schema_dict_file, annotation_dict_file,
                    &tuple_sfl, entries, entries_selector,
                    &entry_header
                );

                fseeko(current_dict, entry_header.offset, SEEK_SET);
                read_dict_entry_children(&children, &entry_header, current_dict);


                fprintf(json_file, "[");

                for(size_t i = 0; i < count; ++i)
                {
                    success = bej_decode_stream(
                        json_file, bej_file, schema_dict_file, annotation_dict_file,
                        children, entry_header.child_count,
                        tuple_sfl.sequence.dict_selector, 1, false, true
                    );

                    if(i + 1 < count)
                    {
                        fprintf(json_file, ",");
                    }
                }

                fprintf(json_file, "]");


                free_dict_entry_children(&children, entry_header.child_count);

                off_t current_pos = ftello(bej_file);

                if(current_pos - value_offset != tuple_sfl.length)
                {
                    return false;
                }

                break;
            }
            case BEJ_FORMAT_PROPERTY_ANNOTATION:
            {
                bej_tuple_s property_sequence;
                bej_unpack_tuple_s(&property_sequence, bej_file);

                fseeko(bej_file, stream_pos, SEEK_SET);

                fprintf(json_file, "\"");
                fprintf(json_file, "%s", entries[property_sequence.seq_num].name);
                bej_decode_anotation_name(json_file, annotation_dict_file, &tuple_sfl.sequence);
                fprintf(json_file, "\":");

                bej_dict_entry_header entry_header;
                bej_dict_entry* children;

                fseeko(annotation_dict_file, sizeof(bej_dict_header), SEEK_SET);
                fread(&entry_header, sizeof(bej_dict_entry_header), 1, annotation_dict_file);

                fseeko(annotation_dict_file, entry_header.offset, SEEK_SET);
                read_dict_entry_children(&children, &entry_header, annotation_dict_file);

                success = bej_decode_stream(
                    json_file, bej_file, schema_dict_file, annotation_dict_file,
                    children, entry_header.child_count,
                    BEJ_DICTIONARY_SELECTOR_ANNOTATION, 1, false, false
                );

                free_dict_entry_children(&children, entry_header.child_count);

                break;
            }
            default:
            {
                success = false;
                break;
            }
        }

        if(curr_prop + 1 < prop_count)
        {
            fprintf(json_file, ",");
        }

        ++curr_prop;
    }

    return success;
}

bool bej_decode(
    FILE* json_file,
    FILE* bej_file,
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    FILE* pdr_map_file
)
{
    bej_file_header bej_header;
    fseeko(bej_file, 0, SEEK_SET);
    fread(&bej_header, sizeof(bej_file_header), 1, bej_file);


    assert(bej_header.version == BEJ_VERSION_1_0_0
        || bej_header.version == BEJ_VERSION_1_1_0);

    assert(bej_header.flags == 0);

    assert(bej_header.schema_class == BEJ_SCHEMA_CLASS_MAJOR
        || bej_header.schema_class == BEJ_SCHEMA_CLASS_EVENT);


    bool success = true;


    // Major schema class or Event
    fseeko(schema_dict_file, sizeof(bej_dict_header), SEEK_SET);

    bej_dict_entry entry;

    read_dict_entry(&entry, schema_dict_file);

    success = bej_decode_stream(
        json_file, bej_file, schema_dict_file, annotation_dict_file,
        &entry, 1,
        BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA, 1, false, false
    );

    free_dict_entry(&entry);


    return success;
}
