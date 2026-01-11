#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

#include "symbol_extractor/directory_scanner.h"
#include "symbol_extractor/file_symbol_extractor.h"
#include "symbol_extractor/symbol_typedef.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// =========================
// Helper: Read file into string
// =========================
static char* read_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc((uint32_t)len + 1);
    if (!buffer) { fclose(f); return NULL; }
    fread(buffer, 1, (uint32_t)len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}

// =========================
// Curl write callback
// =========================
struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + real_size + 1);
    if (!ptr) return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;

    return real_size;
}

// =========================
// Send prompt + JSON to OpenAI GPT-4.1-nano
// =========================
static char* query_openai(const char* prompt, const char* json_codebase) {
    CURL* curl = curl_easy_init();
    if (!curl) return NULL;

    struct MemoryStruct chunk = {malloc(1), 0};

    // Build request JSON
    cJSON* messages = cJSON_CreateArray();
    cJSON* system_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(system_msg, "role", "system");
    cJSON_AddStringToObject(system_msg, "content", "You are a helpful code analysis assistant.");
    cJSON_AddItemToArray(messages, system_msg);

    cJSON* user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");

    char user_content[8192];
    snprintf(user_content, sizeof(user_content), "%s\n\nCodebase JSON:\n%s", prompt, json_codebase);
    cJSON_AddStringToObject(user_msg, "content", user_content);

    cJSON_AddItemToArray(messages, user_msg);

    cJSON* req = cJSON_CreateObject();
    cJSON_AddStringToObject(req, "model", "gpt-4.1-nano");
    cJSON_AddItemToObject(req, "messages", messages);

    char* req_str = cJSON_PrintUnformatted(req);
    cJSON_Delete(req);

	const char* api_key = getenv("OPENAI_API_KEY");
    if (!api_key) {
        fprintf(stderr, "Error: OPENAI_API_KEY is not set.\n");
		exit(1);
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
	char auth_header[512];
	snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
	headers = curl_slist_append(headers, auth_header);
    // headers = curl_slist_append(headers, "Authorization: Bearer YOUR_OPENAI_API_KEY");  // replace with key

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(req_str);
        curl_easy_cleanup(curl);
        free(chunk.memory);
        return NULL;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    free(req_str);

    return chunk.memory;  // caller must free
}

static void handle_response(const char* json_text) {
    cJSON *json = cJSON_Parse(json_text);
    if (!json) {
        fprintf(stderr, "Error parsing JSON!\n");
        return;
    }

    // choices array
    cJSON *choices = cJSON_GetObjectItem(json, "choices");
    if (!cJSON_IsArray(choices)) {
        fprintf(stderr, "No choices array found!\n");
        cJSON_Delete(json);
        return;
    }

    // take the first choice
    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    if (!first_choice) {
        fprintf(stderr, "Empty choices array!\n");
        cJSON_Delete(json);
        return;
    }

    // message object inside the first choice
    cJSON *message = cJSON_GetObjectItem(first_choice, "message");
    if (!cJSON_IsObject(message)) {
        fprintf(stderr, "No message object found!\n");
        cJSON_Delete(json);
        return;
    }

    // content string inside message
    cJSON *content = cJSON_GetObjectItem(message, "content");
    if (cJSON_IsString(content)) {
        printf("\n%s\n\n", content->valuestring);
    } else {
        fprintf(stderr, "No content string found!\n");
    }

    cJSON_Delete(json);
}

// =========================
// Main
// =========================
int main(int argc, char* argv[]) {
    // if (argc < 3 || strcmp(argv[1], "--chat") != 0) {
    //     printf("Usage: %s --chat <prompt>\n", argv[0]);
    //     return 1;
    // }
	if(argc < 2)
		return 1;

    SymbolTable table = {0};
    symbol_table_init(&table);
    scan_directory(argv[1], index_file, &table);
	generate_json(&table);
	puts("Codebase Abstract Syntax Tree generated.");
    symbol_table_free(&table);

    printf("Codebase chatbot ready. Type 'exit' to quit.\n");

	char* codebase_json = read_file("AST.json");
	while(1){
		printf(">>> ");
		char buffer[100];
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer) - 1] = '\0';
		if(strcmp(buffer, "exit") == 0)
			break;

		const char* prompt = buffer;

		if (!codebase_json) return 1;

		char* response = query_openai(prompt, codebase_json);

		handle_response(response);

		if (!response) {
			fprintf(stderr, "Failed to get response from OpenAI GPT-4.1-nano\n");
			return 1;
		}

		free(response);
	}
	free(codebase_json);
	remove("AST.json");
    return 0;
}
