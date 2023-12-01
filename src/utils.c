#include "utils.h"

off_t get_stream_size(FILE* stream)
{
    off_t curr_pos, stream_size;

    curr_pos = ftello(stream);
    fseeko(stream, 0, SEEK_END);

    stream_size = ftello(stream);
    fseeko(stream, curr_pos, SEEK_SET);

    return stream_size;
}

FILE* get_collection_entry_header(
    FILE* schema_dict_file,
    FILE* annotation_dict_file,
    bej_tuple_sfl* tuple_sfl,
    bej_dict_entry* entries,
    uint8_t entries_selector,
    bej_dict_entry_header* entry_header
)
{
    FILE* current_dict;

    if(tuple_sfl->sequence.dict_selector == BEJ_DICTIONARY_SELECTOR_MAJOR_SCHEMA)
    {
        current_dict = schema_dict_file;
    }
    else
    {
        current_dict = annotation_dict_file;
    }

    if (tuple_sfl->sequence.dict_selector != entries_selector || tuple_sfl->format.ro_and_tla == 1)
    {
        fseeko(current_dict, sizeof(bej_dict_header), SEEK_SET);
        fread(&entry_header, sizeof(bej_dict_entry_header), 1, current_dict);

        fseeko(current_dict, entry_header->offset, SEEK_SET);
        fseeko(current_dict, tuple_sfl->sequence.seq_num * sizeof(bej_dict_entry_header), SEEK_CUR);

        fread(&entry_header, sizeof(bej_dict_entry_header), 1, current_dict);
    }
    else
    {
        *entry_header = *entries[tuple_sfl->sequence.seq_num].header;
    }

    return current_dict;
}


void read_dict_entry(bej_dict_entry* entry, FILE* stream)
{
    entry->header = malloc(sizeof(bej_dict_entry_header));
    fread(entry->header, sizeof(bej_dict_entry_header), 1, stream);

    off_t curr_pos = ftello(stream);
    fseeko(stream, entry->header->name_ofset, SEEK_SET);

    entry->name = malloc(entry->header->name_size);
    fread(entry->name, sizeof(char), entry->header->name_size, stream);

    fseeko(stream, curr_pos, SEEK_SET);
}


void free_dict_entry(bej_dict_entry* entry)
{
    free(entry->name);
    free(entry->header);

    entry->header = NULL;
    entry->name = NULL;
}


void read_dict_entry_children(bej_dict_entry** children, bej_dict_entry_header* entry_header, FILE* stream)
{
    *children = calloc(entry_header->child_count, sizeof(bej_dict_entry));

    for(int i = 0; i < entry_header->child_count; ++i)
    {
        read_dict_entry(*children + i, stream);
    }
}

void free_dict_entry_children(bej_dict_entry** children, uint16_t child_count)
{
    for(int i = 0; i < child_count; ++i)
    {
        free_dict_entry(*children + i);
    }

    free(*children);
    *children = NULL;
}
