#include <stdio.h>
#include <string.h>
#include <tree_sitter/api.h>

/* Provided by tree-sitter-c */
extern const TSLanguage* tree_sitter_c(void);

/* Simple recursive AST printer */
static void print_node(TSNode node, const char* source, int indent)
{
    for (int i = 0; i < indent; i++) printf("  ");

    uint32_t start = ts_node_start_byte(node);
    uint32_t end = ts_node_end_byte(node);

    printf("%s [%u, %u]", ts_node_type(node), start, end);

    if (ts_node_child_count(node) == 0)
    {
        printf(" -> \"%.*s\"", (int)(end - start), source + start);
    }

    printf("\n");

    uint32_t child_count = ts_node_child_count(node);
    for (uint32_t i = 0; i < child_count; i++)
    {
        print_node(ts_node_child(node, i), source, indent + 1);
    }
}

int main(void)
{
    const char* source_code =
        "int add(int a, int b) {\n"
        "    return a + b;\n"
        "}\n";

    /* Create parser */
    TSParser* parser = ts_parser_new();
    if (!parser)
    {
        fprintf(stderr, "Failed to create TSParser\n");
        return 1;
    }

    /* Set C language */
    if (!ts_parser_set_language(parser, tree_sitter_c()))
    {
        fprintf(stderr, "Failed to set C language\n");
        return 1;
    }

    /* Parse source */
    TSTree* tree = ts_parser_parse_string(parser, NULL, source_code, strlen(source_code));

    if (!tree)
    {
        fprintf(stderr, "Parse failed\n");
        return 1;
    }

    /* Get root node */
    TSNode root = ts_tree_root_node(tree);

    printf("=== AST ===\n");
    print_node(root, source_code, 0);

    /* Cleanup */
    ts_tree_delete(tree);
    ts_parser_delete(parser);

    return 0;
}
