/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 09:53:13 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 15:31:54 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "../../include/42sh.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int g_exitno;

bool apply_redirect(const Redirection redirect){
	int fd = redirect.fd;
	const int open_flag = (redirect.r_type == R_INPUT) * (O_RDONLY)
		+ (redirect.r_type == R_OUTPUT || redirect.r_type == R_DUP_BOTH) * (O_WRONLY | O_TRUNC | O_CREAT)
		+ (redirect.r_type == R_APPEND || redirect.r_type == R_DUP_BOTH_APPEND) * (O_WRONLY | O_CREAT | O_APPEND);

	int dup_on = (redirect.r_type == R_INPUT || redirect.r_type == R_DUP_IN) * STDIN_FILENO
		+ (redirect.r_type == R_OUTPUT || redirect.r_type == R_APPEND || redirect.r_type == R_DUP_OUT) * STDOUT_FILENO;

	if (redirect.prefix_fd != -1){
		dup_on = redirect.prefix_fd;
	}

	if (redirect.su_type == R_FILENAME && (fd = open(redirect.filename, open_flag, 0664)) == -1){
		perror(strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (redirect.r_type == R_DUP_BOTH || redirect.r_type == R_DUP_BOTH_APPEND){
		if (!secure_dup2(fd, STDOUT_FILENO)) return false;
		if (!secure_dup2(fd, STDERR_FILENO)) return false;
	}
	else{
		if (!secure_dup2(fd, dup_on)) return false;
	}
	if (redirect.su_type == R_FILENAME){
		close(fd);
	}
	return true;
}

bool apply_all_redirect(RedirectionList *redirections){
	for (int i = 0; redirections->r[i]; i++){
		if (!apply_redirect(*redirections->r[i])) {
			return false;
		}
	}
	return true;
}

char *find_bin_location(char *bin, char **env){
	struct stat file_stat;
	if (stat(bin, &file_stat) != -1){

		if (S_ISDIR(file_stat.st_mode)){
			dprintf(2, "%s: Is a directory\n", bin);
			g_exitno = 126;
			return (NULL);
		}
		if (file_stat.st_mode & S_IXUSR){
			return bin;
		}
		else {
			dprintf(2, "%s: Permission Denied\n", bin);
			g_exitno = 126;
			return (NULL);
		}

	}
	else if (stat(bin, &file_stat) == -1 && there_is_slash(bin)){
		dprintf(2, "%s: No such file or directory\n", bin);
		g_exitno = 127;
		return (NULL);
	}

	for (int i = 0; env[i]; i++){
		if (ft_strncmp(env[i], "PATH=", 5) == 0){
			char **path = ft_split(env[i] + sizeof("PATH="), ':');
			for (int i = 0; path[i]; i++){
				char *bin_with_path = ft_strjoin(path[i], (char *)gc_add(ft_strjoin("/",bin)));

				struct stat file_stat;
				if (stat(bin_with_path, &file_stat) != -1){
					if (file_stat.st_mode & S_IXUSR){
						free_charchar(path);
						return (char *)gc_add(bin_with_path);
					}
				}
				free(bin_with_path);
			}
			free_charchar(path);
		}
	}
	dprintf(2, "%s: command not found\n", bin);
	g_exitno = 127;
	return NULL;
}

void exec_simple_command(SimpleCommand *command) {
	if (apply_all_redirect(command->redir_list)) {
		if (!command->bin) {
			gc_cleanup();
			exit(EXIT_SUCCESS);
		}
		char *path = find_bin_location(command->bin, __environ);
		if (path != NULL){
			secure_execve(path, command->args, __environ);
		}
		gc_cleanup();
	}
	else
		gc_cleanup();
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(g_exitno);
}

int exec_builtin(SimpleCommand *command, char **env){
	(void)env;
	if (!ft_strcmp(command->bin, "exit")){
		exit(g_exitno);
	}
	return 0;
}

int exec_executer(Executer *executer, char **env) {
	Executer *current = executer;
	int pipefd[2] = {-1 , -1};
	bool prev_pipe = false;
	pid_t id[1024];
	int i = 0;

	const int STDIN_SAVE = dup(STDIN_FILENO);
	const int STDOUT_SAVE = dup(STDOUT_FILENO);
	const int STDERR_SAVE = dup(STDERR_FILENO);

	while (current) {

		if (prev_pipe) {
			secure_dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
		}
		if (current->next) {
			prev_pipe = true;
			secure_pipe2(pipefd, O_CLOEXEC);
			secure_dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
		}

		if (current->data_tag == DATA_NODE) {
			id[i] = secure_fork();
			if (id[i] == 0) {
				gc_addcharchar(env);
				g_exitno = ast_execute(current->n_data, env);
				close(STDIN_SAVE); close(STDOUT_SAVE); close(STDERR_SAVE);
				close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);
				gc_cleanup();
				exit (g_exitno);
			}
			int exitn = 0;
			waitpid(id[i], &exitn, 0);
			g_exitno = WEXITSTATUS(exitn);
		}

		if (current->data_tag == DATA_TOKENS) {
			{
				SimpleCommand *command = parser_parse_current(current->s_data);
				if (!command && pipefd[0] == -1){
					close(STDIN_SAVE);
					close(STDOUT_SAVE);
					close(STDERR_SAVE);
					return false;
				}
				if (pipefd[0] == -1 && exec_builtin(command, env)){
					break;
				}
				id[i] = secure_fork();
				if (id[i] == 0) {
					gc_addcharchar(env);
					close(STDIN_SAVE);
					close(STDOUT_SAVE);
					close(STDERR_SAVE);
					exec_simple_command(command);
				}
			}
		}

		secure_dup2(STDIN_SAVE, STDIN_FILENO);
		secure_dup2(STDOUT_SAVE, STDOUT_FILENO);
		secure_dup2(STDERR_SAVE, STDERR_FILENO);
		
		i++;
		current = current->next;
	}
	close(STDIN_SAVE);
	close(STDOUT_SAVE);
	close(STDERR_SAVE);
	for (int j = 0; j < i; j++){
		int exitn = 0;
		waitpid(id[j], &exitn, 0);
		g_exitno = WEXITSTATUS(exitn);
	}
	return true;
}

int exec_node(Node *node, char **env) {
	(void) env;

	ExecuterList *list = build_executer_list(node->value.operand);

	for (int it = 0; it < list->size; it++) {
		Executer *executer = list->data[it];
		if (exec_executer(executer, env) == false) {
			break;
		}
	}
	return g_exitno;
}
