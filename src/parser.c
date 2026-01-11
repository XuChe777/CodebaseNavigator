#include "parser.h"
#include "symbol_extractor/symbol_content_extractor.h"

#include <stdio.h>
#include <string.h>

static char *json_escape_string(const char *src, size_t len) {
    // Worst case: every char becomes \u00XX (6 bytes)
    size_t cap = len * 6 + 1;
    char *dst = malloc(cap);
    if (!dst) return NULL;

    char *p = dst;

    for (size_t i = 0; i < len; i++) {
        unsigned char c = src[i];
        switch (c) {
        case '\"': *p++ = '\\'; *p++ = '\"'; break;
        case '\\': *p++ = '\\'; *p++ = '\\'; break;
        case '\n': *p++ = '\\'; *p++ = 'n';  break;
        case '\r': *p++ = '\\'; *p++ = 'r';  break;
        case '\t': *p++ = '\\'; *p++ = 't';  break;
        case '\b': *p++ = '\\'; *p++ = 'b';  break;
        case '\f': *p++ = '\\'; *p++ = 'f';  break;
        default:
            if (c < 0x20) {
                p += sprintf(p, "\\u%04x", c);
            } else {
                *p++ = c;
            }
        }
    }

    *p = '\0';
    return dst;
}

void generate_json(SymbolTable* table_p)
{
    /** Example Format:
    * {
    *     "name": "parse_packet",
    *     "return_type": "int",
    *     "parameters": [
    *         {"type": "const uint8_t *", "name": "buf"},
    *         {"type": "size_t", "name": "len"}
    *     ],
    *     "calls": ["malloc", "memcpy"],
    *     "source": "int parse_packet(...) { ... }"
    * }
	*/

	FILE* file_p = fopen("AST.json", "w");

	if (file_p == NULL) {
        fprintf(stderr, "Error Occurred While creating a file !");
		return;
    }

	fputs("{\n", file_p);
	fputs("    \"files\": [\n", file_p);

    for (size_t i = 0; i < table_p->vector_of_files.size; i++)
    {
        FileAST* ast = table_p->vector_of_files.items[i];
		if(i == 0)
			fputs("        {\n", file_p);
		else
			fputs(",\n        {\n", file_p);
        fprintf(file_p, "            \"file name\": \"%s\",\n", ast->file_name);
        fputs("            \"symbols\": [\n", file_p);
        for (size_t j = 0; j < ast->symbols.size; j++)
        {
			if(j == 0)
				fputs("                {\n", file_p);
			else
				fputs(",\n                {\n", file_p);
            Symbol* symbol = ast->symbols.items[j];
            if (symbol->type == SYMBOL_FUNCTION)
            {
				fputs("                    \"type\":\"function\",\n", file_p);
				bool content_placed = false;
				for(size_t k = 0; k < number_of_function_properties_to_extract; k++){
					const char* property = function_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					if(content){
						char* parsed_content = json_escape_string(content, strlen(content));
						if(!content_placed)
							fprintf(file_p, "                    \"%s\": \"%s\"", property, parsed_content);
						else
							fprintf(file_p, ",\n                    \"%s\": \"%s\"", property, parsed_content);
						content_placed = true;
						free(parsed_content);
					}
				}
				fputs("\n", file_p);
            }
            else if (symbol->type == SYMBOL_STRUCT)
            {
				fputs("                    \"type\":\"struct\"\n", file_p);
				bool content_placed = false;
				for(size_t k = 0; k < number_of_struct_properties_to_extract; k++){
					const char* property = struct_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					if(content){
						char* parsed_content = json_escape_string(content, strlen(content));
						if(!content_placed)
							fprintf(file_p, "                    \"%s\": \"%s\"", property, parsed_content);
						else
							fprintf(file_p, ",\n                    \"%s\": \"%s\"", property, parsed_content);
						content_placed = true;
						free(parsed_content);
					}
				}
				fputs("\n", file_p);
            }
            else if (symbol->type == SYMBOL_ENUM)
            {
				fputs("                    \"type\":\"enum\"\n", file_p);
				for(size_t k = 1; k < number_of_enum_properties_to_extract; k++){
					const char* property = enum_properties_to_extract[k];
					const char* content = symbol_query_property(symbol, property);
					bool content_placed = false;
					if(content){
						char* parsed_content = json_escape_string(content, strlen(content));
						if(!content_placed)
							fprintf(file_p, "                    \"%s\": \"%s\"", property, parsed_content);
						else
							fprintf(file_p, ",\n                    \"%s\": \"%s\"", property, parsed_content);
						content_placed = true;
						free(parsed_content);
					}
				}
				fputs("\n", file_p);
            }
			fprintf(file_p, "                }");
        }
        fputs("\n            ]\n", file_p);
        fputs("        }", file_p);
    }

	fputs("\n    ]\n", file_p);
	fputs("}\n", file_p);

	fclose(file_p);
}
