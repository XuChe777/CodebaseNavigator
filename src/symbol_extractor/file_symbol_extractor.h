#ifndef FILE_SYMBOL_EXTRACTOR_H
#define FILE_SYMBOL_EXTRACTOR_H

#include "symbol_typedef.h"

void push_symbol(SymbolTable *table, const char *name, SymbolType type,
                 uint32_t start_line, uint32_t end_line,
                 const char *source_file);

void index_file(const char *path, void *user_data);

#endif // FILE_SYMBOL_EXTRACTOR_H
