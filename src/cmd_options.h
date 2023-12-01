#ifndef BEJPARSE_OPTIONS_H
#define BEJPARSE_OPTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>


typedef struct bej_cmd_options
{
    bool help_opt;
    bool quiet_opt;
    bool verbose_opt;
    bool encode_opt;
    bool decode_opt;
    bool schema_dict_opt;
    bool annotation_dict_opt;
    bool pdrmap_file_opt;
    bool json_file_opt;
    bool bej_file_opt;

    char *schema_dict_path;
    char *annotation_dict_path;
    char *pdrmap_file_path;
    char *json_file_path;
    char *bej_file_path;
}
bej_cmd_options;

typedef struct option long_option;


bool set_opts(
    int argc,
    char* const argv[],
    const long_option* long_opts,
    const char* short_opts,
    bej_cmd_options* parse_opts
);

int get_long_opt(const long_option* long_opts, const int short_code);
void print_option_hint(const long_option* long_opts, const int option_code, const char* msg);
void print_usage_info(const char* usage[]);
void print_options_info(const long_option* long_opts, const char* opt_descs[]);

FILE* open_option_file(
    const char* path,
    const char* mode,
    const long_option* long_opts,
    const int option_code);

#endif // BEJPARSE_OPTIONS_H
