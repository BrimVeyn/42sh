/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:20:10 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/20 16:03:48 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>

int g_exitno;

//LARBIN add FD validity check (>1023)
bool apply_redirect(const Redirection redirect) {
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
		return false;
	}

	if (dup_on >= 1024){
		dprintf(2, "42sh: %d: Bad file descriptor\n", dup_on);
		return false;
	}
	if (fd >= 1024){
		dprintf(2, "42sh: %d: Bad file descriptor\n", fd);
		return false;
	}

	if (redirect.r_type == R_DUP_BOTH || redirect.r_type == R_DUP_BOTH_APPEND) {
		if (!secure_dup2(fd, STDOUT_FILENO)) return false;
		if (!secure_dup2(fd, STDERR_FILENO)) return false;
	}
	else {
		if (!secure_dup2(fd, dup_on)) return false;
	}
	if (redirect.su_type == R_FILENAME){
		close(fd);
	}
	return true;
}

bool apply_all_redirect(RedirectionList *redirections) {
	for (int i = 0; redirections->r[i]; i++) {
		if (!apply_redirect(*redirections->r[i])) {
			return false;
		}
	}
	return true;
}

char *find_bin_location(char *bin, StringList *env){
	struct stat file_stat;
	if (stat(bin, &file_stat) != -1) {

		if (S_ISDIR(file_stat.st_mode)) {
			dprintf(2, "%s: Is a directory\n", bin);
			g_exitno = 126;
			return (NULL);
		}
		if (file_stat.st_mode & S_IXUSR) {
			return bin;
		}
		else {
			dprintf(2, "%s: Permission Denied\n", bin);
			g_exitno = 126;
			return (NULL);
		}

	}
	else if (stat(bin, &file_stat) == -1 && there_is_slash(bin)) {
		dprintf(2, "%s: No such file or directory\n", bin);
		g_exitno = 127;
		return (NULL);
	}

	char **path = ft_split(string_list_get_value_with_id(env, "PATH"), ':');
	for (int i = 0; path[i]; i++) {
		char *bin_with_path = ft_strjoin(path[i], (char *)gc_add(ft_strjoin("/",bin), GC_GENERAL));

		struct stat file_stat;
		if (stat(bin_with_path, &file_stat) != -1){
			if (file_stat.st_mode & S_IXUSR){
				free_charchar(path);
				return (char *)gc_add(bin_with_path, GC_GENERAL);
			}
		}
		free(bin_with_path);
		bin_with_path = NULL;
	}
	free_charchar(path);
	dprintf(2, "%s: command not found\n", bin);
	g_exitno = 127;
	return NULL;
}

bool exec_simple_command(const SimpleCommand *command, Vars *shell_vars) {
	if (apply_all_redirect(command->redir_list)) {
		if (!command->bin) {
			g_exitno = EXIT_SUCCESS;
			return false;
		}
		char *path = find_bin_location(command->bin, shell_vars->env);
		if (!path) return false;
		secure_execve(path, command->args, shell_vars->env->value);
	}
	else return false;
	return true;
}

bool exec_builtin(SimpleCommand *command, Vars *shell_vars){
	if (!command->bin){
		return false;
	}
	if (!ft_strcmp(command->bin, "exit")){
		exit(g_exitno);
	}
	if (!ft_strcmp(command->bin, "env")) {
		builtin_env(shell_vars);
		return true;
	}
	if (!ft_strcmp(command->bin, "set")) {
		builtin_set(shell_vars);
		return true;
	}
	return false;
}

void close_saved_fds(int *saved_fds) {
	if (saved_fds[STDIN_FILENO] != -1) {
		close(saved_fds[STDIN_FILENO]);
		saved_fds[STDIN_FILENO] = -1;
	}	
	if (saved_fds[STDOUT_FILENO] != -1) {
		close(saved_fds[STDOUT_FILENO]);
		saved_fds[STDOUT_FILENO] = -1;
	}
	if (saved_fds[STDERR_FILENO] != -1) {
		close(saved_fds[STDERR_FILENO]);
		saved_fds[STDERR_FILENO] = -1;
	}
}

void close_all_fds(void) {
	for (uint16_t i = 3; i < 1024; i++) {
		close(i);
	}
}

void close_std_fds(void) {
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int exec_executer(Executer *executer, Vars *shell_vars) {
	Executer *current = executer;
	int pipefd[2] = {-1 , -1};
	int saved_fds[3] = {-1, -1, -1};
	pid_t pids[1024];
	int i = 0;

	saved_fds[STDIN_FILENO] = dup(STDIN_FILENO);
	saved_fds[STDOUT_FILENO]= dup(STDOUT_FILENO);
	saved_fds[STDERR_FILENO] = dup(STDERR_FILENO);

	bool prev_pipe = false;

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
			if (current->n_data->tree_tag == TREE_COMMAND_GROUP) {
				if (current->n_data->redirs != NULL) {
					if (apply_all_redirect(current->n_data->redirs)) {
						g_exitno = ast_execute(current->n_data, shell_vars);
					}
				} else {
					g_exitno = ast_execute(current->n_data, shell_vars);
				}
			} else if (current->n_data->tree_tag == TREE_SUBSHELL) {
				pids[i] = secure_fork();
				if (pids[i] == 0) {
					close_all_fds();
					if (current->n_data->redirs != NULL) {
						if (!apply_all_redirect(current->n_data->redirs)) {
							gc_cleanup(GC_SUBSHELL);
							exit(g_exitno);
						}
					}
					g_exitno = ast_execute(current->n_data, shell_vars);
					//LE BUG VENAIT DE LA LARBIN
					gc_cleanup(GC_SUBSHELL);
					free(gc_get()[GC_GENERAL].garbage);
					//MERDE !
					close_std_fds();
					exit(g_exitno);
				}
			}
		}

		if (current->data_tag == DATA_TOKENS) {
			SimpleCommand *command = parser_parse_current(current->s_data, shell_vars);
			// printCommand(command);
			if (!command && pipefd[0] == -1){
				close_all_fds();
				return false;
			}
			if (pipefd[0] == -1 && exec_builtin(command, shell_vars)){
				break;
			}
			pids[i] = secure_fork();
			if (pids[i] == 0) {
				close_all_fds();
				if (!exec_simple_command(command, shell_vars)) {
					gc_cleanup(GC_ALL);
					exit(g_exitno);
				}
			}
		}

		dup2(saved_fds[STDIN_FILENO], STDIN_FILENO);
		dup2(saved_fds[STDOUT_FILENO], STDOUT_FILENO);
		dup2(saved_fds[STDERR_FILENO], STDERR_FILENO);
		
		//increment pid only if it isnt a command group
		i += !(current->data_tag == DATA_NODE && current->n_data->tree_tag == TREE_COMMAND_GROUP);
		current = current->next;
	}

	for (int j = 0; j < i; j++) {
		int status = 0;
		waitpid(pids[j], &status, 0);
		g_exitno = WEXITSTATUS(status);
	}

	close_saved_fds(saved_fds);
	return true;
}

int exec_node(Node *node, Vars *shell_vars) {
	const ExecuterList *list = build_executer_list(node->value.operand);

	for (int it = 0; it < list->size; it++) {
		Executer *executer = list->data[it];
		if (exec_executer(executer, shell_vars) == false) {
			break;
		}
	}
	return g_exitno;
}
