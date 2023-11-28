cmake_minimum_required(VERSION 3.5)

project(BEJ_parser LANGUAGES C)
set(C_STANDARD_REQUIRED C99)

add_executable(${PROJECT_NAME}
    "src/main.c"
    "src/bejparse_options.c" "src/bejparse_options.h"
)
