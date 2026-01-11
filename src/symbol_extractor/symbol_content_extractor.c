#include "symbol_content_extractor.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <tree_sitter/api.h>

#include "symbol_typedef.h"

const char* function_properties_to_extract[] = {
	"name",
	"function_definition",
	"call_expression",
	"identifier",
	"comment"
};
const size_t number_of_function_properties_to_extract = sizeof(function_properties_to_extract) / sizeof(const char*);

const char* struct_properties_to_extract[] = {
	"name",
	"struct_specifier",
	"identifier",
	"comment"
};
const size_t number_of_struct_properties_to_extract = sizeof(struct_properties_to_extract) / sizeof(const char*);

const char* enum_properties_to_extract[] = {
	"name",
	"enum_specifier",
	"identifier",
	"comment"
};
const size_t number_of_enum_properties_to_extract = sizeof(enum_properties_to_extract) / sizeof(const char*);


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

void symbol_extract_info_as_function(Symbol* sym, TSNode func_node, const char* source)
{
	for(size_t i = 0; i < number_of_function_properties_to_extract; i++){
		const char* property_name = function_properties_to_extract[i];
		TSNode* node = find_node_by_name(&func_node, property_name);
		if(node)
			symbol_add_property(sym, property_name, slice_source(source, *node));
	}
}

void symbol_extract_info_as_struct(Symbol* sym, TSNode st_node, const char* source)
{
	for(size_t i = 0; i < number_of_struct_properties_to_extract; i++){
		const char* property_name = struct_properties_to_extract[i];
		TSNode* node = find_node_by_name(&st_node, property_name);
		if(node)
			symbol_add_property(sym, property_name, slice_source(source, *node));
	}
}

void symbol_extract_info_as_enum(Symbol* sym, TSNode en_node, const char* source){
	for(size_t i = 0; i < number_of_enum_properties_to_extract; i++){
		const char* property_name = enum_properties_to_extract[i];
		TSNode* node = find_node_by_name(&en_node, property_name);
		if(node)
			symbol_add_property(sym, property_name, slice_source(source, *node));
	}
}
