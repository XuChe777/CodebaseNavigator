#include "symbol_extractor/symbol_typedef.h"
#include "symbol_extractor/directory_scanner.h"
#include "symbol_extractor/file_symbol_extractor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

int main(void){
	SymbolTable table = {0};

    scan_directory("src", index_file, &table);

    for (size_t i = 0; i < table.size; i++) {
        Symbol *s = &table.symbols[i];
		if(s->type == SYMBOL_FUNCTION){
			// printf("FUNCTION %-20s %-20s %s\n", s->signature, s->name, s->source_file_name);
			printf("FUNCTION %-20s %s\n", s->name, s->source_file_name);
		}
		else if(s->type == SYMBOL_STRUCT){
			// printf("STRUCT %-20s %-20s %s\n", s->signature, s->name, s->source_file_name);
		}
    }
}
