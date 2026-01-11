#include "directory_scanner.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

static int is_c_file(const char *name) {
    const char *dot = strrchr(name, '.');
    return dot && (!strcmp(dot, ".c") || !strcmp(dot, ".h"));
}

static int is_ignored_dir(const char *name) {
    return !strcmp(name, ".") ||
           !strcmp(name, "..") ||
           !strcmp(name, ".git") ||
           !strcmp(name, "build");
}

void scan_directory(const char *dir, file_callback_t user_call_back, void *user_data) {
    DIR *d = opendir(dir);
    if (!d) return;

    struct dirent *ent;
    while ((ent = readdir(d))) {
        if (is_ignored_dir(ent->d_name))
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);

        struct stat st;
        if (stat(path, &st) != 0)
            continue;

        if (S_ISDIR(st.st_mode)) {
            scan_directory(path, user_call_back, user_data);
        } else if (S_ISREG(st.st_mode) && is_c_file(ent->d_name)) {
            user_call_back(path, user_data);
        }
    }

    closedir(d);
}
