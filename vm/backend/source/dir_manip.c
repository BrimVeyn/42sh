#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int fill_dir(DIR *src_dir, struct dirent **dir_buffer) {
    struct dirent *it;
    int count = 0;

    while ((it = readdir(src_dir)) != NULL) {
		if (!strcmp(it->d_name, ".") || !strcmp(it->d_name, ".."))
			continue;

		dir_buffer[count] = malloc(sizeof(struct dirent));
		memcpy(dir_buffer[count], it, sizeof(struct dirent));
		count++;
    }

    return count;
}

void sort_dir(struct dirent **dir_buffer, int category_count) {
    for (int i = 0; i < category_count - 1; i++) {
        for (int j = i + 1; j < category_count; j++) {
            if (strcmp(dir_buffer[i]->d_name, dir_buffer[j]->d_name) > 0) {
                struct dirent *tmp = dir_buffer[i];
                dir_buffer[i] = dir_buffer[j];
                dir_buffer[j] = tmp;
            }
        }
    }
}

void print_dir(struct dirent **dir_buffer){
	for (int i = 0; dir_buffer[i]; i++){
		printf("name: %s\n", dir_buffer[i]->d_name);
	}
}
