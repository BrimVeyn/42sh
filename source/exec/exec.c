/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 15:59:07 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 10:18:38 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

// can have Prefix
// >&   X
// <&   X
// <<   
// <

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


char *find_bin_location(char *bin){
	char **env = __environ;
	for (int i = 0; env[i]; i++){
		if (ft_strncmp(env[i], "PATH=", 5) == 0){
			char **path = ft_split(env[i] + sizeof("PATH="), ':');
			for (int i = 0; path[i]; i++){
				char *bin_with_path = ft_strjoin(path[i], (char *)gc_add(ft_strjoin("/",bin)));
				if (access(bin_with_path, F_OK | X_OK) == 0){
					free_charchar(path);
					return (char *)gc_add(bin_with_path);
				}
				free(bin_with_path);
			}
			free_charchar(path);
		}
	}
	return bin;
}

void exec_simple_command(SimpleCommand *command) {
	printf("%s\n", find_bin_location("ls"));
    pid_t id[1024] = {0};
    int pipefd[2] = {-1, -1};
    int prev_pipefd = -1;
    int i = 0;
	
    while (command) {
        if (command->next) {
            secure_pipe2(pipefd, O_CLOEXEC);
        }
        id[i] = secure_fork();
        if (id[i] == 0) {
            if (i != 0) {
                secure_dup2(prev_pipefd, STDIN_FILENO);
            }
            if (command->next) {
                secure_dup2(pipefd[1], STDOUT_FILENO);
            }
            if (prev_pipefd != -1 && close(prev_pipefd)) {}
            if (pipefd[0] != -1 && close(pipefd[0])) {}
            secure_execve(find_bin_location(command->bin), command->args, __environ);
        }
        if (prev_pipefd != -1 && close(prev_pipefd)) {}
        if (pipefd[1] != -1 && close(pipefd[1])) {}
        prev_pipefd = pipefd[0];
        command = command->next;
        i++;
    }

    if (prev_pipefd != -1 && close(prev_pipefd)) {}

    int j = 0;
    while (j < i) {
        waitpid(id[j++], NULL, 0);
    }
}
