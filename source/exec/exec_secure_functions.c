/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_secure_functions.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:52:42 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/04 12:53:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int is_valid_fd(int fd) {
	return fcntl(fd, F_GETFL) != -1;
}        

bool error_bad_file_descriptor(int fd) {
	dprintf(STDERR_FILENO, "42sh: %d: Bad file descriptor\n", fd);
	return false;
}

bool secure_dup2(int from, int to) {
	if (!is_valid_fd(from)) return error_bad_file_descriptor(from);
	dup2(from, to);
	return true;
}

void secure_pipe2(int pipefd[2], int flags){
	if (pipe2(pipefd, flags) == -1){
		perror("Fatal error pipe2: ");
    gc_cleanup();
		exit(EXIT_FAILURE);
	}
}

void secure_execve(const char *pathname, char *const argv[], char *const envp[]){
	if (execve(pathname, argv, envp) == -1){
		printf("%s: command not found\n", argv[0]);
		gc_cleanup();
		exit(EXIT_FAILURE);
	}
}

int secure_fork(void){
	int id = fork();
	if (id == -1){
		perror("Fatal error execve: ");
    gc_cleanup();
		exit(EXIT_FAILURE);
	}
	return id;
}
