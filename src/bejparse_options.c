#include "bejparse_options.h"


void set_opts(
    int argc,
    char* const argv[],
    const long_option* long_opts,
    const char* short_opts,
    bejparse_options* parse_opts
)
{
    int option_code = -1;
    while((option_code = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1)
    {
        switch (option_code)
        {
            case 'h':
            {
                parse_opts->help_opt = true;
                break;
            }
            case 'q':
            {
                parse_opts->quiet_opt = true;
                break;
            }
            case 'v':
            {
                parse_opts->verbose_opt = true;
                break;
            }
            case 'e':
            {
                parse_opts->encode_opt = true;
                break;
            }
            case 'd':
            {
                parse_opts->decode_opt = true;
                break;
            }
            case 's':
            {
                parse_opts->schema_dictionary_path = optarg;
                break;
            }
            case 'a':
            {
                parse_opts->annotation_dictionary_path = optarg;
                break;
            }
            case 'p':
            {
                parse_opts->pdrmap_file_path = optarg;
                break;
            }
            case 'j':
            {
                parse_opts->json_file_path = optarg;
                break;
            }
            case 'b':
            {
                parse_opts->bej_file_path = optarg;
                break;
            }
        }
    }
}
