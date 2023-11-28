#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bejparse_options.h"


void print_usage_info(const char* usage[]);
void print_options_info(const long_option* long_opts, const char* opt_descs[]);
void print_help(const long_option* long_opts, const char* opt_descs[], const char* usage[]);


void print_usage_info(const char* opt_usage[])
{
    fprintf(stderr, "usage: COMMAND");

    for(int i = 0; opt_usage[i] != NULL; ++i)
    {
        fprintf(stderr, "\n\t%s", opt_usage[i]);
    }

    fprintf(stderr, "\n");
}

void print_options_info(const long_option* long_opts, const char* opt_descs[])
{
    fprintf(stderr, "options:");

    for(int i = 0; opt_descs[i] != NULL; ++i)
    {
        fprintf(stderr,
            "\n\t-%c, --%-30s%s",
            (char)long_opts[i].val,
            long_opts[i].name,
            opt_descs[i]
        );
    }

    fprintf(stderr, "\n");
}

void print_help(const long_option* long_opts, const char* opt_descs[], const char* opt_usage[])
{
    print_usage_info(opt_usage);
    print_options_info(long_opts, opt_descs);
    fprintf(stderr, "\n");
}



int main(int argc, char* argv[])
{
    // command short options
    const char* short_opts = "hqveds:a:p:j:b:";

    // command long options (long value is the same as short value)
    const long_option long_opts[] = {
        { "help",                   no_argument,        NULL, (int)'h' },
        { "quiet",                  no_argument,        NULL, (int)'q' },
        { "verbose",                no_argument,        NULL, (int)'v' },
        { "encode",                 no_argument,        NULL, (int)'e' },
        { "decode",                 no_argument,        NULL, (int)'d' },
        { "schema-dictionary",      required_argument,  NULL, (int)'s' },
        { "annotation-dictionary",  required_argument,  NULL, (int)'a' },
        { "pdrmap-file",            required_argument,  NULL, (int)'p' },
        { "json-file",              required_argument,  NULL, (int)'j' },
        { "bej-file",               required_argument,  NULL, (int)'b' },
        { NULL, 0, NULL, 0 }
    };

    // command options descriptions (has the same order as long_options)
    const char* opt_descs[] = {
        "show this help message and exit",
        "no output prints unless errors",
        "increase output verbosity",
        "encode json file",
        "decode bej file",
        "schema dictionary file path",
        "annotation dictionary file path",
        "PDR map file path",
        "JSON file path",
        "BEJ file path",
        NULL
    };

    // string array with exmple usage data
    const char* opt_usage[] = {
        "[-h]",
        "[-v|-q]",
        "-d|-e",
        "-s <SCHEMA_DICTIONARY>",
        "-a <ANNOTATION_DICTIONARY>",
        "-j <JSON_FILE> | -b <BEJ_FILE>",
        "[-p <PDRMAP_FILE>]\n",
        NULL
    };

    // retreve all options
    bejparse_options parse_opts;
    memset(&parse_opts, 0, sizeof(bejparse_options));
    set_opts(argc, argv, long_opts, short_opts, &parse_opts);

    // override quiet if verbose is set
    if(parse_opts.quiet_opt && parse_opts.verbose_opt)
    {
        parse_opts.quiet_opt = false;
    }

    if(parse_opts.help_opt)
    {
        //print help
        print_help(long_opts, opt_descs, opt_usage);
    }
    else
    if(parse_opts.encode_opt && !parse_opts.decode_opt)
    {
        //encode
        fprintf(stderr, "TODO: %s\n", "encode");
    }
    else
    if(parse_opts.decode_opt && !parse_opts.encode_opt)
    {
        //decode
        fprintf(stderr, "TODO: %s\n", "decode");
    }
    else
    {
        print_usage_info(opt_usage);
    }

    return EXIT_SUCCESS;
}
