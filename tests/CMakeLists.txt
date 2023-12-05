project(${TEST_TARGET_NAME} LANGUAGES CXX)
set(CPP_STANDARD_REQUIRED CXX17)

find_package(Catch2 REQUIRED)


add_executable(${TEST_TARGET_NAME}
    "encode_test.cpp"
    "decode_test.cpp"
    "annotation.h"
    "schema.h"
    "bej_file.h"
    "json_file.h"
)

target_include_directories(${TEST_TARGET_NAME} PUBLIC "${PROJECT_SOURCE_PATH}")

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE BUILD_TEST)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
	Catch2::Catch2
    Catch2::Catch2WithMain
    ${LIB_TARGET_NAME}
)
