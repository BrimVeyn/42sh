#include "dynamic_arrays.h"
#include "exec.h"
#include "expansion.h"
#include "final_parser.h"
#include "libft.h"
#include "signals.h"
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
    int pipe_fd[2]; // Array to hold the pipe file descriptors
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        _fatal("pipe: failed", 1);
    }
	(void)error;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        _fatal("fork: failed", 1);
    }

    if (IS_CHILD(pid)) {
        // Child process
		close_fd_set(FD_ALL);
		signal_manager(SIG_EXEC);

        if (kind == EXP_PROC_SUB_OUT) {
            // Redirect child process's output to the writing end of the pipe
            close(pipe_fd[1]); // Close reading end
            if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
                perror("dup2");
                close(pipe_fd[1]);
                _fatal("dup2: failed", 1);
            }
			/*int fdnull = open("/dev/null", 0666);*/
			/*dup2(fdnull, );*/
			/*close(fdnull);*/
            close(pipe_fd[0]); // No longer needed

        } else if (kind == EXP_PROC_SUB_IN) {
            // Redirect child process's input from the reading end of the pipe
            close(pipe_fd[0]); // Close writing end
            if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
                perror("dup2");
                close(pipe_fd[0]);
                _fatal("dup2: failed", 1);
            }
            close(pipe_fd[1]); // No longer needed
        }

        // Execute the provided command
        shell(SHELL_GET)->interactive = false;
        parse_input(str, "process_sub", shell_vars);
        exit_clean();
    }

	char buffer[100] = {0};
    // Parent process
    if (kind == EXP_PROC_SUB_OUT) {
        // Parent will read from the pipe
        close(pipe_fd[0]); // Close writing end
		ft_sprintf(buffer, "/dev/fd/%d", pipe_fd[1]); // Return file descriptor as a string
		da_push(g_fdSet, fd_init(pipe_fd[1], FD_PARENT));
		return ft_strdup(buffer);
    } else if (kind == EXP_PROC_SUB_IN) {
        // Parent will write to the pipe
        close(pipe_fd[1]); // Close writing end
        ft_sprintf(buffer, "/dev/fd/%d", pipe_fd[0]); // Return file descriptor as a string
		da_push(g_fdSet, fd_init(pipe_fd[0], FD_PARENT));
		return ft_strdup(buffer);
    }

    // If something unexpected happens, clean up
	close(pipe_fd[1]);
	close(pipe_fd[0]);
    return NULL;
}



