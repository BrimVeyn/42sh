/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   secure_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:52:42 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 15:16:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void secure_dup2(int oldfd, int newfd){
	if (dup2(oldfd, newfd) == -1){
		perror("Fatal error dup2: ");
		exit(EXIT_FAILURE);
	}
}

void secure_pipe2(int pipefd[2], int flags){
	if (pipe2(pipefd, flags) == -1){
		perror("Fatal error pipe2: ");
		exit(EXIT_FAILURE);
	}
}

void secure_execve(const char *pathname, char *const argv[], char *const envp[]){
	if (execve(pathname, argv, envp) == -1){
		perror("Fatal error execve: ");
		exit(EXIT_FAILURE);
	}
}

int secure_fork(void){
	int id = fork();
	if (id == -1){
		perror("Fatal error execve: ");
		exit(EXIT_FAILURE);
	}
	return id;
}
