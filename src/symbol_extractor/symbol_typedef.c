#include "symbol_typedef.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void idx_trie_init(IdxTrieNode* root){
	for(size_t i = 0; i < 128; i++){
		root->children[i] = NULL;
	}
	root->identity = 0;
	root->is_end_of_word = true;
}
void idx_trie_free(IdxTrieNode* root){
	for(size_t i = 0; i < 128; i++)
		if(root->children[i]){
			idx_trie_free(root->children[i]);
			free(root->children[i]);
		}
}
IdxTrieNode* idx_trie_query(IdxTrieNode* root, const char* key){
	IdxTrieNode *current = root;
    for (size_t i = 0; i < strlen(key); i++) {
		IdxTrieNode* child = current->children[(size_t)key[i]];
        if (!child) {
            return NULL;
        }
        current = child;
    }
	if(current->is_end_of_word)
		return current;
    return NULL;
}
void idx_trie_push(IdxTrieNode* root, const char* key, size_t value){
	IdxTrieNode* current = root;
	for(size_t i = 0; i < strlen(key); i++){
		if (!current->children[(size_t)key[i]]){
			current->children[(size_t)key[i]] = malloc(sizeof(IdxTrieNode));
			idx_trie_init(current->children[(size_t)key[i]]);
			current->children[(size_t)key[i]]->is_end_of_word = false;
		}
		current = current->children[(size_t)key[i]];
	}
	current->is_end_of_word = true;
	current->identity = value;
}

void vector_init(Vector* vector){
	vector->items = malloc(8 * sizeof(void*));
	vector->size = 0;
	vector->capacity = 8;
}
void vector_free(Vector* vector){
	for(uint32_t i = 0; i < vector->size; i++)
		free(vector->items[i]);
	free(vector->items);
}
void vector_push_item(Vector* vector, void* item, size_t item_size){
    if (vector->size == vector->capacity) {
        vector->capacity = vector->capacity ? vector->capacity * 2 : 8;
        vector->items = realloc(vector->items, vector->capacity * sizeof(void*));
    }
	vector->items[vector->size] = malloc(item_size);
	memcpy(vector->items[vector->size], item, item_size);
	vector->size++;
}

void symbol_init(Symbol* symbol, SymbolType type, const char* source_file_name, uint32_t start_line, uint32_t end_line){
	symbol->type = type;
	vector_init(&symbol->properties);
	idx_trie_init(&symbol->property_index_trie_root);
	symbol->source_file_name = malloc(strlen(source_file_name) * sizeof(char) + 1);
	strcpy(symbol->source_file_name, source_file_name);
	symbol->start_line = start_line;
	symbol->end_line = end_line;
}
void symbol_free(Symbol* symbol){
	vector_free(&symbol->properties);
	idx_trie_free(&symbol->property_index_trie_root);
	free(symbol->source_file_name);
}
void symbol_add_property(Symbol* symbol, const char* property_name, const char* content){
	vector_push_item(&symbol->properties, (void*)content, strlen(content) * sizeof(char) + 1);
	idx_trie_push(&symbol->property_index_trie_root, property_name, symbol->properties.size - 1);
}
char* symbol_query_property(Symbol* symbol, const char* property_name){
	IdxTrieNode* node = idx_trie_query(&symbol->property_index_trie_root, property_name);
	if(node)
		return symbol->properties.items[node->identity];
	return NULL;
}

void symbol_table_init(SymbolTable *table) {
	vector_init(&table->vector_of_files);
}
void symbol_table_free(SymbolTable *table) {
	for(size_t file_idx = 0; file_idx < table->vector_of_files.size; file_idx++){
		FileAST* ast = table->vector_of_files.items[file_idx];
		free(ast->file_name);
		for(size_t symbol_idx = 0; symbol_idx < ast->symbols.size; symbol_idx++)
			symbol_free(ast->symbols.items[symbol_idx]);
		vector_free(&ast->symbols);
	}
	vector_free(&table->vector_of_files);
}
void symbol_table_add_file(SymbolTable *table, const char* file_name){
	FileAST new_ast;
	new_ast.file_name = malloc(strlen(file_name) * sizeof(char) + 1);
	strcpy(new_ast.file_name, file_name);
	vector_init(&new_ast.symbols);
	vector_push_item(&table->vector_of_files, &new_ast, sizeof(new_ast));
}
void symbol_table_add_symbol(SymbolTable *table, Symbol symbol) {
	Vector* symbols_vector_p = &((FileAST*)table->vector_of_files.items[table->vector_of_files.size - 1])->symbols;
	vector_push_item(symbols_vector_p, &symbol, sizeof(Symbol));
}
