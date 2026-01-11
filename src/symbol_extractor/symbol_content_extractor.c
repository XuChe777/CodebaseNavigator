#include "symbol_content_extractor.h"

#include <stdint.h>
#include <string.h>
#include <tree_sitter/api.h>

#include "symbol_typedef.h"

static char* slice_source(const char* src, TSNode node)
{
    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);
    uint32_t len = end - start;

    char* out = malloc(len * sizeof(char) + 1);
    memcpy(out, src + start, len);
    out[len] = '\0';
    return out;
}

static TSNode* find_node_by_name(TSNode* node, const char* name) {
    if (strcmp(ts_node_type(*node), name) == 0)
        return node;

    uint32_t n = ts_node_child_count(*node);
    for (uint32_t i = 0; i < n; i++) {
        TSNode child = ts_node_child(*node, i);
        TSNode* found = find_node_by_name(&child, name);
        if (found)
            return found;
    }
	return NULL;
}

void symbol_extract_function(Symbol* sym, TSNode func_node, const char* source, const char* file)
{
	TSNode* declarator = find_node_by_name(&func_node, "identifier");
    // TSNode declarator = ts_node_child_by_field_name(func_node, "declarator", 10);
    // if (ts_node_is_null(declarator)) declarator = ts_node_child(func_node, 1);

	TSNode* identifier_node = find_node_by_name(&func_node, "identifier");
	if(identifier_node)
		sym->name = slice_source(source, *identifier_node);

    sym->type = SYMBOL_FUNCTION;
    sym->signature = slice_source(source, func_node);
    sym->start_line = ts_node_start_point(func_node).row + 1;
    sym->end_line = ts_node_end_point(func_node).row + 1;
    sym->source_file_name = malloc(strlen(file) * sizeof(char) + 1);
    strcpy(sym->source_file_name, file);
}

void symbol_extract_struct(Symbol* sym, TSNode st_node, const char* source, const char* file)
{
    TSNode name_node = ts_node_child_by_field_name(st_node, "name", 4);
    if (ts_node_is_null(name_node)) name_node = ts_node_child(st_node, 1);

	TSNode* identifier_node = find_node_by_name(&st_node, "identifier");
	if(identifier_node)
		sym->name = slice_source(source, *identifier_node);

    sym->type = SYMBOL_STRUCT;
    sym->signature = slice_source(source, st_node);
    sym->start_line = ts_node_start_point(st_node).row + 1;
    sym->end_line = ts_node_end_point(st_node).row + 1;
    sym->source_file_name = malloc(strlen(file) * sizeof(char) + 1);
    strcpy(sym->source_file_name, file);
}
