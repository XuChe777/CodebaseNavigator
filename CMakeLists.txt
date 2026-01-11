cmake_minimum_required(VERSION 3.20)

project(
	codebase_analyzer
    VERSION 1.0.0
    LANGUAGES C
)

# ------------------------------
# Global configuration
# ------------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# ------------------------------
# Dependencies
# ------------------------------
find_package(cJSON REQUIRED)
find_package(CURL REQUIRED)

# ------------------------------
# Warnings & compiler flags
# ------------------------------
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wconversion
        -Wno-unused-parameter
    )
endif()

# ------------------------------
# Sanitizers (Debug only)
# ------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug"
   AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# ------------------------------
# Project layout
# ------------------------------
add_executable(cba
	src/symbol_extractor/directory_scanner.c
	src/symbol_extractor/file_symbol_extractor.c
	src/symbol_extractor/symbol_content_extractor.c
	src/symbol_extractor/symbol_typedef.c
	src/main.c
	src/parser.c
)
target_link_libraries(cba
	PRIVATE
	tree-sitter
	tree-sitter-c
	cjson
	CURL::libcurl
)

add_executable(Treesitter_Test
	tests/treesitter.c
)
target_link_libraries(Treesitter_Test
	PRIVATE
	tree-sitter
	tree-sitter-c
)

# ------------------------------
# Installation (optional)
# ------------------------------
install(TARGETS cba
        RUNTIME DESTINATION bin)
