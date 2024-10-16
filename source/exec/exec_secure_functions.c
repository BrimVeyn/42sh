/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_secure_functions.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:52:42 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/17 12:12:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#define _GNU_SOURCE
#include <unistd.h>
#include "exec.h"
#include "utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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
    gc(GC_CLEANUP, GC_ALL);
		exit(EXIT_FAILURE);
	}
}

void secure_execve(const char *pathname, char **const argv, char **const envp){
	if (execve(pathname, argv, envp) == -1){
		dprintf(2, "%s: command not found\n", argv[0]);
		gc(GC_CLEANUP, GC_ALL);
		exit(EXIT_FAILURE);
	}
}

int secure_fork(void){
	int id = fork();
	if (id == -1){
		perror("Fatal error execve: ");
    gc(GC_CLEANUP, GC_ALL);
		exit(EXIT_FAILURE);
	}
	return id;
}
