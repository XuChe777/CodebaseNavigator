#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unofficial::tree-sitter::tree-sitter" for configuration "Debug"
set_property(TARGET unofficial::tree-sitter::tree-sitter APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(unofficial::tree-sitter::tree-sitter PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/libtree-sitter.a"
  )

list(APPEND _cmake_import_check_targets unofficial::tree-sitter::tree-sitter )
list(APPEND _cmake_import_check_files_for_unofficial::tree-sitter::tree-sitter "${_IMPORT_PREFIX}/debug/lib/libtree-sitter.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
