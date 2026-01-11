#ifndef SYMBOL_CONTENT_EXTRACTOR_H
#define SYMBOL_CONTENT_EXTRACTOR_H

#include "symbol_typedef.h"
#include <stddef.h>
#include <tree_sitter/api.h>

extern const char* function_properties_to_extract[];
extern const size_t number_of_function_properties_to_extract;
extern const char* struct_properties_to_extract[];
extern const size_t number_of_struct_properties_to_extract;
extern const char* enum_properties_to_extract[];
extern const size_t number_of_enum_properties_to_extract;

extern void symbol_extract_info_as_function(Symbol *sym, TSNode func_node, const char *source);
extern void symbol_extract_info_as_struct(Symbol *sym, TSNode st_node, const char *source);
extern void symbol_extract_info_as_enum(Symbol *sym, TSNode en_node, const char *source);

#endif // SYMBOL_CONTENT_EXTRACTOR_H
