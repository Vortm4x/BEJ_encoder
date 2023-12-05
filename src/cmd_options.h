/**
 *
 *  @file
 *  @brief File contains functions for simplified
 *  command line options handling
 *
 **/

#ifndef BEJPARSE_OPTIONS_H
#define BEJPARSE_OPTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>


/**
 *  @struct bej_cmd_options
 *  @brief Structure for easy access to command line arguments
 *
 *  @var bej_cmd_options::help_opt
 *  @brief help option indicator
 *
 *  @var bej_cmd_options::quiet_opt
 *  @brief quiet option indicator
 *
 *  @var bej_cmd_options::verbose_opt
 *  @brief verbose option indicator
 *
 *  @var bej_cmd_options::encode_opt
 *  @brief encode option indicator
 *
 *  @var bej_cmd_options::decode_opt
 *  @brief decode option indicator
 *
 *  @var bej_cmd_options::schema_dict_opt
 *  @brief schema_dict option indicator
 *
 *  @var bej_cmd_options::annotation_dict_opt
 *  @brief annotation_dict option indicator
 *
 *  @var bej_cmd_options::pdrmap_file_opt
 *  @brief pdrmap_file option indicator
 *
 *  @var bej_cmd_options::json_file_opt
 *  @brief json_file option indicator
 *
 *  @var bej_cmd_options::bej_file_opt
 *  @brief bej_file option indicator
 *
 *  @var bej_cmd_options::schema_dict_path
 *  @brief Schema dictionary file path
 *
 *  @var bej_cmd_options::annotation_dict_path
 *  @brief Annotation dictionary file path
 *
 *  @var bej_cmd_options::pdrmap_file_path
 *  @brief PDR map file path
 *
 *  @var bej_cmd_options::json_file_path
 *  @brief JSON document file path
 *
 *  @var bej_cmd_options::bej_file_path
 *  @brief Binary encoded JSON document file path
 *
 *  @typedef bej_cmd_options
 *  @brief default struct typedef
 **/
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

/**
 *  @typedef bej_long_option
 *  @brief struct option alias that used in getopt_long function
 **/
typedef struct option bej_long_option;


/**
 *  Retrieve command line options and their arguments from command
 *  line arguments
 *
 *  @param[in]  argc        Command line arguments count
 *  @param[in]  argv        Command line arguments string
 *  @param[in]  long_opts   Long options array for getopt_long function
 *  @param[in]  short_opts  Short options null-terminated string
 *  @param[out] parse_opts  Options structure with arguments
 *  @returns True if options are valid, false otherwise
 *  @warning Long options should have same codes as short equivalents
 **/
bool set_opts(
    int argc,
    char* const argv[],
    const bej_long_option* long_opts,
    const char* short_opts,
    bej_cmd_options* parse_opts
);


/**
 *  Get index of long option by its short equivalent
 *
 *  @param[in]  long_opts   Long options array
 *  @param[in]  option_code  Short option code
 *  @returns Index of option by its option code
 *  or -1 if there is no such option
 **/
int get_long_opt(
    const bej_long_option* long_opts,
    const int option_code
);


/**
 *  Print error message related to specific option
 *
 *  @param[in]  long_opts   Long options array
 *  @param[in]  option_code  Short option code
 *  @param[in]  msg         Error message
 **/
void print_option_hint(
    const bej_long_option* long_opts,
    const int option_code,
    const char* msg
);


/**
 *  Prints usage info entries as a list.
 *  Each entry printed from new line
 *
 *  @param[in]  usage       Usage info
 **/
void print_usage_info(const char* usage[]);


/**
 *  Prints help info for all options
 *
 *
 *  @param[in]  long_opts   Long options array
 *  @param[in]  opt_descs   String list with options description
 *  @warning Arrays should have same length
 **/
void print_options_info(
    const bej_long_option* long_opts,
    const char* opt_descs[]
);


/**
 *  Utilizing function to open file by path provided as an option argument.
 *  Prints error message on failure
 *
 *  @param[in]  path            Path to file
 *  @param[in]  mode            fopen file mode
 *  @param[in]  long_opts       Long options array
 *  @param[in]  option_code     Short code of related option
 *  @returns Valid FILE pointer (stream object) on success
 *  or NULL on failure
 **/
FILE* open_option_file(
    const char* path,
    const char* mode,
    const bej_long_option* long_opts,
    const int option_code);

#endif // BEJPARSE_OPTIONS_H
