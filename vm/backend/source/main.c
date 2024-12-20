/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 09:38:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/19 15:32:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <linux/limits.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stddef.h>
#include <libgen.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "tester.h"

int total_tests = 0;
int total_passed = 0;
char **my_env = NULL;

char *my_basename(const char *path) {
    const char *slash = strrchr(path, '/'); // Find last '/'
    return slash ? (char *)(slash + 1) : (char *)path; // Return after last '/' or whole string
}

typedef struct {
	char *mapped_file;
	size_t mapped_size;
} MapResult;

MapResult open_and_map(char *filename) {
	MapResult ret = { NULL, 0 };

    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("Failed to open file");
		exit(1);
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) == -1) {
        perror("Failed to get file stats");
        close(file_fd);
		exit(1);
    }

    char *mapped_file = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (mapped_file == MAP_FAILED) {
        perror("Failed to mmap file");
        close(file_fd);
		exit(1);
    }
    close(file_fd);

	ret.mapped_file = mapped_file;
	ret.mapped_size = file_stat.st_size;

	return ret;
}

void *routine_script(void *cat) {
    Category *category = (Category *)cat;

	//mmap file and compute basename

	DIR *script_dir = opendir(category->fileName);
	if (!script_dir) { perror("dir"); exit(1); }

	struct dirent **dir_buffer = calloc(100, sizeof(struct dirent *));
	int file_count = fill_dir(script_dir, dir_buffer);
	sort_dir(dir_buffer, file_count);

	category->result = calloc(1e6, sizeof(char));
	size_t offset = 0;

	//category header
	offset = sprintf(category->result, "\t\t\t\"category_name\": \"scripts\",\n\t\t\t\"tests\": [\n");

	struct dirent *it;
	int test_number = 1;
	int passed_tests = 0;

	for (int i = 0; (it = dir_buffer[i]) != NULL; i++) {
		char full_filename[PATH_MAX] = {0};
		sprintf(full_filename, "%s/%s", category->fileName, it->d_name);
		// dprintf(2, "it->dname: %s\n", full_filename);

		UnitResult partial = script_test(full_filename, test_number);
		memcpy(category->result + offset, partial.partial, partial.partial_len);
		offset += partial.partial_len;
		//
		passed_tests += (partial.passed);
		free(partial.partial);
		test_number++;
	}

	//Removing trailing , (love JSON)
	offset -= 2;
	memcpy(category->result + offset, "\n", 1);
	offset++;
	//--------
	
	//end of category
	offset += sprintf(category->result + offset, "\t\t\t],\n\t\t\t\"passed_tests\": \"%d\"\n\t\t},\n\t\t{\n", passed_tests);

	//update total passed and total ran tests
	total_passed += passed_tests;
	total_tests += (test_number - 1);

	//save len to avoid computing it again since it can be large
	category->result_len = offset;

    return 0;
}

void *routine_unit(void *cat) {
    Category *category = (Category *)cat;

	//mmap file and compute basename
	MapResult ret = open_and_map(category->fileName);
	char *mapped_file = ret.mapped_file;
	char *category_basename = my_basename(category->fileName);

	//allocated 1M. We know its not gonna be over 1M. Avoid reallocation
	category->result = calloc(1e6, sizeof(char));
	size_t offset = 0;

	//category header
	offset = sprintf(category->result, "\t\t\t\"category_name\": \"%s\",\n\t\t\t\"tests\": [\n", category_basename);

	int test_number = 1;
	int passed_tests = 0;

	//reading line one by one and running unit test
	while (*mapped_file) {
		char *eol = strchr(mapped_file, '\n');
		
		char line[4096] = {0};
		ptrdiff_t line_size = eol - mapped_file;
		memcpy(line, mapped_file, line_size);
		line[line_size] = '\0';

		// dprintf(2, "line: %s\n", line);
		UnitResult partial = unit_test(category_basename, line, test_number);
		memcpy(category->result + offset, partial.partial, partial.partial_len);
		offset += partial.partial_len;
		//
		mapped_file = eol + 1;
		//
		passed_tests += (partial.passed);
		free(partial.partial);
		test_number++;
	}
	//Removing trailing , (love JSON)
	offset -= 2;
	memcpy(category->result + offset, "\n", 1);
	offset++;
	//--------
	
	//end of category
	offset += sprintf(category->result + offset, "\t\t\t],\n\t\t\t\"passed_tests\": \"%d\"\n\t\t},\n\t\t{\n", passed_tests);

	//update total passed and total ran tests
	total_passed += passed_tests;
	total_tests += (test_number - 1);

	//save len to avoid computing it again since it can be large
	category->result_len = offset;
	munmap(mapped_file, ret.mapped_size);

	dprintf(2, "leavin thread: %s\n", category->fileName);
    return 0;
}

