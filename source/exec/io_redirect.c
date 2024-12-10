/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_redirect.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 14:24:27 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/10 15:40:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "exec.h"
#include "final_parser.h"
#include "signals.h"
#include "utils.h"
#include "libft.h"
#include "lexer.h"

#define MAX_FD 1024
#include <sys/stat.h>

typedef enum {
	F_ISDIR = 0b000001,
	F_EXEC =  0b000010,
	F_EXIST = 0b000100,
	F_WRITE = 0b001000,
	F_READ =  0b010000,
	F_POLY = 0b100000, //can be a number
} FilepathMode;

bool check_filepath(char *file_path, FilepathMode mode) {
	if (is_number(file_path) && mode & F_POLY)
		return true;
	struct stat file_stat;
	if (stat(file_path, &file_stat) != -1) {

		if (mode & F_ISDIR && S_ISDIR(file_stat.st_mode)) {
			ft_dprintf(STDERR_FILENO, "42sh: %s: Is a directory\n", file_path);
			g_exitno = 1;
			return false;
		}
		if ((!(file_stat.st_mode & S_IRUSR) && mode & F_READ) || 
			(!(file_stat.st_mode & S_IWUSR) && mode & F_WRITE) ||
			(!(file_stat.st_mode & S_IXUSR) && mode & F_EXEC )){
			ft_dprintf(STDERR_FILENO, "42sh: %s: Permission Denied\n", file_path);
			g_exitno = 1;
			return false;
		}
		return file_path;
	} else if (mode & F_EXIST){
		ft_dprintf(STDERR_FILENO, "42sh: %s: No such file or directory\n", file_path);
		g_exitno = 1;
		return false;
	}
	return true;
}

int get_filepath_mode(TokenType type){
	if (type == LESS)
		return F_EXIST | F_READ | F_ISDIR;
	else if (type == GREAT || type == DGREAT || type == CLOBBER)
		return F_WRITE | F_ISDIR;
	else if (type == LESSGREAT)
		return F_WRITE | F_READ | F_ISDIR;
	else if (type == GREATAND)
		return F_WRITE | F_ISDIR | F_POLY;
	else if (type == LESSAND)
		return F_READ | F_EXIST | F_ISDIR | F_POLY;
	return 0;
}

static void expand_filenames(RedirectionL *const redir_list, Vars *const shell_vars) {
	da_create(tmp_redirs, StringListL, sizeof(char *), GC_SUBSHELL);
	for (size_t i =0; i < redir_list->size; i++) {
		da_push(tmp_redirs, redir_list->data[i]->filename);
	}
	StringListL *result = do_expansions(tmp_redirs, shell_vars, O_NONE);
	for (size_t i = 0; i < redir_list->size; i++) {
		redir_list->data[i]->filename = result->data[i];
	}
}


//TODO:change redir->filename to redir->word
bool redirect_ios(RedirectionL * const redir_list, Vars * const shell_vars) {
	if (!redir_list) {
		return true;
	}

	expand_filenames(redir_list, shell_vars);
	// print_redir_list(redir_list);

	for (size_t i = 0; i < redir_list->size; i++) {
		const RedirectionP *redir = redir_list->data[i];
		const bool has_io_number = (redir->io_number != NULL);
		int io_number = (has_io_number) ? ft_atoi(redir->io_number) : -1;
		if (io_number >= MAX_FD) {
			error("42sh: %d: Bad file descriptor", 1);
			return false;
		}
		//TODO: error managment when open fails
		
		int mode = get_filepath_mode(redir->type);
		if (!check_filepath(redir->filename, mode)) {
			g_exitno = 1;
			return false;
		}

		if (redir->type == GREAT || redir->type == CLOBBER){ //>
			const int open_flags = (O_CREAT | O_TRUNC | O_WRONLY);
			const int fd = open(redir->filename, open_flags, 0664);
			int dup_to = STDOUT_FILENO;
			if (io_number != -1)
				dup_to = io_number;
			if (!secure_dup2(fd, dup_to))
				return false;
			close(fd);
		}
		else if (redir->type == DGREAT){ //>>
			const int open_flags = (O_CREAT | O_APPEND | O_WRONLY);
			const int fd = open(redir->filename, open_flags, 0664);
			int dup_to = STDOUT_FILENO;
			if (io_number != -1)
				dup_to = io_number;
			if (!secure_dup2(fd, dup_to))
				return false;
			close(fd);
		}
		else if (redir->type == LESSGREAT){ //<>
			const int n = (io_number == -1) ? STDIN_FILENO : io_number;
			const int open_flags = (O_CREAT | O_RDWR);
			const int fd = open(redir->filename, open_flags, 0664);
			if (!secure_dup2(fd, n))
				return false;
			close(fd);
		}
		else if (redir->type == LESS){ // <
			const int open_flags = (O_RDONLY);
			int dup_to = STDIN_FILENO;
			const int fd = open(redir->filename, open_flags, 0664);
			if (io_number != -1) {
				dup_to = io_number;
			}
			if (!secure_dup2(fd, dup_to))
				return (g_exitno = 1, false);
			close(fd);
		}
		else if (redir->type == LESSAND){ // <&
			//
			if (!is_number(redir->filename) && ft_strcmp(redir->filename, "-")){
				return false;
			}

			const int n = (io_number == -1) ? STDIN_FILENO : io_number;

			const int fd = ft_atoi(redir->filename);
			if (fd < 0 || fcntl(fd, F_GETFD) == -1) {
				ft_dprintf(STDERR_FILENO, "42sh: %d: Bad file descriptor\n", fd);
				return false;
			}

			if (!ft_strcmp("-", redir->filename) || fd == n){
				close(n);
			} else {
				if (!secure_dup2(fd, n)){
					return false;
				}
				close(fd);
			}
		}
		else if (redir->type == GREATAND){ //>&
			// if (!is_number(redir->filename) && ft_strcmp(redir->filename, "-")){
			// 	return false;
			// }

			const int n = (io_number == -1) ? STDOUT_FILENO : io_number;
			const int open_flags = O_CREAT | O_TRUNC | O_WRONLY;

			const int fd = (is_number(redir->filename)) ? ft_atoi(redir->filename) : open(redir->filename, open_flags, 0664);
			if (fd < 0 || fcntl(fd, F_GETFL) == -1) {
				ft_dprintf(STDERR_FILENO, "42sh: %d: Bad file descriptor\n", fd);
				return false;
			}

			if (!ft_strcmp("-", redir->filename) || fd == n){
				close(n);
			} else {
				if (!secure_dup2(fd, n)){
					return false;
				}
				close(fd);
			}
		}
	}
	return true;
}
