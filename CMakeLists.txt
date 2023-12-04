cmake_minimum_required(VERSION 3.21)

project(BEJ_parser LANGUAGES C)
set(C_STANDARD_REQUIRED C99)

add_executable(${PROJECT_NAME}
    "src/main.c"
    "src/cmd_options.c" "src/cmd_options.h"
    "src/decode.c" "src/decode.h"
    "src/utils.c" "src/utils.h"
    "src/encode.c" "src/encode.h"
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
)
