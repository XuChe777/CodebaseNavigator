cmake_minimum_required(VERSION 3.20)

project(
    code_navigator
    VERSION 0.1.0
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
find_package(SDL3 REQUIRED)
# find_package(tree-sitter-c CONFIG REQUIRED)

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
add_executable(App
	src/symbol_extractor/directory_scanner.c
	src/symbol_extractor/file_symbol_extractor.c
	src/symbol_extractor/symbol_content_extractor.c
	src/symbol_extractor/symbol_typedef.c
	src/main.c
)
target_link_libraries(App
	# SDL3::SDL3
	PRIVATE
	tree-sitter
	tree-sitter-c
)

add_executable(SDL_Test
	tests/sdl3.c
)
target_link_libraries(SDL_Test
	PRIVATE
	SDL3::SDL3
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
# install(TARGETS code_navigator
#         RUNTIME DESTINATION bin)
