#pragma once
#include <stddef.h>

// opens a file dialog and returns the selected file example
// filter_list example: "mp3,flac,wav,ogg" or NULL for all files
// returned string is dynamic (NULL if canceled), caller must free
char* file_dialog_open_file(const char* filter_list);

// opens a folder picker dialog and returns folder path
// returned string is dynamic (NULL if canceled), caller must free
char* file_dialog_open_folder();

// opens a multiple file selection dialog and returns array of paths
// out_count will contain the number of files selected
// returned string array is dynamic (NULL if canceled), caller must free
char** file_dialog_open_multiple(const char* filter_list, size_t* out_count);
