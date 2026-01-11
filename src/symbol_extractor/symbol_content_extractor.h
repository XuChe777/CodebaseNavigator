#ifndef SYMBOL_CONTENT_EXTRACTOR_H
#define SYMBOL_CONTENT_EXTRACTOR_H

#include "symbol_typedef.h"
#include <tree_sitter/api.h>

void symbol_extract_function(Symbol *sym, TSNode func_node, const char *source, const char *file);
void symbol_extract_struct(Symbol *sym, TSNode st_node, const char *source, const char *file);

#endif // SYMBOL_CONTENT_EXTRACTOR_H
