#include <limits.h>
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

int execute_unit(char *bin, char *line, char *fileName, int test_number) {

	pid_t pid = fork();

	if (!pid) {
		char input_file[PATH_MAX] = {0};
		char output_file[PATH_MAX] = {0};
		char error_file[PATH_MAX] = {0};

		memset(input_file, 0, PATH_MAX);
		memset(output_file, 0, PATH_MAX);
		memset(error_file, 0, PATH_MAX);

		snprintf(input_file, PATH_MAX, "./data/i_%s_%d", fileName, test_number);
		snprintf(output_file, PATH_MAX, "./data/%so_%s_%d", my_basename(bin), fileName, test_number);
		snprintf(error_file, PATH_MAX, "./data/%se_%s_%d", my_basename(bin), fileName, test_number);

		int fd_in = open(input_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		write(fd_in, line, strlen(line));
		write(fd_in, "\n", 1);
		lseek(fd_in, 0, 0);

		int fd_out = open(output_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		int fd_err = open(error_file, O_CREAT | O_TRUNC | O_RDWR, 00644);

		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		dup2(fd_err, STDERR_FILENO);

		close(fd_in);
		close(fd_out);
		close(fd_err);

		if (*bin == 'b') {
			char *args[3] = { bin, "--posix", NULL };
			if (execve(bin, args, my_env) == -1) {
				exit(1);
			}
		} else {
			char *args[2] = { bin, NULL };
			if (execve(bin, args, my_env) == -1) {
				exit(1);
			}
		}

	} else {
		int status;
		waitpid(pid, &status, 0);
		return WEXITSTATUS(status);
	}
	return -1;

}

char *create_out_dir(char *line, char *outDirName) {
	char buffer[2048] = {0};
	int match = 0;
	char *line_cpy = strdup(line);
	char *saved_ptr = line_cpy;

	while ((match = ft_strstr(line_cpy, "XXXOUTXXX")) != -1) {
		int postMatchPos = match + 9;
		char *before = ft_substr(line_cpy, 0, match);
		char *after = ft_substr(line_cpy, postMatchPos, strlen(line_cpy) - postMatchPos);
		sprintf(buffer, "%s%s%s", before, outDirName, after);
		free(before);
		free(after);
		line_cpy = buffer;
	}
	mkdir(outDirName, S_IRWXU);
	free(saved_ptr);
	return strdup(buffer);
}

bool compare_dirs(char *path_lhs, char *path_rhs) {

	DIR *lDir = opendir(path_lhs);
	struct dirent *it;

	while ((it = readdir(lDir)) != NULL) {
		if (it->d_type == DT_DIR) 
			continue;

		char fileNameLhs[2048] = {0};
		char fileNameRhs[2048] = {0};
		sprintf(fileNameLhs, "%s/%s", path_lhs, it->d_name);
		sprintf(fileNameRhs, "%s/%s", path_rhs, it->d_name);

		int result = compare_files(fileNameLhs, fileNameRhs);
		// dprintf(2, "result: %d\n", result);
		if (result <= 0)
			return false;
	}
	return true;
}


void ft_rmdir(char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }
    struct dirent *it;
    char fullpath[2048];

    while ((it = readdir(dir)) != NULL) {
        // Skip the special entries "." and ".."
        if (strcmp(it->d_name, ".") == 0 || strcmp(it->d_name, "..") == 0) {
            continue;
        }
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, it->d_name);
        if (it->d_type == DT_DIR) {
            ft_rmdir(fullpath);
        } else {
			// dprintf(2, "PATH: %s\n", fullpath);
            if (unlink(fullpath) == -1) {
                perror("unlink");
            }
        }
    }
    closedir(dir);

    // Remove the directory itself
    if (rmdir(path) == -1) {
		return ;
	}
}

