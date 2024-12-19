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

int execute_script(char *bin, char *fileName, int test_number) {

	pid_t pid = fork();

	if (!pid) {
		char output_file[PATH_MAX] = {0};
		char error_file[PATH_MAX] = {0};

		memset(output_file, 0, PATH_MAX);
		memset(error_file, 0, PATH_MAX);

		snprintf(output_file, PATH_MAX, "./data/%so_%s_%d", my_basename(bin), my_basename(fileName), test_number);
		snprintf(error_file, PATH_MAX, "./data/%se_%s_%d", my_basename(bin), my_basename(fileName), test_number);

		int fd_out = open(output_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		int fd_err = open(error_file, O_CREAT | O_TRUNC | O_RDWR, 00644);
		int fd_in = open("/dev/null", O_RDWR);

		dup2(fd_out, STDOUT_FILENO);
		dup2(fd_err, STDERR_FILENO);
		dup2(fd_in, STDIN_FILENO);

		close(fd_out);
		close(fd_err);
		close(fd_in);

		char *args[3] = { bin, fileName, NULL };
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

UnitResult script_test(char *fileName, int test_number) {

	int exit_42sh = execute_script("./42sh", fileName, test_number);
	int exit_bash = execute_script("/bin/bash", fileName, test_number);

	char out_42[PATH_MAX] = {0};
	char out_bash[PATH_MAX] = {0};
	char err_42[PATH_MAX] = {0};
	char err_bash[PATH_MAX] = {0};

	memset(out_42, 0, PATH_MAX);
	memset(out_bash, 0, PATH_MAX);
	memset(err_42, 0, PATH_MAX);
	memset(err_bash, 0, PATH_MAX);

	snprintf(out_42, PATH_MAX, "./data/42sho_%s_%d", my_basename(fileName), test_number);
	snprintf(out_bash, PATH_MAX, "./data/basho_%s_%d", my_basename(fileName), test_number);
	snprintf(err_42, PATH_MAX, "./data/42she_%s_%d", my_basename(fileName), test_number);
	snprintf(err_bash, PATH_MAX, "./data/bashe_%s_%d", my_basename(fileName), test_number);

	bool output_ok = compare_files(out_42, out_bash);
	bool error_ok = compare_files(err_42, err_bash);
	bool exit_ok = (exit_42sh == exit_bash);

	char *partial_result = calloc(2048, sizeof(char));

	int len = sprintf(partial_result, "\t\t\t\t{\n\t\t\t\t\t\"id\": \"%d\",\n\t\t\t\t\t\"input\": \"%s\",\n\t\t\t\t\t\"42sh_output\": \"%s\",\n\t\t\t\t\t\"bash_output\": \"%s\",\n" \
		"\t\t\t\t\t\"42sh_error\": \"%s\",\n\t\t\t\t\t\"bash_error\": \"%s\",\n\t\t\t\t\t\"42sh_exit_code\": \"%d\",\n\t\t\t\t\t\"bash_exit_code\": \"%d\",\n" \
		"\t\t\t\t\t\"output_ok\": \"%d\",\n\t\t\t\t\t\"error_ok\": \"%d\",\n\t\t\t\t\t\"exit_ok\": \"%d\"\n\t\t\t\t},\n"
		 , test_number, fileName, out_42, out_bash, err_42, err_bash, exit_42sh, exit_bash, output_ok, error_ok, exit_ok);
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
