/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/18 09:38:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/18 15:56:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

#define SRC_PATH "./src/"

typedef struct {
	char *fileName;
	char *result;
	int result_len;
} Category;

typedef struct {
	Category *cat;
} Data;

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        return -1; // Error opening files
    }

    // Get file sizes
    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);
    long size1 = ftell(f1);
    long size2 = ftell(f2);
    if (size1 != size2) {
        fclose(f1);
        fclose(f2);
        return 0; // Files are different
    }
	if (!size1) {
		fclose(f1);
		fclose(f2);
		return 1;
	}

    // Compare file contents
    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    unsigned char *buffer1 = malloc(size1);
    unsigned char *buffer2 = malloc(size2);
    fread(buffer1, 1, size1, f1);
    fread(buffer2, 1, size2, f2);

    int result = memcmp(buffer1, buffer2, size1);

    free(buffer1);
    free(buffer2);
    fclose(f1);
    fclose(f2);

    return result == 0 ? 1 : 0; // 1 if files are equal, 0 if different
}

int execute_with(char *bin, char *line, char *fileName, int test_number) {
	pid_t pid = vfork();

	if (!pid) {
		char input_file[PATH_MAX] = {0};
		char output_file[PATH_MAX] = {0};
		char error_file[PATH_MAX] = {0};

		sprintf(input_file, "./data/i_%s_%d", fileName, test_number);
		sprintf(output_file, "./data/%so_%s_%d", basename(bin), fileName, test_number);
		sprintf(error_file, "./data/%se_%s_%d", basename(bin), fileName, test_number);
		int fd_in = open(input_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		write(fd_in, line, strlen(line) + 1);
		lseek(fd_in, 0, 0);

		int fd_out = open(output_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		int fd_err = open(error_file, O_CREAT | O_TRUNC | O_RDWR, 00644);


		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		dup2(fd_err, STDERR_FILENO);

		close(fd_in);
		close(fd_out);
		close(fd_err);

		char *args[2] = { bin, NULL };

		if (execv(bin, args) == -1) {
			exit(1);
        }
	} else {
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}
	return -1;

}

typedef struct {
	char *partial;
	size_t partial_len;
	bool passed;
} UnitResult;

UnitResult unit_test(char *fileName, char *line, int test_number) {
	int exit_42sh = execute_with("./42sh", line, fileName, test_number);
	int exit_bash = execute_with("/bin/bash", line, fileName, test_number);

	char input[PATH_MAX] = {0};
	char out_42[PATH_MAX] = {0};
	char out_bash[PATH_MAX] = {0};
	char err_42[PATH_MAX] = {0};
	char err_bash[PATH_MAX] = {0};

	sprintf(input, "./data/i_%s_%d", fileName, test_number);
	sprintf(out_42, "./data/42sho_%s_%d", fileName, test_number);
	sprintf(out_bash, "./data/basho_%s_%d", fileName, test_number);
	sprintf(err_42, "./data/42she_%s_%d", fileName, test_number);
	sprintf(err_bash, "./data/bashe_%s_%d", fileName, test_number);

	bool output_ok = compare_files(out_42, out_bash);
	bool error_ok = compare_files(err_42, err_bash);
	bool exit_ok = (exit_42sh == exit_bash);

	char *partial_result = calloc(2048, sizeof(char));

	int len = sprintf(partial_result, "\t\t\t\t{\n\t\t\t\t\t\"id\": \"%d\",\n\t\t\t\t\t\"input\": \"%s\",\n\t\t\t\t\t\"42sh_output\": \"%s\",\n\t\t\t\t\t\"bash_output\": \"%s\",\n" \
		"\t\t\t\t\t\"42sh_error\": \"%s\",\n\t\t\t\t\t\"bash_error\": \"%s\",\n\t\t\t\t\t\"42sh_exit_code\": \"%d\",\n\t\t\t\t\t\"bash_exit_code\": \"%d\",\n" \
		"\t\t\t\t\t\"output_ok\": \"%d\",\n\t\t\t\t\t\"error_ok\": \"%d\",\n\t\t\t\t\t\"exit_ok\": \"%d\"\n\t\t\t\t},\n"
		 , test_number, input, out_42, out_bash, err_42, err_bash, exit_42sh, exit_bash, output_ok, error_ok, exit_ok);

	UnitResult ret = {
		.partial = partial_result,
		.partial_len = len,
		.passed = (output_ok && exit_ok),
	};

	return ret;
}

void *routine(void *cat) {
    Category *category = (Category *)cat;

    int file_fd = open(category->fileName, O_RDONLY);
    if (file_fd == -1) {
        perror("Failed to open file");
        return NULL;
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) == -1) {
        perror("Failed to get file stats");
        close(file_fd);
        return NULL;
    }

    char *mapped_file = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (mapped_file == MAP_FAILED) {
        perror("Failed to mmap file");
        close(file_fd);
        return NULL;
    }
    close(file_fd);

	char *category_basename = basename(category->fileName);

	category->result = calloc(1e6, sizeof(char));
	size_t offset = 0;

	offset = sprintf(category->result, "\t\t\t\"category_name\": \"%s\",\n\t\t\t\"tests\": [\n", category_basename);

	int test_number = 1;
	int passed_tests = 0;
	while (*mapped_file) {
		char *eol = strchr(mapped_file, '\n');
		
		char line[4096] = {0};
		ptrdiff_t line_size = eol - mapped_file;
		memcpy(line, mapped_file, line_size);
		line[line_size] = '\0';

		UnitResult partial = unit_test(category_basename, line, test_number);
		memcpy(category->result + offset, partial.partial, partial.partial_len);
		offset += partial.partial_len;

		mapped_file = eol + 1;

		passed_tests += (partial.passed);
		free(partial.partial);
		test_number++;
	}
	offset -= 2;
	memcpy(category->result + offset, "\n", 1);
	offset++;
	offset += sprintf(category->result + offset, "\t\t\t],\n\t\t\t\"passed_tests\": \"%d\"\n\t\t},\n\t\t{\n", passed_tests);
	category->result_len = offset;

	munmap(mapped_file, file_stat.st_size);
	dprintf(2, "leavin thread: %s\n", category->fileName);
    return 0;
}

int fill_dir(DIR *src_dir, struct dirent **dir_buffer) {
    struct dirent *it;
    int count = 0;

    while ((it = readdir(src_dir)) != NULL) {
        if (it->d_type != DT_DIR) {
            dir_buffer[count] = malloc(sizeof(struct dirent));
            memcpy(dir_buffer[count], it, sizeof(struct dirent));
            count++;
        }
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
int main(void) {

	//Count dir entries to allocate our structures
	DIR *src_dir = opendir(SRC_PATH);

	struct dirent **dir_buffer = calloc(100, sizeof(struct dirent *));
	int category_count = fill_dir(src_dir, dir_buffer); //return dir count
	sort_dir(dir_buffer, category_count);

	struct dirent *it;

	Data data = {
		.cat = calloc(category_count, sizeof(Category)),
	};

	pthread_t *threads = calloc(category_count, sizeof(pthread_t));

	int i = 0;
	while (i < 1000 && dir_buffer[i] != NULL) {
		it = dir_buffer[i];
		if (it->d_type != DT_DIR) {
			//Create a threead for each category in src/
			char buffer[PATH_MAX] = {0};
			snprintf(buffer, PATH_MAX, "%s%s", SRC_PATH, it->d_name);
			data.cat[i].fileName = strdup(buffer);

			pthread_create(&threads[i], NULL, routine, &data.cat[i]);
			i++;
        }
	}

	//Join all threads
	for (int i = 0; i < category_count; i++) {
		pthread_join(threads[i], NULL);
	}

	char *final_result = calloc(1e6, sizeof(char));
	int offset;

	offset = sprintf(final_result, "{\n\t\"categories\": [\n\t\t{\n");

	for (int i = 0; i < category_count; i++) {
		memcpy(final_result + offset, data.cat[i].result, data.cat[i].result_len);
		offset += data.cat[i].result_len;
	}

	offset -= 6;

	memcpy(final_result + offset, "\n\t]\n}\n", 5);
	offset += 5;

	int fd_out = open("./data/log.json", O_CREAT | O_TRUNC | O_WRONLY, 00644);
	if (fd_out == -1) {
		perror("failed");
	}

	write(fd_out, final_result, offset);
	close(fd_out);

	for (int i = 0; i < category_count; i++) {
		free(data.cat[i].result);
	}
	free(final_result);

	for (int i = 0; i < category_count; i++) {
		free(dir_buffer[i]);
	}
	free(dir_buffer);
	closedir(src_dir);
	return 0;
}
