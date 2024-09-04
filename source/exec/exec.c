/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:19:22 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/04 15:29:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "../../include/42sh.h"
#include <sys/wait.h>
#include <unistd.h>

int g_exitno;

// can have Prefix
// >&   X
// <&   X
// <<   
// <
// R_DUP_BOTH, //&>[ex|n]
// R_DUP_BOTH_APPEND, //&>>[ex]

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


int there_is_slash(char *str){
	for (int i = 0; str[i]; i++){
		if (str[i] == '/'){
			return 0;
		}
	}
	return 1;
}

char *find_bin_location(char *bin, char **env){

	struct stat file_stat;
	if (stat(bin, &file_stat) != -1){

		if (S_ISDIR(file_stat.st_mode)){
			dprintf(2, "%s: Is a directory\n", bin);
			return NULL;
		}
		if (file_stat.st_mode & S_IXUSR){
			return bin;
		}
		else {
			dprintf(2, "%s: Permission Denied\n", bin);
			return NULL;
		}

	}
	// else if (there_is_slash(bin)){
	// 	dprintf(2, "%s: No such file or directory\n", bin);
	// 	return NULL;
	// }

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
	dprintf(2, "%s: Command not found\n", bin);
	return NULL;
}

int exec_init_subshell(void *line, bool has_pipe){
	(void)line;
	int pipefd[2];
	if (has_pipe == true){
		secure_pipe2(pipefd, O_CLOEXEC);
	}
	int id = secure_fork();
	if (id == 0){
		//call ast
		exit(EXIT_SUCCESS);
	}
	int exitno = 0;
	waitpid(id, &exitno, 0);
	return exitno;
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
		exit(EXIT_FAILURE);
		//fid_bin_location exit number
	} else {
		gc_cleanup();
		exit(EXIT_FAILURE);
	}
	printf("OULALALALALA\n");
	exit(EXIT_FAILURE);
}

int exec_executer(Executer *executer) {
	Executer *current = executer;
	int pipefd[2];
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
			pid_t id = secure_fork();
			if (id == 0) {
				g_exitno = ast_execute(current->n_data);
				exit (g_exitno);
			}
			waitpid(id, &g_exitno, 0);
		}

		if (current->data_tag == DATA_TOKENS) {
			id[i] = secure_fork();
			if (id[i] == 0) {
				SimpleCommand *command = parser_parse_current(current->s_data);
				exec_simple_command(command);
			}
		}

		secure_dup2(STDIN_SAVE, STDIN_FILENO);
		secure_dup2(STDOUT_SAVE, STDOUT_FILENO);
		secure_dup2(STDERR_SAVE, STDERR_FILENO);
		
		i++;
		current = current->next;
	}
	for (int j = 0; j < i; j++) {
		waitpid(id[j], &g_exitno, 0);
	}
	close(STDIN_SAVE);
	close(STDOUT_SAVE);
	close(STDERR_SAVE);
	return g_exitno;
}

int exec_node(Node *node, char **env) {
	(void) env;

	ExecuterList *list = build_executer_list(node->value.operand);
	// printf("tag = %d\n", list->data[0]->data_tag);

	for (int it = 0; it < list->size; it++) {
		Executer *executer = list->data[it];
		g_exitno = exec_executer(executer);
	}
	return g_exitno;
}
//
// int exec_node(SimpleCommand *command, char **env) {
// 	pid_t id[1024] = {0};
// 	int pipefd[2] = {-1, -1};
// 	int prev_pipefd = -1;
// 	int i = 0;
//
// 	while (command) {
// 		if (command->next) {
// 			secure_pipe2(pipefd, O_CLOEXEC);
// 		}
// 		id[i] = secure_fork();
// 		if (id[i] == 0) {
// 			if (i != 0) {
// 				secure_dup2(prev_pipefd, STDIN_FILENO);
// 			}
// 			if (command->next) {
// 				secure_dup2(pipefd[1], STDOUT_FILENO);
// 			}
// 			if (prev_pipefd != -1 && close(prev_pipefd)) {}
// 			if (pipefd[0] != -1 && close(pipefd[0])) {}
// 			if (apply_all_redirect(command->redir_list)) {
// 				if (!command->bin) {
// 					gc_cleanup();
// 					exit(EXIT_SUCCESS);
// 				}
// 				secure_execve(find_bin_location(command->bin, env), command->args, __environ);
// 			} else {
// 				gc_cleanup();
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 		if (prev_pipefd != -1 && close(prev_pipefd)) {}
// 		if (pipefd[1] != -1 && close(pipefd[1])) {}
// 		prev_pipefd = pipefd[0];
// 		command = command->next;
// 		i++;
// 	}
//
// 	if (prev_pipefd != -1 && close(prev_pipefd)) {}
//
// 	int j = 0;
// 	while (j < i) {
// 		waitpid(id[j++], NULL, 0);
// 	}
// 	return 1;
// }
