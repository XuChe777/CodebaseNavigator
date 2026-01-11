#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

#include "symbol_extractor/directory_scanner.h"
#include "symbol_extractor/file_symbol_extractor.h"
#include "symbol_extractor/symbol_content_extractor.h"
#include "symbol_extractor/symbol_typedef.h"
#include "parser.h"

int main(void)
{
    SymbolTable table = {0};
    symbol_table_init(&table);
    scan_directory("src", index_file, &table);
	generate_json(&table);
	puts("file generated");
    symbol_table_free(&table);
}
