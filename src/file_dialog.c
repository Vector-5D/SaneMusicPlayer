#include "file_dialog.h"
#include "logger.h"
#include "nfd.h"
#include <stdlib.h>
#include <string.h>

char* file_dialog_open_file(const char* filter_list) {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_OpenDialog(filter_list, NULL, &out_path);

    if (result == NFD_OKAY) {
        char* path = strdup(out_path);
        free(out_path);
        LOG_INFO("File selected: %s", path);
        return path;
    } else if (result == NFD_CANCEL) {
        LOG_INFO("User cancelled file dialog.");
    } else {
        LOG_ERROR("File dialog server: %s", NFD_GetError());
    }

    return NULL;
}

char* file_dialog_open_folder() {
    nfdchar_t* out_path = NULL;
    nfdresult_t result = NFD_PickFolder(NULL, &out_path);

    if (result == NFD_OKAY) {
        char* path = strdup(out_path);
        free(out_path);
        LOG_INFO("Folder selected: %s", path);
        return path;
    } else if (result == NFD_CANCEL) {
        LOG_INFO("User cancelled folder dialog.");
    } else {
        LOG_ERROR("Folder dialog error: %s", NFD_GetError());
    }

    return NULL;
}

char** file_dialog_open_multiple(const char* filter_list, size_t* out_count) {
    nfdpathset_t path_set;
    nfdresult_t result = NFD_OpenDialogMultiple(filter_list, NULL, &path_set);

    if (result == NFD_OKAY) {
        size_t count = NFD_PathSet_GetCount(&path_set);
        char** paths = malloc(count * sizeof(char*));

        if (!paths) {
            LOG_ERROR("Memory allocation failed for multiple file paths.");
            NFD_PathSet_Free(&path_set);
            return NULL;
        }

        for (size_t i = 0; i < count; i++) {
            nfdchar_t* path = NFD_PathSet_GetPath(&path_set, i);
            paths[i] = strdup(path);
        }

        NFD_PathSet_Free(&path_set);
        *out_count = count;
        LOG_INFO("Selected %zu files", count);
        return paths;
    } else if (result == NFD_CANCEL) {
        LOG_INFO("User cancelled multiple file dialog.");
    } else {
        LOG_ERROR("Multiple file dialog error: %s", NFD_GetError());
    }

    *out_count = 0;
    return NULL;
}
