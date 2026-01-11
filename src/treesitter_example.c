#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

#include "symbol_extractor/directory_scanner.h"
#include "symbol_extractor/file_symbol_extractor.h"
#include "symbol_extractor/symbol_content_extractor.h"
#include "symbol_extractor/symbol_typedef.h"

int main(void)
{
    SymbolTable table = {0};
    symbol_table_init(&table);
    scan_directory("src", index_file, &table);

    for (size_t i = 0; i < table.vector_of_files.size; i++)
    {
        FileAST* ast = table.vector_of_files.items[i];
		puts("==============================================");
        printf("FILE %s\n", ast->file_name);
        for (size_t j = 0; j < ast->symbols.size; j++)
        {
            Symbol* symbol = ast->symbols.items[j];
            if (symbol->type == SYMBOL_FUNCTION)
            {
				puts("\nFUNCTION");
				for(size_t k = 0; k < number_of_function_properties_to_extract; k++){
					const char* property = function_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					if(content)
						printf("%-10s: %s\n", property, content);
				}
            }
            else if (symbol->type == SYMBOL_STRUCT)
            {
				puts("\nSTRUCT");
				for(size_t k = 0; k < number_of_struct_properties_to_extract; k++){
					const char* property = struct_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					if(content)
						printf("%-10s: %s\n", property, content);
				}
            }
            else if (symbol->type == SYMBOL_ENUM)
            {
                puts("\nENUM");
				for(size_t k = 1; k < number_of_enum_properties_to_extract; k++){
					const char* property = enum_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					if(content)
						printf("%-10s: %s\n", property, content);
				}
            }
        }
    }
    symbol_table_free(&table);
}
