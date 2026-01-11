#ifndef DIRECTORY_SCANNER_H
#define DIRECTORY_SCANNER_H
typedef void (*file_callback_t)(const char *path, void *user_data);
void scan_directory(const char *dir, file_callback_t cb, void *user_data);

#endif // DIRECTORY_SCANNER_H
