#include "symbol_typedef.h"

#include <stdlib.h>
#include <string.h>

void symbol_table_init(SymbolTable *table) {
    table->symbols = NULL;
    table->size = 0;
    table->capacity = 0;
}

void symbol_table_push_symbol(SymbolTable *table, Symbol symbol) {
    if (table->size == table->capacity) {
        table->capacity = table->capacity ? table->capacity * 2 : 8;
        table->symbols = realloc(table->symbols, table->capacity * sizeof(Symbol));
    }
	table->symbols[table->size] = symbol;
	
    table->size++;
}

void symbol_table_free(SymbolTable *table) {
    for (size_t i = 0; i < table->size; i++) {
        free(table->symbols[i].name);
		free(table->symbols[i].source_file_name);
    }
    free(table->symbols);
}
