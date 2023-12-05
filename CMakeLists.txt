cmake_minimum_required(VERSION 3.21)

project(BEJ_parser LANGUAGES C)
set(C_STANDARD_REQUIRED C99)

set(EXE_TARGET_NAME         ${PROJECT_NAME})
set(LIB_TARGET_NAME         ${PROJECT_NAME}_lib)
set(TEST_TARGET_NAME        ${PROJECT_NAME}_tests)

set(PROJECT_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/src")

add_library(${LIB_TARGET_NAME} STATIC
    "${PROJECT_SOURCE_PATH}/encode.c" "${PROJECT_SOURCE_PATH}/encode.h"
    "${PROJECT_SOURCE_PATH}/decode.c" "${PROJECT_SOURCE_PATH}/decode.h"
    "${PROJECT_SOURCE_PATH}/utils.c"  "${PROJECT_SOURCE_PATH}/utils.h"
)

add_executable(${EXE_TARGET_NAME}
    "${PROJECT_SOURCE_PATH}/cmd_options.c" "${PROJECT_SOURCE_PATH}/cmd_options.h"
    "${PROJECT_SOURCE_PATH}/main.c"
)

target_link_libraries(${EXE_TARGET_NAME} PRIVATE ${LIB_TARGET_NAME})

add_subdirectory(tests)
