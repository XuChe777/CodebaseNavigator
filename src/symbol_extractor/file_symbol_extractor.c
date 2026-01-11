#include "symbol_typedef.h"
#include "symbol_content_extractor.h"
#include "file_symbol_extractor.h"

#include <stdint.h>
#include <tree_sitter/api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const TSLanguage *tree_sitter_c(void);

/* Extract functions and structs from a single C source file */
void index_file(const char *path, void *user_data) {
	SymbolTable* table = user_data;

	/* read file */
    FILE *f = fopen(path, "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *source = malloc((unsigned long)size + 1);
    fread(source, 1, (unsigned long)size, f);
    source[size] = 0;
    fclose(f);

	symbol_table_add_file(table, path);

    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    TSTree *tree = ts_parser_parse_string(
        parser, NULL, source, (uint32_t)size);

    TSNode root = ts_tree_root_node(tree);
    TSTreeCursor cursor = ts_tree_cursor_new(root);

	bool backtracking = false;
	while(1){
		TSNode node = ts_tree_cursor_current_node(&cursor);
        const char *type = ts_node_type(node);

		if(backtracking){
			if(ts_tree_cursor_goto_next_sibling(&cursor))
				backtracking = false;
			else if(!ts_tree_cursor_goto_parent(&cursor))
				break;
		} else{
			/* Function */
            if (strcmp(type, "function_definition") == 0) {
				Symbol symbol = {0};
				symbol_init(&symbol, SYMBOL_FUNCTION, path, ts_node_start_point(node).row + 1, ts_node_end_point(node).row + 1);
				symbol_extract_info_as_function(&symbol, node, source);
				symbol_table_add_symbol(table, symbol);
            }
            /* Struct */
            else if (strcmp(type, "struct_specifier") == 0) {
				Symbol symbol = {0};
				symbol_init(&symbol, SYMBOL_STRUCT, path, ts_node_start_point(node).row + 1, ts_node_end_point(node).row + 1);
				symbol_extract_info_as_struct(&symbol, node, source);
				symbol_table_add_symbol(table, symbol);
            }
            /* Enum */
            else if (strcmp(type, "enum_specifier") == 0) {
				Symbol symbol = {0};
				symbol_init(&symbol, SYMBOL_ENUM, path, ts_node_start_point(node).row + 1, ts_node_end_point(node).row + 1);
				symbol_extract_info_as_enum(&symbol, node, source);
				symbol_table_add_symbol(table, symbol);
            }

			backtracking = !ts_tree_cursor_goto_first_child(&cursor);
		}
	}

    ts_tree_cursor_delete(&cursor);
    ts_tree_delete(tree);
    ts_parser_delete(parser);
	free(source);
}
