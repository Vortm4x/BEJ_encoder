#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_options.h"
#include "encode.h"
#include "decode.h"


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
        { "schema-dict",            required_argument,  NULL, (int)'s' },
        { "annotation-dict",        required_argument,  NULL, (int)'a' },
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
    bej_cmd_options parse_opts;
    memset(&parse_opts, 0, sizeof(bej_cmd_options));
    if(!set_opts(argc, argv, long_opts, short_opts, &parse_opts))
    {
        print_usage_info(opt_usage);
        return EXIT_FAILURE;
    }

    if(parse_opts.help_opt)
    {
        //print help
        print_options_info(long_opts, opt_descs);
        print_usage_info(opt_usage);
        fprintf(stderr, "\n");

        return EXIT_SUCCESS;
    }


    // override quiet flag if verbose is set
    if(parse_opts.quiet_opt && parse_opts.verbose_opt)
    {
        parse_opts.quiet_opt = false;
    }

    // annotation dict and schema dict are both required
    if(!parse_opts.annotation_dict_opt)
    {
        print_option_hint(long_opts, (int)'a', "option required");
        return EXIT_FAILURE;
    }
    if(!parse_opts.schema_dict_opt)
    {
        print_option_hint(long_opts, (int)'s', "option required");
        return EXIT_FAILURE;
    }

    FILE* annotation_dict_file = NULL;
    FILE* schema_dict_file = NULL;
    FILE* json_file = NULL;
    FILE* bej_file = NULL;
    FILE* pdr_map_file = NULL;


    annotation_dict_file = open_option_file(parse_opts.annotation_dict_path, "rb", long_opts, (int)'a');
    schema_dict_file = open_option_file(parse_opts.schema_dict_path, "rb", long_opts, (int)'s');

    if(parse_opts.encode_opt && !parse_opts.decode_opt)
    {
        //encode

        if(parse_opts.json_file_opt)
        {
            json_file = open_option_file(parse_opts.json_file_path, "r", long_opts, (int)'j');
        }
        else
        {
            json_file = stdin;
        }

        if(parse_opts.bej_file_opt)
        {
            bej_file = open_option_file(parse_opts.bej_file_path, "wb", long_opts, (int)'b');
        }
        else
        {
            bej_file = stdout;
        }

        if(parse_opts.pdrmap_file_opt)
        {
            pdr_map_file = open_option_file(parse_opts.pdrmap_file_path, "w", long_opts, (int)'p');
        }


        bej_encode(
            json_file,
            bej_file,
            schema_dict_file,
            annotation_dict_file,
            pdr_map_file
        );

    }
    else
    if(parse_opts.decode_opt && !parse_opts.encode_opt)
    {
        //decode

        if(parse_opts.json_file_opt)
        {
            json_file = open_option_file(parse_opts.json_file_path, "w", long_opts, (int)'j');
        }
        else
        {
            json_file = stdout;
        }

        if(parse_opts.bej_file_opt)
        {
            bej_file = open_option_file(parse_opts.bej_file_path, "rb", long_opts, (int)'b');
        }
        else
        {
            bej_file = stdin;
        }

        if(parse_opts.pdrmap_file_opt)
        {
            pdr_map_file = open_option_file(parse_opts.pdrmap_file_path, "r", long_opts, (int)'p');
        }


        bej_decode(
            json_file,
            bej_file,
            schema_dict_file,
            annotation_dict_file,
            pdr_map_file
        );
    }
    else
    {
        // if both or neither operation flags is set - print usage
        print_usage_info(opt_usage);
        return EXIT_FAILURE;
    }


    fclose(annotation_dict_file);
    fclose(schema_dict_file);

    if(parse_opts.json_file_opt)
    {
        fclose(json_file);
    }

    if(parse_opts.bej_file_opt)
    {
        fclose(bej_file);
    }

    if(parse_opts.pdrmap_file_opt)
    {
        fclose(pdr_map_file);
    }

    return EXIT_SUCCESS;
}
