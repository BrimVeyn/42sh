/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:37:43 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/27 15:29:39 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

#include "../lexer/lexer.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

SimpleCommand init_test_simple_command(void){
	SimpleCommand command;
	command.bin = "/bin/cat";
	command.args = calloc(3, sizeof(char *));
	command.args[0] = "/bin/cat";
	// command.args[1] = "-la";
	return command;
}

Redirection init_test_redirect(void){
	Redirection redirect;
	redirect.filename = "Makefile";
	redirect.su_type = R_FILENAME;
	redirect.r_type = R_INPUT;
	return redirect;
}


void apply_redirect(const Redirection redirect){
	int fd = redirect.fd;
	const int open_flag = (redirect.r_type == R_INPUT) * (O_RDONLY)
		+ (redirect.r_type == R_OUTPUT) * (O_WRONLY | O_TRUNC | O_CREAT)
		+ (redirect.r_type == R_APPEND) * (O_WRONLY | O_CREAT);

	const int dup_on = (redirect.r_type == R_INPUT) * STDIN_FILENO
		+ (redirect.r_type == R_OUTPUT || redirect.r_type == R_APPEND) * STDOUT_FILENO; 

	if ((fd = open(redirect.filename, open_flag)) == -1){
		perror("Can't open file:");
		exit(EXIT_FAILURE);
	}
	dup2(fd, dup_on);
	if (redirect.su_type == R_FILENAME){
		close(fd);
	}
}

RedirectionList *redirection_list_init(void);
void redirection_list_add(RedirectionList *rl, Redirection *redirection);
void redirection_list_prepend(RedirectionList *rl, Redirection *redirection);

RedirectionList *redirection_list_init(void) {
	RedirectionList *self = gc_add(ft_calloc(1, sizeof(RedirectionList)));
	RedirectionList rl = {
		.r = gc_add(ft_calloc(10, sizeof(Redirection *))),
		.size = 0,
		.capacity = 10,
	};
	*self = rl;
	return self;
}

void redirection_list_add(RedirectionList *rl, Redirection *redirection) {
	if (rl->size >= rl->capacity) {
		rl->capacity *= 2;
		rl->r = ft_realloc(rl->r, rl->size, rl->capacity, sizeof(Redirection *));
	}
	rl->r[rl->size] = redirection;
	rl->size += 1;
}

void redirection_list_prepend(RedirectionList *rl, Redirection *redirection){
	if (rl->size >= rl->capacity) {
		rl->capacity *= 2;
		rl->r = ft_realloc(rl->r, rl->size, rl->capacity, sizeof(Redirection *));
	}
	rl->size += 1;
	for (int i = rl->size; i > 0; i--){
		rl->r[i] = rl->r[i - 1];
	}
	rl->r[0] = redirection;
}

void apply_all_redirect(RedirectionList redirections){
	for (int i = 0; redirections.r[i]; i++){
		apply_redirect(*redirections.r[i]);
	}
}

void exec_simple_command(SimpleCommand command){
	execve(command.bin, command.args, __environ);
}

int main (void){
	int id = 0;
	if ((id = fork()) == -1){
		perror("Fork failed:");
		exit(EXIT_FAILURE);
	}
	if (id != 0){
		waitpid(id, NULL, 0);
	} 
	else{
		apply_redirect(init_test_redirect());
		exec_simple_command(init_test_simple_command());
	}
	return 0;
}
