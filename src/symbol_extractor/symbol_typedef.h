#ifndef SYMBOL_TYPEDEF_H
#define SYMBOL_TYPEDEF_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    SYMBOL_FUNCTION,
    SYMBOL_STRUCT
} SymbolType;

typedef struct
{
    SymbolType type;
    char* name;  // function or struct name
    char* signature;
    char* source_file_name;
    uint32_t start_line;
    uint32_t end_line;
} Symbol;

typedef struct
{
    Symbol* symbols;
    size_t size;
    size_t capacity;
} SymbolTable;

extern void symbol_table_init(SymbolTable* table);
extern void symbol_table_free(SymbolTable* table);

extern void symbol_table_push_symbol(SymbolTable* table, Symbol symbol);

#endif  // SYMBOL_TYPEDEF_H
