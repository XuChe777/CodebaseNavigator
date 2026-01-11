#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

extern const TSLanguage *tree_sitter_c(void);

typedef struct
{
    char* type;
    char* name;  // function or struct name
    char* signature;
    char* source_file;
    uint32_t start_byte;
    uint32_t end_byte;
    uint32_t start_line;
    uint32_t end_line;
} Symbol;

typedef struct
{
    Symbol* data;
    size_t size;
    size_t capacity;
} SymbolTable;

void init_symbol_table(SymbolTable *table) {
    table->data = NULL;
    table->size = 0;
    table->capacity = 0;
}

void push_symbol(SymbolTable *table, const char *name, const char *type,
                 uint32_t start_line, uint32_t end_line,
                 const char *source_file) {
    if (table->size == table->capacity) {
        table->capacity = table->capacity ? table->capacity * 2 : 8;
        table->data = realloc(table->data, table->capacity * sizeof(Symbol));
    }
	table->data[table->size].name = malloc(strlen(name));
	strcpy(table->data[table->size].name, name);
	table->data[table->size].type = malloc(strlen(type));
	strcpy(table->data[table->size].type, type);
    table->data[table->size].start_line = start_line;
    table->data[table->size].end_line = end_line;
    table->data[table->size].source_file = source_file;
    table->size++;
}

void free_symbol_table(SymbolTable *table) {
    for (size_t i = 0; i < table->size; i++) {
        free(table->data[i].name);
        free(table->data[i].type);
    }
    free(table->data);
}

/* Get node text safely */
static void node_text(TSNode node, const char *source, char *buffer, size_t bufsize) {
    uint32_t start = ts_node_start_byte(node);
    uint32_t end   = ts_node_end_byte(node);
    size_t len = (size_t)(end - start);
    if (len >= bufsize) len = bufsize - 1;
    memcpy(buffer, source + start, len);
    buffer[len] = '\0';
}

/* Extract functions and structs from a single C source file */
void extract_c_symbols(SymbolTable *table, const char *source, const char *filename) {
    TSParser *parser = ts_parser_new();
    ts_parser_set_language(parser, tree_sitter_c());

    TSTree *tree = ts_parser_parse_string(parser, NULL, source, strlen(source));
    TSNode root = ts_tree_root_node(tree);
    TSTreeCursor cursor = ts_tree_cursor_new(root);

	bool backtracking = false;
	while(1){
		TSNode node = ts_tree_cursor_current_node(&cursor);
        const char *type = ts_node_type(node);
		char name_buffer[128] = {0};
		if(backtracking){
			if(ts_tree_cursor_goto_next_sibling(&cursor))
				backtracking = false;
			else if(!ts_tree_cursor_goto_parent(&cursor))
				break;
		} else{
			/* Function */
            if (strcmp(type, "function_definition") == 0) {
                TSNode decl = ts_node_child_by_field_name(node, "declarator", 9);
                if (ts_node_is_null(decl)) decl = ts_node_child(node, 1);
                node_text(decl, source, name_buffer, sizeof(name_buffer));
                push_symbol(table, name_buffer, "function",
                            ts_node_start_point(node).row + 1,
                            ts_node_end_point(node).row + 1,
                            filename);
            }
            /* Struct */
            else if (strcmp(type, "struct_specifier") == 0) {
                TSNode name_node = ts_node_child_by_field_name(node, "name", 4);
                if (!ts_node_is_null(name_node)) {
                    node_text(name_node, source, name_buffer, sizeof(name_buffer));
                    push_symbol(table, name_buffer, "struct",
                                ts_node_start_point(node).row + 1,
                                ts_node_end_point(node).row + 1,
                                filename);
                }
            }

			backtracking = !ts_tree_cursor_goto_first_child(&cursor);
		}
	}

    ts_tree_cursor_delete(&cursor);
    ts_tree_delete(tree);
    ts_parser_delete(parser);
}

int main(void) {
    SymbolTable table;
    init_symbol_table(&table);

    const char *file1 = "struct Point { int x; int y; };\n"
                        "int add(int a, int b) { return a + b; }";

    const char *file2 = "struct Circle { int r; };\n"
                        "void draw_circle(struct Circle c) {}";

    extract_c_symbols(&table, file1, "file1.c");
    extract_c_symbols(&table, file2, "file2.c");

    printf("=== C Symbol Table ===\n");
    for (size_t i = 0; i < table.size; i++) {
        printf("[%s] %s (%s:%u-%u)\n",
               table.data[i].type,
               table.data[i].name,
               table.data[i].source_file,
               table.data[i].start_line,
               table.data[i].end_line);
    }

    free_symbol_table(&table);
    return 0;
}
