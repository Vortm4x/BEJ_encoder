#include <catch2/catch.hpp>
#include <stdio.h>
#include "annotation.h"
#include "schema.h"
#include "json_file.h"

extern "C"
{
    #include "encode.h"
}

TEST_CASE("Testing encoding json...", "[encode]")
{
    FILE* schema_dict_file = tmpfile();
    FILE* anotation_dict_file = tmpfile();
    FILE* bej_file = tmpfile();
    FILE* json_file = tmpfile();

    fwrite(schema_dict_bin, sizeof(uint8_t), SCHEMA_DICT_BIN_LEN, schema_dict_file);
    fwrite(annotation_dict_bin, sizeof(uint8_t), ANNOTATION_DICT_BIN_LEN, anotation_dict_file);
    fwrite(json_file_bin, sizeof(uint8_t), JSON_FILE_BIN_LEN, json_file);

    fseeko(schema_dict_file, 0, SEEK_SET);
    fseeko(anotation_dict_file, 0, SEEK_SET);
    fseeko(bej_file, 0, SEEK_SET);
    fseeko(json_file, 0, SEEK_SET);

    REQUIRE(bej_encode(json_file, bej_file, schema_dict_file, anotation_dict_file, NULL));
}
