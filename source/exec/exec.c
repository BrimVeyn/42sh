/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 11:37:43 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/27 13:44:54 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef enum {
	R_INPUT, //[?n]<[ex|n]
	R_OUTPUT, //[?n]>[ex|n]
	R_APPEND, //[?n]>>[ex|n]
	R_HERE_DOC, //[?n]<<[delimiter]
	R_DUP_IN, //[?n]<&[ex|n]
	R_DUP_OUT, //[?n]>&[ex|n]
	R_DUP_BOTH, //&>[ex|n]
	R_DUP_BOTH_APPEND, //&>>[ex]
} type_of_redirection;

typedef struct {
	char *bin;
	char **args;
} SimpleCommand;

typedef enum {
	R_FD,
	R_FILENAME,
} type_of_suffix;

typedef struct {
	int prefix_fd;
	type_of_redirection r_type;
	type_of_suffix su_type;
	union {
		int fd;
		char *filename;
	};
} Redirection;

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

void apply_redirect(Redirection redirect){
	int fd = redirect.fd;
	if (redirect.r_type == R_INPUT){
		if (redirect.su_type == R_FILENAME){
			if ((fd = open(redirect.filename, O_RDONLY)) == -1){
				perror("Can't open file:");
				exit(EXIT_FAILURE);
			}
		}

		dup2(fd, STDIN_FILENO);
		
		if (redirect.su_type == R_FILENAME){
			close(fd);
		}
	}
	else if (redirect.r_type == R_OUTPUT){

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