void copy_outfiles(char *path, char *dest, bool cmp) {
	int fd_out = open(dest, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
	if (!cmp) {
		close(fd_out);
		return ;
	}

	DIR *dir = opendir(path);
	struct dirent *it;
	
	while ((it = readdir(dir)) != NULL) {
		if (it->d_type == DT_DIR)
			continue;

		char fullpath[2048] = {0};
		sprintf(fullpath, "%s/%s", path, it->d_name);

		int fd_in = open(fullpath, O_RDONLY, S_IRUSR);
		char *buffer = read_whole_file(fd_in);

		write(fd_out, buffer, strlen(buffer));
		free(buffer);
		close(fd_in);
	}
	close(fd_out);
}


UnitResult unit_test(char *fileName, char *line, int test_number) {

	char *line_42sh = line;
	char *line_bash = line;
	char *dir_42sh = NULL;
	char *dir_bash = NULL;
	bool outfilesCmp = false;

	if (ft_strstr(line, "XXXOUTXXX") != -1) {
		outfilesCmp = true;

		char outDirName[1024] = {0};

		sprintf(outDirName, "%s_%d_%s", fileName, test_number, "42sh");
		dir_42sh = strdup(outDirName);
		line_42sh = create_out_dir(line, outDirName);

		sprintf(outDirName, "%s_%d_%s", fileName, test_number, "bash");
		dir_bash = strdup(outDirName);
		line_bash = create_out_dir(line, outDirName);
	}

	// dprintf(2, "executing: %s\n", line_42sh);
	int exit_42sh = execute_unit("./42sh", line_42sh, fileName, test_number);
	int exit_bash = execute_unit("/bin/bash", line_bash, fileName, test_number);

	char input[PATH_MAX] = {0};
	char out_42[PATH_MAX] = {0};
	char out_bash[PATH_MAX] = {0};
	char err_42[PATH_MAX] = {0};
	char err_bash[PATH_MAX] = {0};
	char file_42[PATH_MAX] = {0};
	char file_bash[PATH_MAX] = {0};

	snprintf(input, PATH_MAX, "./data/i_%s_%d", fileName, test_number);
	snprintf(out_42, PATH_MAX, "./data/42sho_%s_%d", fileName, test_number);
	snprintf(out_bash, PATH_MAX, "./data/basho_%s_%d", fileName, test_number);
	snprintf(err_42, PATH_MAX, "./data/42she_%s_%d", fileName, test_number);
	snprintf(err_bash, PATH_MAX, "./data/bashe_%s_%d", fileName, test_number);
	snprintf(file_42, PATH_MAX, "./data/42shf_%s_%d", fileName, test_number);
	snprintf(file_bash, PATH_MAX, "./data/bashf_%s_%d", fileName, test_number);

	copy_outfiles(dir_42sh, file_42, outfilesCmp);
	copy_outfiles(dir_bash, file_bash, outfilesCmp);

	bool output_ok = compare_files(out_42, out_bash);
	bool error_ok = compare_files(err_42, err_bash);
	bool exit_ok = (exit_42sh == exit_bash);
	int files_ok = (outfilesCmp) ? compare_dirs(dir_bash, dir_42sh) : -1;

	if (outfilesCmp) {
		ft_rmdir(dir_42sh);
		ft_rmdir(dir_bash);
		free(dir_bash);
		free(dir_42sh);
	}

	char *partial_result = calloc(2048, sizeof(char));

	int len = sprintf(partial_result, "\t\t\t\t{\n\t\t\t\t\t\"id\": \"%d\",\n\t\t\t\t\t\"input\": \"%s\",\n" \
		"\t\t\t\t\t\"42sh_output\": \"%s\",\n\t\t\t\t\t\"bash_output\": \"%s\",\n" \
		"\t\t\t\t\t\"42sh_error\": \"%s\",\n\t\t\t\t\t\"bash_error\": \"%s\",\n" \
	    "\t\t\t\t\t\"42sh_files\": \"%s\",\n\t\t\t\t\t\"bash_files\": \"%s\",\n" \
		"\t\t\t\t\t\"42sh_exit_code\": \"%d\",\n\t\t\t\t\t\"bash_exit_code\": \"%d\",\n" \
		"\t\t\t\t\t\"output_ok\": \"%d\",\n\t\t\t\t\t\"error_ok\": \"%d\",\n" \
		"\t\t\t\t\t\"files_ok\": \"%d\",\n\t\t\t\t\t\"exit_ok\": \"%d\"\n\t\t\t\t},\n"
		 , test_number, input, out_42, out_bash, err_42, err_bash, file_42, file_bash, exit_42sh, exit_bash, output_ok, error_ok, files_ok, exit_ok);
	if (len == -1) {
		perror("sprintf");
		exit(1);
	}

	UnitResult ret = {
		.partial = partial_result,
		.partial_len = len,
		.passed = (output_ok && exit_ok),
	};

	return ret;
}
