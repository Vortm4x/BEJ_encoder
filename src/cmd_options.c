#include "cmd_options.h"


bool set_opts(
    int argc,
    char* const argv[],
    const bej_long_option* long_opts,
    const char* short_opts,
    bej_cmd_options* parse_opts
)
{
    opterr = 1;

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
                if(parse_opts->schema_dict_opt)
                {
                    print_option_hint(long_opts, option_code, "option duplicate");
                    return false;
                }

                parse_opts->schema_dict_opt = true;
                parse_opts->schema_dict_path = optarg;
                break;
            }
            case 'a':
            {
                if(parse_opts->annotation_dict_opt)
                {
                    print_option_hint(long_opts, option_code, "option duplicate");
                    return false;
                }

                parse_opts->annotation_dict_opt = true;
                parse_opts->annotation_dict_path = optarg;
                break;
            }
            case 'p':
            {
                if(parse_opts->pdrmap_file_opt)
                {
                    print_option_hint(long_opts, option_code, "option duplicate");
                    return false;
                }

                parse_opts->pdrmap_file_opt = true;
                parse_opts->pdrmap_file_path = optarg;
                break;
            }
            case 'j':
            {
                if(parse_opts->json_file_opt)
                {
                    print_option_hint(long_opts, option_code, "option duplicate");
                    return false;
                }

                parse_opts->json_file_opt = true;
                parse_opts->json_file_path = optarg;
                break;
            }
            case 'b':
            {
                if(parse_opts->bej_file_opt)
                {
                    print_option_hint(long_opts, option_code, "option duplicate");
                    return false;
                }

                parse_opts->bej_file_opt = true;
                parse_opts->bej_file_path = optarg;
                break;
            }
            case '?':
            {
                return false;
                break;
            }
        }
    }

    return true;
}

int get_long_opt(
    const bej_long_option* long_opts,
    const int option_code
)
{
    int pos = -1;

    for(int i = 0; long_opts[i].name != NULL; ++i)
    {
        if(long_opts[i].val == option_code)
        {
            pos = i;
            break;
        }
    }

    return pos;
}

void print_option_hint(
    const bej_long_option* long_opts,
    const int option_code,
    const char* msg
)
{
    int pos = get_long_opt(long_opts, option_code);

    if(pos != -1)
    {
        fprintf(stderr,
            "error: option -%c/--%s: %s\n",
            long_opts[pos].val,
            long_opts[pos].name,
            msg
        );
    }
}

void print_usage_info(const char* opt_usage[])
{
    fprintf(stderr, "usage: COMMAND");

    for(int i = 0; opt_usage[i] != NULL; ++i)
    {
        fprintf(stderr, "\n\t%s", opt_usage[i]);
    }

    fprintf(stderr, "\n");
}

void print_options_info(const bej_long_option* long_opts, const char* opt_descs[])
{
    fprintf(stderr, "options:");

    for(int i = 0; opt_descs[i] != NULL; ++i)
    {
        fprintf(stderr,
            "\n\t-%c, --%-30s%s\n",
            (char)long_opts[i].val,
            long_opts[i].name,
            opt_descs[i]
        );
    }
}

FILE* open_option_file(
    const char* path,
    const char* mode,
    const bej_long_option* long_opts,
    const int option_code)
{
    FILE* file = fopen(path, mode);

    if(file == NULL)
    {
        print_option_hint(long_opts, option_code, "can't open file");
        perror(path);

        exit(EXIT_FAILURE);
    }

    return file;
}
