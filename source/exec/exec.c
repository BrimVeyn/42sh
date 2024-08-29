/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 15:59:07 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/28 16:53:19 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

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

void apply_all_redirect(RedirectionList *redirections){
	for (int i = 0; redirections->r[i]; i++){
		apply_redirect(*redirections->r[i]);
	}
}

void exec_simple_command(SimpleCommand *command){
	type_of_separator next_separator = interface_separator(NULL, I_READ);
	pid_t id[1024];
	int i = 0;
	int pipefd[2];

	if (true){
		if (command->next)
			secure_pipe2(pipefd, O_CLOEXEC);
		while(command){
			id[i] = secure_fork();
			if (id[i] == 0){
				if (i != 0)
					secure_dup2(pipefd[0], STDIN_FILENO);
				if (command->next != NULL)
					secure_dup2(pipefd[1], STDOUT_FILENO);
				apply_all_redirect(command->redir_list);
				secure_execve(command->bin, command->args, __environ);
			}
			command = command->next;
			i++;
		}

		int j = 0;
		while(j < i){
			waitpid(id[j++], NULL, 0);
		}
	}
}
