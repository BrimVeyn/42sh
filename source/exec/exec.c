/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:19:22 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 16:03:48 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

// can have Prefix
// >&   X
// <&   X
// <<   
// <
// R_DUP_BOTH, //&>[ex|n]
// R_DUP_BOTH_APPEND, //&>>[ex]

int is_valid_fd(int fd) {
  return fcntl(fd, F_GETFL) != -1;
}        

bool error_bad_file_descriptor(int fd) {
    dprintf(STDERR_FILENO, "42sh: %d: Bad file descriptor\n", fd);
    return false;
}

//vrai secure_dup2 LARBIN
bool really_secure_dup2(int from, int to) {
  if (!is_valid_fd(from)) return error_bad_file_descriptor(from);
  dup2(from, to);
  return true;
}


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
		// perror("Can't open file");
		exit(EXIT_FAILURE);
	}
	
	printf("dup %d on %d\n", fd, dup_on);

	if (redirect.r_type == R_DUP_BOTH || redirect.r_type == R_DUP_BOTH_APPEND){
    if (!really_secure_dup2(fd, STDOUT_FILENO)) return false;
    if (!really_secure_dup2(fd, STDERR_FILENO)) return false;
	}
	else{
    if (!really_secure_dup2(fd, dup_on)) return false;
	}
	if (redirect.su_type == R_FILENAME){
    printf("close %d\n", fd);
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
      if (apply_all_redirect(command->redir_list)) {
        if (!command->bin) {
          gc_cleanup();
          exit(EXIT_SUCCESS);
        }
        secure_execve(find_bin_location(command->bin), command->args, __environ);
      } else {
        gc_cleanup();
        exit(EXIT_FAILURE);
      }
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
