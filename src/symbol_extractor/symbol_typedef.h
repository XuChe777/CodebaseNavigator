#ifndef SYMBOL_TYPEDEF_H
#define SYMBOL_TYPEDEF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    SYMBOL_FUNCTION,
    SYMBOL_STRUCT,
	SYMBOL_ENUM
} SymbolType;

typedef struct idx_trie_node_st{
	struct idx_trie_node_st *children[128];
	size_t identity;
	bool is_end_of_word;
} IdxTrieNode;

extern void idx_trie_init(IdxTrieNode* root);
extern void idx_trie_free(IdxTrieNode* root);
extern IdxTrieNode* idx_trie_query(IdxTrieNode* root, const char* key);
extern void idx_trie_push(IdxTrieNode* root, const char* key, size_t value);

typedef struct vector_st{
	void** items;
    size_t size;
    size_t capacity;
} Vector;

extern void vector_init(Vector* vector);
extern void vector_free(Vector* vector);
extern void vector_push_item(Vector* vector, void* item, size_t item_size);

typedef struct symbol_st
{
    SymbolType type;
	Vector properties;
	IdxTrieNode property_index_trie_root;
    char* source_file_name;
    uint32_t start_line;
    uint32_t end_line;
} Symbol;

extern void symbol_init(Symbol* symbol, SymbolType type, const char* source_file_name, uint32_t start_line, uint32_t end_line);
extern void symbol_free(Symbol* symbol);
extern void symbol_add_property(Symbol* symbol, const char* property_name, const char* content);
extern char* symbol_query_property(Symbol* symbol, const char* property_name);

typedef struct file_ast_st{
	char* file_name;
	Vector symbols;
} FileAST;

typedef struct
{
	Vector vector_of_files;
} SymbolTable;

extern void symbol_table_init(SymbolTable* table);
extern void symbol_table_free(SymbolTable* table);
extern void symbol_table_add_file(SymbolTable *table, const char* file_name);
extern void symbol_table_add_symbol(SymbolTable* table, Symbol symbol);

#endif  // SYMBOL_TYPEDEF_H