void setup_env(void) {
    char **base_env = __environ;
    size_t env_size = 0;
    for (size_t i = 0; base_env[i]; i++)
        env_size++;

    my_env = calloc(env_size + 2, sizeof(char *));

    size_t i;
    for (i = 0; base_env[i]; i++) {
        my_env[i] = strdup(base_env[i]);
    }
    my_env[i++] = strdup("LC_COLLATE=C");
    my_env[i] = 0;
}

int main(void) {

	//Count dir entries to allocate our structures
	DIR *src_dir = opendir(SRC_PATH);
	if (!src_dir) {
		perror("dir");
		exit(1);
	}

	//Count source files, sort them by alphabetical order
	struct dirent **dir_buffer = calloc(100, sizeof(struct dirent *));
	int category_count = fill_dir(src_dir, dir_buffer); //return dir count
	sort_dir(dir_buffer, category_count);

	struct dirent *it;

	//Allocate N categories and N threads
	Category *cat = calloc(category_count, sizeof(Category));
	pthread_t *threads = calloc(category_count, sizeof(pthread_t));

    //Set LC_COLLATE="C" <-- posix standard sorting for file expansions
    setup_env();

	int i = 0;
	while (i < 1000 && dir_buffer[i] != NULL) {
		it = dir_buffer[i];
		if (it->d_type != DT_DIR) {
			// i++;
			// continue;
			//Create a thread for each category in src/
			char filename[PATH_MAX] = {0};
			snprintf(filename, PATH_MAX, "%s%s", SRC_PATH, it->d_name);
			cat[i].fileName = strdup(filename);

			pthread_create(&threads[i], NULL, routine_unit, &cat[i]);
        } else {
			char dirname[PATH_MAX] = {0};
			snprintf(dirname, PATH_MAX, "%s%s", SRC_PATH, it->d_name);
			cat[i].fileName = strdup(dirname);

			pthread_create(&threads[i], NULL, routine_script, &cat[i]);
		}
		i++;
	}

	//Join all threads
	for (int i = 0; i < category_count; i++) {
		pthread_join(threads[i], NULL);
	}

	//Join all category result to form the output JSON file
	char *final_result = calloc(1e6, sizeof(char));
	int offset;

	offset = sprintf(final_result, "{\n\t\"categories\": [\n\t\t{\n");

	for (int i = 0; i < category_count; i++) {
		memcpy(final_result + offset, cat[i].result, cat[i].result_len);
		offset += cat[i].result_len;
	}
	//-----------------------------------------------------
	
	//Remove trainling comma (again)
	offset -= 6;

	//JSON footer
	offset += sprintf(final_result + offset, "\n\t],\n\t\"total_tests\": \"%d\",\n\t\"total_passed\": \"%d\"\n}\n", total_tests, total_passed);

	//Write datas in log.json
	int fd_out = open("./data/log.json", O_CREAT | O_TRUNC | O_WRONLY, 00644);
	if (fd_out == -1) {
		perror("failed");
	}

	write(fd_out, final_result, offset);
	close(fd_out);

	for (int i = 0; i < category_count; i++) {
		free(cat[i].result);
	}
	free(final_result);

	for (int i = 0; i < category_count; i++) {
		free(dir_buffer[i]);
	}
	free(dir_buffer);
	closedir(src_dir);
	return 0;
}
