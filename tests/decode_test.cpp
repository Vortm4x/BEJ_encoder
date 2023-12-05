#include <catch2/catch.hpp>
#include "annotation.h"
#include "schema.h"
#include "bej_file.h"

extern "C"
{
    #include "decode.h"
}

TEST_CASE("Testing decoding json...", "[decode]")
{
    FILE* schema_dict_file = tmpfile();
    FILE* anotation_dict_file = tmpfile();
    FILE* bej_file = tmpfile();
    FILE* json_file = tmpfile();

    fwrite(schema_dict_bin, sizeof(uint8_t), SCHEMA_DICT_BIN_LEN, schema_dict_file);
    fwrite(annotation_dict_bin, sizeof(uint8_t), ANNOTATION_DICT_BIN_LEN, anotation_dict_file);
    fwrite(bej_file_bin, sizeof(uint8_t), BEJ_FILE_BIN_LEN, bej_file);

    fseeko(schema_dict_file, 0, SEEK_SET);
    fseeko(anotation_dict_file, 0, SEEK_SET);
    fseeko(bej_file, 0, SEEK_SET);
    fseeko(json_file, 0, SEEK_SET);

    REQUIRE(bej_decode(json_file, bej_file, schema_dict_file, anotation_dict_file, NULL));
}
