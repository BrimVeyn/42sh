#include "exec.h"
#include "expansion.h"
#include "final_parser.h"
#include "libft.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

char *process_substitution(char *const str, const ExpKind kind, Vars *const shell_vars, int *const error) {
	char file_name[] = "/tmp/process_sub_XXXXXX";

	int fd = mkstemp(file_name);
	if (fd == -1)
		_fatal("mkstemp: failed", 1);
	close(fd);
	unlink(file_name);


	if (mkfifo(file_name, 0666) == -1)
		_fatal("mkfifo: failed", 1);

	pid_t pid = fork();
	if (pid == -1) {
		unlink(file_name); // Clean up FIFO
		_fatal("fork: failed", 1);
	}
	int fifo_fd = open(file_name, O_RDWR);

	if (IS_CHILD(pid)) {
		// Set process group for job control
		if (setpgid(0, 0) == -1) {
			perror("setpgid");
			unlink(file_name);
			_fatal("setpgid: failed", 1);
		}

		// Ignore SIGTTOU to prevent stopping
		signal(SIGTTOU, SIG_IGN);

		if (kind == EXP_PROC_SUB_OUT) {
			// Child will read from FIFO
			if (fifo_fd == -1 || dup2(fifo_fd, STDIN_FILENO) == -1) {
				perror("open");
				unlink(file_name);
				_fatal("open: failed", 1);
			}
			close(fifo_fd);

		} else if (kind == EXP_PROC_SUB_IN) {
			// Child will write to FIFO
			int fifo_fd = open(file_name, O_RDWR);
			if (fifo_fd == -1 || dup2(fifo_fd, STDOUT_FILENO) == -1) {
				perror("open");
				unlink(file_name);
				_fatal("open: failed", 1);
			}
			close(fifo_fd);
		}

		// Execute the provided command
		shell(SHELL_GET)->interactive = false;
		parse_input(str, "process_sub", shell_vars);
		exit_clean();
	} else {
		close(fifo_fd);
	}
	
	(void)error;

	// Parent process returns the FIFO path for substitution
	return ft_strdup(file_name);
}


