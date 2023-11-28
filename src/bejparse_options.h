#ifndef BEJPARSE_OPTIONS_H
#define BEJPARSE_OPTIONS_H

#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>


typedef struct bej_parse_options
{
    bool help_opt;
    bool quiet_opt;
    bool verbose_opt;
    bool encode_opt;
    bool decode_opt;

    char *schema_dictionary_path;
    char *annotation_dictionary_path;
    char *pdrmap_file_path;
    char *json_file_path;
    char *bej_file_path;
}
bejparse_options;

typedef struct option long_option;


void set_opts(
    int argc,
    char* const argv[],
    const long_option* long_opts,
    const char* short_opts,
    bejparse_options* parse_opts
);



#endif // BEJPARSE_OPTIONS_H
