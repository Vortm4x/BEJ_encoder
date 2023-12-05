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


void read_dict_entry(
    bej_dict_entry* entry,
    FILE* stream
)
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


void read_dict_entry_children(
    bej_dict_entry** children,
    const bej_dict_entry_header* entry_header,
    FILE* stream
)
{
    *children = calloc(entry_header->child_count, sizeof(bej_dict_entry));

    for(int i = 0; i < entry_header->child_count; ++i)
    {
        read_dict_entry(*children + i, stream);
    }
}

void free_dict_entry_children(
    bej_dict_entry** children,
    const uint16_t child_count
)
{
    for(int i = 0; i < child_count; ++i)
    {
        free_dict_entry(*children + i);
    }

    free(*children);
    *children = NULL;
}
